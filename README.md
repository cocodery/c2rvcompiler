# C2RV Compiler by 3TLE3WA

# 整体架构说明

```
+---------+    +------------------------------+
| sy file | -> | antlr4 pre genernated parser | ---+
+---------+    +------------------------------+    |
                                                   |
                      +----------------------+     |
                +---- | frontend gen llvm ir | <---+
                |     +----------------------+
                v
    +---------------------+
    | backend distributer |
    +---------------------+
        |     |     |  
        |     |     | <----- compile tasks allocated to multi-threads
        |     |     |
        |    ...   ...
        |
        v
  +-------------------+
  | risc-lang ir      |  <!-- thats why some variable name use prefix rl_ -->
  | just uop and vreg |
  +-------------------+
            |
            |
            |
            v            +--------------+
      +-------------+    | other thread |
      |  riscv asm  |    +--------------+
      +-------------+           v
            |                   v
            |                   |                       +--------------+
            +-------------------+------------------ <<< | other thread |
                                |                       +--------------+
                                |
                                v
                +---------------------------------+
                | release other resources         |
                | remain asm env obj              |
                | for optimization and generation |
                +---------------------------------+
                                |
                                | <--- do some magic (i dont know what
                                |      will i do before generation yet)
                                v
                          +----------+
                          | asm file |
                          +----------+
```

# TODO

## 前端

- [x] 解 phi
- [ ] 常量传播 special，在 main 函数里，如果当前块的所有 domi 里都没有含有副作用的函数调用，则所有全局变量转换为常量
- [ ] 除以浮点常数转换为乘以倒数（注意精度问题）

## 后端

- [x] 移除多余的 `load` `save`
- [x] 过大全局变量转换为全局指针变量，main 函数内额外增加 malloc 调用（XD 目前不需要，仅在全局变量上加上 onheap 标识）
- [x] 寄存器线性扫描分配策略
    - [x] 不会被使用的 arg 寄存器征用
    - [x] 基于引用次数的贪心分配
- [x] 合并 `fmul` `fadd` `fsub`（XD 精度问题放弃）
- [x] 合并 `b` `icmp`
- [x] 移除多余的跳转
- [x] 合并可合并的基本块（为后期指令调度提供更大空间，同时减少跳转次数）

## 架构相关优化

以下内容相当理想化，以至于需要很多尝试：

- [ ] 基于目前的虚拟资源分配机制，参考缓存着色方法，控制栈上内容的存储位置，需要考虑目标机的缓存机制
- [ ] 基于 Sifive-U74 的顺序双发射机制安排指令顺序

## 进一步优化说明

### 寄存器分配机制

目前的寄存器分配机制是先使用虚拟资源分配，然后对虚拟资源进行评估，最后定夺如何分配寄存器。

该方法借鉴于操作系统的虚拟资源分配和 TOSCA。

不过依然有如下可优化部分：

1. 空闲资源回收机制优化，目前是按照随机顺序分配的（实际是 unordered_map 第一个元素）
2. 评估方法，目前是按照该虚拟资源对应的虚拟寄存器的总引用次数排序，引用次数越多优先获取真实寄存器资源

建议：
 
1. 利好评估方法对空闲资源排序，使用方法让评分最高的虚拟资源能够获得尽可能高的分数，这样获取真实寄存器资源的时候意味着可以在更多地方使用寄存器运算。
2. 前端给予足够多暗示，促进后端对寄存器的评分

> 注意，为了简化部分运算，实际上将 t0，t1，t2，fs0，fs1，fs2 这六个寄存器当作特殊用途寄存器，将它们纳入分配考虑的时候，需要将所有有关它们被特殊用途的地方均修改。

### Live Out 运算办法

使用的是 EaC 上给出的迭代计算方法。性能消耗相当庞大，容易造成编译超时，可以考虑优化。

### Live In 计算

消耗不算大，但是也是可以优化的部分。目前的 Live In 计算逻辑如下：

1. 查询表，获得所有跳转到本基本块的跳转语句
2. 通过跳转语句获取到跳转语句所属的基本块 father（注意，有一个优化，合并 `b` 和 `icmp` 会产生跳转语句，这里需要正确生成 father）
3. 所有相应基本块 father 的 Live Out 的并集就是当前块的 Live In。

# Makefile 说明

> 注意，本项目的 Makefile 可能随时变动

| target | 目的 | 参数和说明 |
| --- | --- | --- |
| `pyll` | 使用 `runtest.py` 测试 llvm ir 正确性 | 无 |
| `pyasm` | 使用 `runtest.py` 测试 riscv asm 正确性 | 无 |
| `release` | 编译出 release 版本的编译器，一般会定义 NDEBUG | 无 |
| `debug` | 编译出 debug 版本的编译器，用于调试 | 无 |
| `build` | 按照依赖编译编译器 | 无 |
| `run` | 单测 | `DEMO` 选择测例，$(DEMO)*.sy；`SMODE` 单测模式，可选 `MODE` 里的内容； |
| `ll` `rv` | 分别以生成 llvm ir 和 riscv asm 为目的进行单测，可用 `run` 的参数 | 无 |
| `qemu-dbg` | 开启 qemu 调试，目前在端口 1234 | 无 |
| `pys` | `runtest.py` 单独测试 asm | 无 |
| `diff` | 对比 `runtest.py` 单独测试最终 res 和正确输出 | 无 |
| `perf` | 测试编译用时情况 | 无 |
| `clean*` | 各种清理 | 无 |
| `format-all` | 用 clang-format 整理所有文件格式 | 无 |
| `all` | 旧 shell 脚本测试 | 无 |

# CMake VSCode Json Setting

``` json
// .vscode/settings.json
{
  "cmake.debugConfig": {
    "args": [
      "-S", "-o", "main.s", "-l", "main.ll", "../main.sy"
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