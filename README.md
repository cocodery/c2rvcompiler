# C2RV Compiler by 3TLE3WA


# TODO

- [x] 解 phi

## 后端

- [ ] 移除多余的 `load` `save`
- [ ] 寄存器分配策略
    - [ ] 不会被使用的 arg 寄存器征用
    - [ ] 基于引用次数和函数参数需求的贪心分配
- [ ] 合并 `fmul` `fadd` `fsub` (x 精度问题放弃)
- [x] 合并 `b` `icmp`
- [x] 移除多余的跳转

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

# RV ToolChain Compile Arg

``` bash
./configure --prefix="$RISCV" --with-arch=rv64gc --with-abi=lp64d --with-multilib-generator="rv64gc-lp64--"
```