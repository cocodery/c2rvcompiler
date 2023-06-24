TOPNAME 		:= compiler 
BUILD_DIR 		:= build
BINARY 			:= $(BUILD_DIR)/$(TOPNAME)
CMAKE 			:= cmake
MAKE 			:= make
GDB 			:= gdb
LLDB 			:= lldb
LLVM_LINK 		:= llvm-link
LLI 			:= lli
DIFF 			:= diff
ECHO			:= echo
FORMATTER		:= clang-format
CLANG			:= clang

TMP				:= /tmp

# 检查 rv 工具链情况
ifneq ($(RISCV),)
RVAS			:= $(RISCV)/bin/riscv64-unknown-elf-as
RVCC			:= $(RISCV)/bin/riscv64-unknown-elf-gcc
RVOD			:= $(RISCV)/bin/riscv64-unknown-elf-objdump
SPIKE			:= $(RISCV)/bin/spike
PK				:= $(RISCV)/riscv64-unknown-elf/bin/pk
endif

PY				:= python
PYTEST			:= runtest.py

$(shell mkdir -p $(BUILD_DIR))

OS 				:= $(shell uname)
NPROC			:= $(shell nproc)
ALL_SRC			:= $(shell find src -name '*.cpp' -or -name '*.hh')

# 默认 debug 模式，比较严格的检测和 DEBUG_MODE 宏
CMAKE_BUILD_VAR	:= CMAKE_BUILD_TYPE="Debug"

ifeq ($(MOD),ASAN)
# 打开 address sanitizer
CMAKE_BUILD_VAR	+= ASAN=1
else ifeq ($(MOD),RLS)
# 打开 O2 NDEBUG (关掉 assert)
CMAKE_BUILD_VAR	:= CMAKE_BUILD_TYPE="Release"
endif

CMAKE_BUILD_ENV := $(addprefix -D,$(CMAKE_BUILD_VAR))

MODE 			:= functional # hidden_functional final_performance performance

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

SINGLE_TEST_NAME:= main

PARGS			:= -s $(SYLIB_C)

# make python test all fake targets
PYALL			:= pyall
PYALL_TARGETS	:= $(addprefix $(PYALL)/,$(MODE))

# make python test llvmir fake targets
PYLL			:= pyll
PYLL_TARGETS	:= $(addprefix $(PYLL)/,$(MODE))

# make python test asm fake targets
PYASM			:= pyasm
PYASM_TARGETS	:= $(addprefix $(PYASM)/,$(MODE))

# make formatter fake targets
FORMAT			:= format
FORMAT_TARGETS	:= $(addprefix $(FORMAT)/,$(ALL_SRC))

$(SYLIB_LL): $(SYLIB_C) $(SYLIB_H)
	@$(CLANG) -emit-llvm -S $(SYLIB_C) -I $(SYLIB_H) -o $@

$(PYALL_TARGETS): $(PYALL)/%:$(TEST_DIR)/%
	@$(PY) $(PYTEST) -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) -A $(PARGS) $(shell find $< -name "*.sy")

$(PYLL_TARGETS): $(PYLL)/%:$(TEST_DIR)/%
	@$(PY) $(PYTEST) -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) -l $(PARGS) $(shell find $< -name "*.sy")

$(PYASM_TARGETS): $(PYASM)/%:$(TEST_DIR)/%
	@$(PY) $(PYTEST) -c $(BINARY) -d $(BUILD_DIR)/$(CPLER_TEST_DIR)/$(notdir $@) -a $(PARGS) $(shell find $< -name "*.sy")

.PHONY: pyall
pyall: $(SYLIB_LL) $(BINARY) $(PYALL_TARGETS)

.PHONY: pyll
pyll:  $(SYLIB_LL) $(BINARY) $(PYLL_TARGETS)

.PHONY: pyasm
pyasm: $(SYLIB_LL) $(BINARY) $(PYASM_TARGETS)

$(BINARY): $(ALL_SRC)
	$(CMAKE) $(CMAKE_BUILD_ENV) -S . -B $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR) -j$(NPROC) -s

.PHONY: build
build: $(BINARY)

ifneq ($(DEMO),)
PRE		= cat $(TEST_DIR)/functional/$(DEMO)*.sy > $(SINGLE_TEST_NAME).sy
INP		= $(shell ls $(TEST_DIR)/functional/$(DEMO)*.in)
REDINP	= $(addprefix < ,$(INP)) 
CATINP  = $(addprefix cat ,$(INP)) 
endif

.PHONY: run
run: build $(SYLIB_LL)
	$(PRE)
	$(CATINP)
	$(BINARY) -S -o $(SINGLE_TEST_NAME).s -l $(SINGLE_TEST_NAME).ll $(SINGLE_TEST_NAME).sy
	$(LLVM_LINK) $(SYLIB_LL) $(SINGLE_TEST_NAME).ll -S -o $(SINGLE_TEST_NAME).run.ll
	$(LLI) $(SINGLE_TEST_NAME).run.ll $(REDINP)
	$(ECHO) $$?
	$(RVCC) -o $(SINGLE_TEST_NAME).out $(SINGLE_TEST_NAME).s $(SYLIB_C) -static
	$(RVOD) -D $(SINGLE_TEST_NAME).out > $(SINGLE_TEST_NAME).dump
	$(SPIKE) $(PK) $(SINGLE_TEST_NAME).out $(REDINP)
	$(ECHO) $$?
	

rvrun:
	$(BINARY) -S -o $(SINGLE_TEST_NAME).s $(SINGLE_TEST_NAME).sy
	$(RVCC) -o $(SINGLE_TEST_NAME).out $(SINGLE_TEST_NAME).s $(SYLIB_C) -static
	$(RVOD) -D $(SINGLE_TEST_NAME).out > $(SINGLE_TEST_NAME).dump
	$(SPIKE) -l --log=$(SINGLE_TEST_NAME).out.log -d $(PK) $(SINGLE_TEST_NAME).out

.PHONY: all asm

.ONESHELL:
all: build
	@success=0
	@for file in $(sort $(TEST_CASES))
	do
		ASM=$${file%.*}.s
		LOG=$${file%.*}.log
		RES=$${file%.*}.res
		LL=$${file%.*}.ll
		IN=$${file%.*}.in
		OUT=$${file%.*}.out
		FILE=$${file##*/}
		FILE=$${FILE%.*}
		timeout 180s ./$(BINARY) -S -o $${ASM} -l $${LL} $${file}  >> $${LOG}
		RETURN_VALUE=$$? 
		if [ $$RETURN_VALUE = 124 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Timeout\033[0m"
			continue
		else if [ $$RETURN_VALUE != 0 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Error\033[0m"
			continue
			fi
		fi
		$(LLVM_LINK) sylib.ll $${LL} -S -o $${LL} >> $${LOG} 2>&1
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

.ONESHELL:
asm: build
	@success=0
	@for file in $(sort $(TEST_CASES))
	do
		ASM=$${file%.*}.s
		LOG=$${file%.*}.log
		RES=$${file%.*}.res
		IN=$${file%.*}.in
		OUT=$${file%.*}.out
		FILE=$${file##*/}
		FILE=$${FILE%.*}
		timeout 500s ./$(BINARY) -S -o $${ASM} $${file} >> $${LOG}
		RETURN_VALUE=$$? 
		if [ $$RETURN_VALUE = 124 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Timeout\033[0m"
			continue
		else if [ $$RETURN_VALUE != 0 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mCompile Error\033[0m"
			continue
			fi
		fi
		gcc -march=armv7-a $${ASM} sylib.o -o exec -static >> $${LOG} 2>&1
		if [ $$? != 0 ]; then
			$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mLink Error\033[0m"
		else
			if [ -f "$${IN}" ]; then
				timeout 180s ./exec <$${IN} 2>>$${LOG} >$${RES}
			else
				timeout 180s ./exec 2>>$${LOG} >$${RES}
			fi
			RETURN_VALUE=$$?
			FINAL=`tail -c 1 $${RES}`
			[ $${FINAL} ] && $(ECHO) "\n$${RETURN_VALUE}" >> $${RES} || $(ECHO) "$${RETURN_VALUE}" >> $${RES}

			diff -Z $${RES} $${OUT} >/dev/null 2>&1
			if [ $$? != 0 ]; then
				$(ECHO) "\033[1;31mFAIL:\033[0m $${FILE}\t\033[1;31mWrong Answer\033[0m"
			else
				success=$$((success + 1))
				$(ECHO) "\033[1;32mPASS:\033[0m $${FILE}"
			fi
		fi
	done

.PHONY: gdb
gdb: build
	$(GDB) -q --args $(BINARY) -S -o $(SINGLE_TEST_NAME).s -l $(SINGLE_TEST_NAME).ll $(SINGLE_TEST_NAME).sy

.PHONY: lldb
lldb: build
	$(LLDB) $(BINARY)

.PHONY: clean
clean:
	-@rm -rf $(BUILD_DIR)

.PHONY: clean-test
clean-test:
	-@rm -rf $(OUTPUT_ASM) $(OUTPUT_LOG) $(OUTPUT_RES) $(OUTPUT_IR) 
	-@rm -rf $(SINGLE_TEST_NAME).ll $(SINGLE_TEST_NAME).run.ll $(SINGLE_TEST_NAME).{s,S} $(SINGLE_TEST_NAME).{out,out.log,dump} *.ir.s

.PHONY: clean-all
clean-all: clean clean-test

$(FORMAT_TARGETS): $(FORMAT)/%:%
	$(FORMATTER) $^ -i
 
.PHONY: format-all
format-all: $(FORMAT_TARGETS)

