# FPTreeDB键值存储系统

## 基本说明

本次课程设计基于针对NVM优化的数据结构FPTree，实现一个简单的单线程键值存储引擎FPTreeDB。我们通过将其包装成一个调用库，供用户程序使用并管理其数据存储，与LevelDB的使用方式类似。为了证明其高效性，我们还将其和LevelDB进行测试对比。

## v1(4.25 - 5.4)

- 实现了PAllocator

- 使用了LevelDB并进行简单的测试

- 对应的文件为src文件夹下面的lycsb.cpp，p_allocator.cpp

- lycsb运行截图如下：

  ![lycsb](https://github.com/kjhmh2/database418/blob/v3/pic/lycsb.JPG)

## v2(5.5 - 5.11)

需要完成FPTreeDB插入和重载操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行



## v3(5.12 - 5.18)

需要完成FPTreeDB查询和更新操作并通过相关测试，对应fptree.cpp的实现和fptree_test.cpp部分的运行

1. 实现了FPTree有关查询相关的操作
   - 对应的函数为Value InnerNode::find(Key k)
   - 实现基本步骤：
     1. 使用二分查找findIndex(k)查找目标子节点
     2. 如果找不到，则返回MAX_VALUE
     3. 如果能够找到，则对其孩子进行递归查询操作
2. 实现了FPTree有关更新相关的操作
   - 对应的函数为bool InnerNode::update(Key k, Value v)
   - 实现基本步骤：
     1. 使用二分查找findIndex(k)查找目标子节点
     2. 如果找不到，返回false
     3. 如果能够找到，则对其孩子进行递归更新操作

## v4(5.19 - 5.31)（待完成）

需要完成FPTreeDB删除操作和所有剩下实现以及测试，对应fptree.cpp的实现和fptree_test.cpp所有的运行

