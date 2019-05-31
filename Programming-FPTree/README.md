# FPTreeDB键值存储系统

## 基本说明

本次课程设计基于针对NVM优化的数据结构FPTree，实现一个简单的单线程键值存储引擎FPTreeDB。我们通过将其包装成一个调用库，供用户程序使用并管理其数据存储，与LevelDB的使用方式类似。为了证明其高效性，我们还将其和LevelDB进行测试对比。

## v1(4.25 - 5.4)

- 实现了PAllocator
- 使用了LevelDB并进行简单的测试
- 对应的文件为src文件夹下面的lycsb.cpp，p_allocator.cpp

## v2(5.5 - 5.11)（待完成）

- 重新修改了lycsb文件

- 运行截图如下：

  

- 

需要完成FPTreeDB插入和重载操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行

## v3(5.12 - 5.18)（待完成）

需要完成FPTreeDB查询和更新操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行

## v4(5.19 - 5.31)（待完成）

需要完成FPTreeDB删除操作和所有剩下实现以及测试，对应fptree.cpp的实现和fptree_test.cpp所有的运行

