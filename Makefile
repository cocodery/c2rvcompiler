TOPNAME 			:= compiler 
BUILD_DIR 		:= ./build
BINARY 				:= $(BUILD_DIR)/$(TOPNAME)
CMAKE 				:= cmake
MAKE 					:= make
GDB 					:= gdb
LLDB 					:= lldb
LLVM_LINK 		:= llvm-link
LLI 					:= lli
DIFF 					:= diff
ECHO 					:= echo -e

OS 						:= $(shell uname)
NPROC					:= $(shell nproc)

MODE 					:= functional # test case directory

TEST_DIR 		:= ./compiler2022/公开样例与运行时库/$(MODE)
TEST_CASES	:= $(shell find $(TEST_DIR) -name "*.sy")

SINGLE_TEST_NAME:= main

OUTPUT_ASM 	:= $(addsuffix .s,$(basename $(TEST_CASES)))
OUTPUT_RES 	:= $(addsuffix .res,$(basename $(TEST_CASES)))
OUTPUT_LOG 	:= $(addsuffix .log,$(basename $(TEST_CASES)))
OUTPUT_IR  	:= $(addsuffix .ll,$(basename $(TEST_CASES)))

CMAKE_BUILD_VAR	:= CMAKE_BUILD_TYPE="Debug"

ifeq ($(MODE), RESTRICT)
CMAKE_BUILD_VAR	+= RESTRICT=1
else ifeq ($(MODE), RELEASE)
CMAKE_BUILD_VAR	:= CMAKE_BUILD_TYPE="Release"
endif

CMAKE_BUILD_ENV 	:= $(addprefix -D,$(CMAKE_BUILD_VAR))

$(shell mkdir -p $(BUILD_DIR))

.PHONY: build run all asm gdb lldb clean clean-test

$(BUILD_DIR)/$(TOPNAME):
	$(CMAKE) $(CMAKE_BUILD_ENV) -S . -B $(BUILD_DIR)
	$(MAKE) -C $(BUILD_DIR) -j$(NPROC) -s

build: $(BUILD_DIR)/$(TOPNAME)

$(SINGLE_TEST_NAME).ll:
	$(BUILD_DIR)/$(TOPNAME) -S -o $(SINGLE_TEST_NAME).S -l $(SINGLE_TEST_NAME).ll $(SINGLE_TEST_NAME).sy

run: $(BUILD_DIR)/$(TOPNAME) $(SINGLE_TEST_NAME).ll
	$(LLVM_LINK) sylib.ll $(SINGLE_TEST_NAME).ll -S -o $(SINGLE_TEST_NAME)

.ONESHELL:
all:
	@success=0
	@for file in $(sort $(TEST_CASES))
	do
		ASM	=$${file%.*}.s
		LOG	=$${file%.*}.log
		RES	=$${file%.*}.res
		LL	=$${file%.*}.ll
		IN	=$${file%.*}.in
		OUT	=$${file%.*}.out
		FILE=$${file##*/}
		FILE=$${FILE%.*}
		timeout 180s ./$(BUILD_DIR)/$(TOPNAME) -S -o $${ASM} -l $${LL} $${file}  >> $${LOG}
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
asm:
	@success=0
	@for file in $(sort $(TEST_CASES))
	do
		ASM	=$${file%.*}.s
		LOG	=$${file%.*}.log
		RES	=$${file%.*}.res
		IN	=$${file%.*}.in
		OUT	=$${file%.*}.out
		FILE=$${file##*/}
		FILE=$${FILE%.*}
		timeout 500s ./$(BUILD_DIR)/$(TOPNAME) -S -o $${ASM} $${file} >> $${LOG}
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

gdb: $(BUILD_DIR)/$(TOPNAME)
	$(GDB) --batch --args $(BUILD_DIR)/$(TOPNAME) -S -o $(SINGLE_TEST_NAME).S -l $(SINGLE_TEST_NAME).ll $(SINGLE_TEST_NAME).sy

lldb: $(BUILD_DIR)/$(TOPNAME)
	$(LLDB) $(BUILD_DIR)/$(TOPNAME)

clean:
	-@rm -rf $(BUILD_DIR)

clean-test:
	-@rm -rf $(OUTPUT_ASM) $(OUTPUT_LOG) $(OUTPUT_RES) $(OUTPUT_IR)  
