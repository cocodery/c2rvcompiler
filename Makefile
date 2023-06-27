TOPNAME 		:= compiler
BUILD_DIR 		:= build
BINARY 			:= $(BUILD_DIR)/$(TOPNAME)
SINGLE_TEST		:= main

# base dev
CMAKE 			:= cmake
GDB 			:= gdb
DIFF 			:= diff
ECHO			:= echo
TMP				:= /tmp


# llvm toolchain
LLDB 			:= lldb
LLLD 			:= llvm-link
LLI 			:= lli
FORMATTER		:= clang-format
CLANG			:= $(shell which clang)
CLANGXX			:= $(shell which clang++)


# rv toolchain
ARCH			:= riscv64
RVCC_linux		:= $(ARCH)-linux-gnu-gcc
RVOD_linux		:= $(ARCH)-linux-gnu-objdump
RVGDB_linux		:= $(ARCH)-linux-gnu-gdb
RVCC_elf		:= $(ARCH)-elf-gcc
RVOD_elf		:= $(ARCH)-elf-objdump
QEMU			:= qemu-$(ARCH)
SIM_CMD			:= $(QEMU)


# python test
PY				:= python
PYTEST			:= runtest.py

# make build dir
$(shell mkdir -p $(BUILD_DIR))

OS				:= $(shell uname)
NPROC			:= $(shell nproc)
ANTLR_SRC		:= $(shell find antlr -name '*.cpp' -or -name '*.h')
PROJECT_SRC		:= $(shell find 3tle3wa -name '*.cpp' -or -name '*.hh')
ALL_SRC			:= ${ANTLR_SRC} ${PROJECT_SRC}

MODE 			?= functional hidden_functional performance final_performance
SMODE			?= hidden_functional

CPLER_TEST_DIR	:= compiler2022
TEST_DIR 		:= $(CPLER_TEST_DIR)/公开样例与运行时库
TEST_DIRS		:= $(addprefix $(TEST_DIR)/,$(MODE))
TEST_CASES		:= $(shell find $(TEST_DIRS) -name "*.sy")

SYLIB_C			:= $(TEST_DIR)/sylib.c
SYLIB_H			:= $(TEST_DIR)/sylib.h
SYLIB_A			:= $(TEST_DIR)/libsysy.a
SYLIB_LL		:= sylib.ll

OUTPUT_ASM 		:= $(addsuffix .s,$(basename $(TEST_CASES)))
OUTPUT_RES 		:= $(addsuffix .res,$(basename $(TEST_CASES)))
OUTPUT_LOG 		:= $(addsuffix .log,$(basename $(TEST_CASES)))
OUTPUT_IR  		:= $(addsuffix .ll,$(basename $(TEST_CASES)))

# make python test llvmir fake targets
PYLL			:= pyll
PYLL_TARGETS	:= $(addprefix $(PYLL)/,$(MODE))

# make python test asm fake targets
PYASM			:= pyasm
PYASM_TARGETS	:= $(addprefix $(PYASM)/,$(MODE))

$(SYLIB_LL): $(SYLIB_C) $(SYLIB_H)
	@$(CLANG) -emit-llvm -S $(SYLIB_C) -I $(SYLIB_H) -o $@

$(PYLL_TARGETS): $(PYLL)/%:$(TEST_DIR)/% $(SYLIB_LL)
	@$(PY) $(PYTEST) -l -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) $(sort $(shell find $< -name "*.sy")) -m "$(LLI)" -s $(SYLIB_LL) -x $(LLLD)

$(PYASM_TARGETS): $(PYASM)/%:$(TEST_DIR)/%
	@$(PY) $(PYTEST) -a -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) $(sort $(shell find $< -name "*.sy")) -m "$(SIM_CMD)" -s $(SYLIB_C) -x $(RVCC_linux)

.PHONY: pyll
pyll:  build $(PYLL_TARGETS)

.PHONY: pyasm
pyasm: build $(PYASM_TARGETS)

release: $(ALL_SRC)
	$(CMAKE) -S . -B $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR) -j$(NPROC) -s

CMAKE_BUILD_ENV	:= -DCMAKE_C_COMPILER:FILEPATH=$(CLANG) -DCMAKE_CXX_COMPILER:FILEPATH=$(CLANGXX)

debug: $(ALL_SRC)
	$(CMAKE) -DCMAKE_BUILD_TYPE="Debug" $(CMAKE_BUILD_ENV) -S . -B $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR) -j$(NPROC) -s

.PHONY: build
build: release

ifneq ($(DEMO),)
LOAD	= cat $(TEST_DIR)/$(SMODE)/$(DEMO)*.sy > $(SINGLE_TEST).sy
INPFILE	= $(shell ls $(TEST_DIR)/$(SMODE)/$(DEMO)*.in)
REDINP	= $(addprefix < ,$(INPFILE))
endif

.PHONY: run
run: debug $(SYLIB_LL)
	$(LOAD)
	$(BINARY) -S -o $(SINGLE_TEST).s -l $(SINGLE_TEST).ll $(SINGLE_TEST).sy
	$(LLLD) $(SYLIB_LL) $(SINGLE_TEST).ll -S -o $(SINGLE_TEST).run.ll
	$(LLI) $(SINGLE_TEST).run.ll $(REDINP)
	$(ECHO) $$?

.PHONY: ll
ll: $(SYLIB_LL)
	$(BINARY) -S -l $(SINGLE_TEST).ll $(SINGLE_TEST).sy
	$(LLLD) $(SYLIB_LL) $(SINGLE_TEST).ll -S -o $(SINGLE_TEST).run.ll
	$(LLI) $(SINGLE_TEST).run.ll $(REDINP)
	$(ECHO) $$?

.PHONY: rv
rv:
	$(BINARY) -S -o $(SINGLE_TEST).s -d $(SINGLE_TEST).ir.s $(SINGLE_TEST).sy
	$(RVCC_linux) -o $(SINGLE_TEST).out $(SINGLE_TEST).s $(SYLIB_C) -static -march=rv64gc -g
	$(RVOD_linux) -D $(SINGLE_TEST).out > $(SINGLE_TEST).dump
	$(SIM_CMD) $(SINGLE_TEST).out $(REDINP)
	$(ECHO) $$?

.PHONY: qemu-dbg
qemu-dbg:
	$(SIM_CMD) -singlestep -g 1234 $(SINGLE_TEST).out $(REDINP)

.PHONY: rvgdb
rvgdb:
	$(RVGDB_linux) -q $(SINGLE_TEST).out

.PHONY: pys
pys:
	@$(PY) $(PYTEST) -a -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) $(shell ls $(TEST_DIR)/$(SMODE)/$(DEMO)*.sy) -m "$(SIM_CMD)" -s $(SYLIB_C) -x $(RVCC_linux)

.PHONY: diff
diff:
	code -d $(shell ls $(BUILD_DIR)/$(CPLER_TEST_DIR)/pys/$(DEMO)*.res) $(shell ls $(TEST_DIR)/functional/$(DEMO)*.out)

.PHONY: perf
.ONESHELL:
perf:
	perf record -e cpu-clock -g $(BINARY) -S -o $(SINGLE_TEST).s $(SINGLE_TEST).sy


.PHONY: clean
clean:
	-@rm -rf $(BUILD_DIR)

.PHONY: clean-test
clean-test:
	-@rm -rf $(OUTPUT_ASM) $(OUTPUT_LOG) $(OUTPUT_RES) $(OUTPUT_IR)

.PHONY: clean-s
clean-s:
	-@rm -rf $(SINGLE_TEST).{ll,run.ll} $(SINGLE_TEST).{s,S} $(SINGLE_TEST).{out,out.log,dump,ir.s}

.PHONY: clean-py
clean-py:
	-@rm -rf $(BUILD_DIR)/$(CPLER_TEST_DIR)

.PHONY: clean-all
clean-all: clean clean-test clean-s

# make formatter fake targets
FORMAT			:= format
FORMAT_TARGETS	:= $(addprefix $(FORMAT)/,$(PROJECT_SRC))

$(FORMAT_TARGETS): $(FORMAT)/%:%
	$(FORMATTER) $^ -i
 
.PHONY: format-all
format-all: $(FORMAT_TARGETS)

# old shell test
.PHONY: all
.ONESHELL:
all: build
	@success=0
	for file in $(sort $(TEST_CASES))
	do
		LOG=$${file%.*}.log
		RES=$${file%.*}.res
		LL=$${file%.*}.ll
		IN=$${file%.*}.in
		OUT=$${file%.*}.out
		FILE=$${file##*/}
		FILE=$${FILE%.*}
		timeout 180s ./$(BINARY) -S -l $${LL} $${file}  >> $${LOG}
		RETURN_VALUE=$$? 
		if [ $$RETURN_VALUE = 124 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Timeout\033[0m"
			continue
		else if [ $$RETURN_VALUE != 0 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Error\033[0m"
			continue
			fi
		fi
		$(LLLD) sylib.ll $${LL} -S -o $${LL} >> $${LOG} 2>&1
		if [ $$? != 0 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mAssemble Error\033[0m"
		else
			if [ -f "$${IN}" ]; then
				timeout 300s $(LLI) $${LL} <$${IN} 2>>$${LOG} >$${RES}
			else
				timeout 300s $(LLI) $${LL} 2>>$${LOG} >$${RES}
			fi
			RETURN_VALUE=$$?
			FINAL=`tail -c 1 $${RES}`
			[ $${FINAL} ] && $(ECHO) "\n$${RETURN_VALUE}" >> $${RES} || $(ECHO) "$${RETURN_VALUE}" >> $${RES}

			$(DIFF) -Z $${RES} $${OUT} >/dev/null 2>&1
			if [ $$? != 0 ]; then
				$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mWrong Answer\033[0m"
			else
				success=$$((success + 1))
				$(ECHO) "\033[1;32mPASS:\033[0m $${FILE}"
			fi
		fi
	done