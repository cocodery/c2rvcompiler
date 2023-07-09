# C2RV Compiler by 3TLE3WA

# 整体架构说明

```
          +-----------+
          | sysy file |
          +-----------+
                |
                v
  +------------------------------+
  | antlr4 pre genernated parser |
  +------------------------------+
                |
                |
                v
      +----------------------+
      | frontend gen llvm ir |
      +----------------------+
                |
                v
      +---------------------+
      | backend distributer |
      +---------------------+
          |     |     |  
          |     |     | <--- compile tasks allocated to multi-threads if parallel issued
          |     |     |      serialize issue or parallel issue
          |    ...   ...
          |
          v
  +-------------------+
  | risc-lang ir      |
  | just Uop and VReg |  <-- schedule on ir, assign register, plan stack map
  +-------------------+
            |
            |
            |
            v            +--------------+
      +-------------+    | other thread |
      |  riscv asm  |    +--------------+
      +-------------+           v
            |                   v
            |                   |                  +--------------+
            +-------------------+------------- <<< | other thread |
                                |                  +--------------+
                                |
                                | <--- do trick on generation
                                |
                                v
                +---------------------------------+
                | asm optimization and generation |
                +---------------------------------+
                                |
                                | <--- schedule on asm and reduce redundant codes
                                v
                          +----------+
                          | asm file |
                          +----------+
```

# TODO

## 中端

- [x] 解 phi
- [ ] 常量传播后的 0 - a 转换为 neg a
- [ ] 常量传播 special，在 main 函数里，如果当前块的所有 domi 里都没有含有副作用的函数调用，则所有全局变量转换为常量
- [ ] 尾递归以及可尾递归化的函数进行 inline，转换成循环
- [ ] 利用乘法分配律，将多个同系数乘法转换成加法和一个乘法，除法涉及精度问题，不需要做
- [ ] 全局标签值的重复加载和保存问题，应当将其转换为寄存器内
  - 建议，后端使用的办法是一个全局标签记录。在每次加载全局标签的时候做如下记录：
    1. 如果 offset 是常数，记录 (glb id, offset)
    2. 如果 offset 不是常数，目前可以不做考虑
  - 在这个情况下，每当要加载全局标签的时候，就可以参考记录，直接从寄存器里获取到地址
  - 以上只对重复加载有帮助，对于重复保存需要额外手段
- [ ] fabs, fneg, fmax, fmin
- [ ] 位运算优化
- [ ] 以下为上一个后端实现的功能，目前需要提到中端优化
  - [ ] 浮点数 2 的幂次乘除法转整数运算
    - 流程比较长，详细见下方说明
  - [ ] 将常数加减乘除优化提前到后端 ir 阶段并使用链接方法优化（小小的黑魔法）
  - [ ] 全局符号加载后的寄存器多次复用，使用查询表和链接方法优化
  - [ ] 移除多余的 `load` `save`
  - [ ] 过大全局变量转换为全局指针变量，main 函数内额外增加 malloc 调用（XD 目前不需要，仅在全局变量上加上 onheap 标识）
  - [ ] 除以浮点常数转换为乘以倒数（XD 精度问题放弃）
  - [ ] 移除多余的跳转
  - [ ] 合并可合并的基本块（为后期指令调度提供更大空间，同时减少跳转次数）
  - [ ] 浮点数关于 2 的幂次优化（目前仅 2，注意精度问题）

## 后端

- [ ] 窥孔优化
  - [ ] 合并 `b` `icmp`
- [x] 贪心寄存器分配算法
  - [ ] split
- [ ] 指令调度 
  - [ ] 解决内存别名问题
- [ ] 释放所有的寄存器，也即不再有特殊用处的寄存器
- [ ] 常用栈上变量按靠近 fp 和 sp 方式分布和计算


# 后端细节说明

使用了不少自定义的数据结构，可能问题多多，在考虑修改的时候要纳入考虑。

## Saving 状态

当经过一个函数调用的时候，需要被 caller 保存的寄存器会被保存到栈上。当函数返回后，这些寄存器的值并不会马上被使用，而可能会直接开始下一次函数调用。那么此时这些寄存器是不需要被恢复的，故使用一个 Saving 位指示当前寄存器已经保存在栈上可以不用做取出再加载的操作。

## Interval Manager

活跃区间，使用二进制位表示是否在某个位置活跃。目前用该方法判断是否会产生冲突。

该方法虽然想法非常好，但可能会造成非常大的存储浪费。

## 栈组织

```
+-------------------+ <--- fp
| ra                |
+-------------------+
| s0/fp             |
+-------------------+
| callee/caller     |
| save registers    |
+-------------------+
| remain spills     |
+-------------------+
| alloca for now    |
| short type first  |
| then long array   |
+-------------------+
| spills until      |
| (addr - sp) < 2048|
+-------------------+
| arguments > 8     |
+-------------------+ <--- sp
```

## 寄存器偏好

当函数内不调用其他函数的时候，优先使用 t 和 a 寄存器。

当函数内调用其他函数的时候，优先使用 s 寄存器。

# Makefile 说明

> 注意，本项目的 Makefile 可能随时变动

| target       | 目的                                                              | 参数和说明                                                             |
| ------------ | ----------------------------------------------------------------- | ---------------------------------------------------------------------- |
| `pyll`       | 使用 `runtest.py` 测试 llvm ir 正确性                             | 无                                                                     |
| `pyasm`      | 使用 `runtest.py` 测试 riscv asm 正确性                           | 无                                                                     |
| `release`    | 编译出 release 版本的编译器，一般会定义 NDEBUG                    | 无                                                                     |
| `debug`      | 编译出 debug 版本的编译器，用于调试                               | 无                                                                     |
| `build`      | 按照依赖编译编译器                                                | 无                                                                     |
| `run`        | 单测                                                              | `DEMO` 选择测例，$(DEMO)*.sy；`SMODE` 单测模式，可选 `MODE` 里的内容； |
| `ll` `rv`    | 分别以生成 llvm ir 和 riscv asm 为目的进行单测，可用 `run` 的参数 | 无                                                                     |
| `qemu-dbg`   | 开启 qemu 调试，目前在端口 1234                                   | 无                                                                     |
| `pys`        | `runtest.py` 单独测试 asm                                         | 无                                                                     |
| `diff`       | 对比 `runtest.py` 单独测试最终 res 和正确输出                     | 无                                                                     |
| `perf`       | 测试编译用时情况                                                  | 无                                                                     |
| `clean*`     | 各种清理                                                          | 无                                                                     |
| `format-all` | 用 clang-format 整理所有文件格式                                  | 无                                                                     |
| `all`        | 旧 shell 脚本测试                                                 | 无                                                                     |
| `cp`         | 模拟测评机逻辑，测试是否能够编译                                  | 无                                                                     |

# CMake VSCode Json Setting

``` json
// .vscode/settings.json
{
  "cmake.debugConfig": {
    "args": [
      "-S", "-o", "main.s", "-d", "main.ir.s", "-l", "main.ll", "../main.sy"
    ]
  }
}
```

# RV ToolChain

手动编译建议编译用编译 riscv64-linux 的 gcc。也是目前各种 Linux 发行版会使用的交叉编译器。使用该版本是因为其和 qemu 配合比较方便，可以方便测试浮点数。

如果使用 spike + pk 和 riscv64-elf 的 gcc，可能会在浮点数方面遇上难题。但不得不说，spike 对于调试前期的整数程序帮助很大。

``` bash
./configure --prefix="$RISCV" --with-arch=rv64gc --with-abi=lp64d
```

# Antlr 建议

高于 4.12 版本，使用包管理器下载。

# 浮点数 2 的幂次乘除法转整数运算

注意，该优化可能会导致性能降低，本性能提升仅在 qemu 上测试过。

C 语言程序如下，这里假设了第二个乘数或者除数是常量，且是 2 的幂次，也就是 `constant_2pow` 类型。

注意，单纯的加减是不够的，还需要考虑到符号位等细节，所以需要不少的指令，一共大约 12 条指令才能完成核心部分。同时为了不会溢出，运算也需要保证，还要添加控制流防止溢出。

但是，测例的输入都是有保证的，所以可以减少一点控制流开销。

可能会困惑为什么要转换成多个整数操作。虽然这会占据整数流水线，但是对于一直等待迭代除法完成运算来看了，让整数流水线一直保持忙碌，浮点流水线完成比较方便的运算也是很有用的。

``` c
union flt {
  struct {
    unsigned mas: 23;
    unsigned exp: 8;
    unsigned sign: 1;
  };
  unsigned i;
  float f;
} cvt ;

float __crvc_flt_sra(float a, constant_2pow b) {
  union flt ai, bi, ci;
  ai.f = a;
  if (ai.i == 0) {
    return 0;
  }
  bi.f = b;
  ci.mas = ai.mas;
  int m = bi.exp - 127;
  int texp = ai.exp - m;
  ci.exp = texp;
  ci.sign = ai.sign ^ bi.sign;
  return ci.f;
}

float __crvc_flt_sll(float a, constant_2pow b) {
  union flt ai, bi, ci;
  ai.f = a;
  if (ai.i == 0) {
    return 0;
  }
  bi.f = b;
  ci.mas = ai.mas;
  int m = bi.exp - 127;
  int texp = ai.exp + m;
  ci.exp = texp;
  ci.sign = ai.sign ^ bi.sign;
  return ci.f;
}
```

# gdb cfi info insert

``` 
# define frame
  .text
.Ltext0:
  .cfi_sections	.debug_frame

# start proc
  .cfi_startproc

# after pc decrease imm
  .cfi_def_cfa_offset imm

# after ra restore
  .cfi_restore 1

# after pc restore
  .cfi_def_cfa_offset 0

# end proc
  .cfi_endproc
```