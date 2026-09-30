# Embedded-C-Cpp-Learning

> Clean Public Snapshot Candidate · R1 internal candidate · no remote configured

保留独立编写的 C/C++ practice、host tests 与软件结构文档。

## Public snapshot scope

- 保留独立源码文件：27 个；范围以当前 candidate tree 为准。
- 课程源码、课程图片、PDF、未知字体、未知生成资产和不必要的第三方/vendor 大包均不在本快照中。
- 26 份课程源码未进入该快照；功能范围仅以保留的 practice 与 tests 为准。

## Contents

- [技术文档](docs/)
- [自主架构图来源说明](assets/images/SOURCES.md)
- [独立 practice 与 host tests](projects/)

## Validation boundary

- 未提供板端运行证据时，不声称 hardware verified。
- 本轮对保留的 host-testable 核心运行了 GCC 16.1.0 编译和测试；未验证 Keil/板端构建或硬件运行。
- 文档中的协议、地址、寄存器和架构关系属于技术事实说明，不表示未知来源的具体实现已被保留。

## License scope

根目录 LICENSE 仅适用于该 candidate 中由仓库维护者独立编写的文档、图示和代码。未捆绑的 upstream 组件、课程材料和第三方实现不因文档引用而受到根 MIT License 覆盖。