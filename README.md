# C2RV Compiler by 3TLE3WA


# TODO

- [ ] 解 phi

## 后端

- [ ] 合并 `mul` `add` `sub`
- [ ] 合并 `b` `icmp`
- [ ] 移除多余的 `load` `save`
- [ ] 移除多余的跳转
- [ ] 寄存器分配策略
    - [ ] 不会被使用的 arg 寄存器征用
    - [ ] 基于引用次数和函数参数需求的贪心分配

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