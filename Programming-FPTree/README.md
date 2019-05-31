# FPTreeDB键值存储系统

## 基本说明

​	本次课程设计基于针对NVM优化的数据结构FPTree，实现一个简单的单线程键值存储引擎FPTreeDB。我们通过将其包装成一个调用库，供用户程序使用并管理其数据存储，与LevelDB的使用方式类似。为了证明其高效性，我们还将其和LevelDB进行测试对比。

## 工具

- LevelDB

  ​	LevelDB是Google开源的持久化KV单机数据库，具有很高的随机写、顺序读/写性能，但随机读的性能比较一般。LevelDB适合应用在查询较少，写操作较多的场景。LevelDB应用LSM(Log Structured Merge) 策略，lsm_tree对索引变更进行延迟及批量处理，并通过一种类似于归并排序的方式高效地将更新迁移到磁盘，降低索引插入开销。

  ​	本次课程设计，需要安装LevelDB，其作用主要是用以和我们实现的FPTree进行对比测试性能。

- PMDK

  ​	 PMDK全称Persistent Memory Development Kit ，是一套用于在非易失性内存（Non-volatile Memory，即是NVM）上进程编程的用户态软件库。

  ​	本次课程设计使用的是PMDK的libpmem库，FPTree中所有涉及NVM的操作利用其进行。编程要用到其中的函数包括：

  ​	pmem_map_file：打开并映射文件

  ​	pmem_persist：持久化对NVM内容的修改

- YCSB测试

  ​	YCSB，全称是 "Yahoo！Cloud Serving Benchmark”，是雅虎开发的用来对云服务进行基础测试的工具，是一个键值数据库性能测试的benchmark，其内部涵盖了常见的NoSQL数据库产品，如Cassandra、MongoDB等等。在运行YCSB的时候，可以配置不同的workload和DB，也可以指定线程数和并发数等其他参数。

  ​	本次课程设计利用YCSB进行测试主要分为两步，第一步是读取load文件，插入一定量的数据对数据库进行初始化。第二步是读取run文件，进行数据库相关操作。

  ​	一个load文件的样例为：

  ```
  INSERT 6284781860667377211
  INSERT 8517097267634966620
  INSERT 1820151046732198393
  ...
  ```

  ​	一个run文件的样例为：

  ```
  UPDATE 1495948725458769726
  UPDATE 3751601629208066540
  READ 4118333051244900687
  ...
  ```

  ​	前面的英文表示进行的操作，比如插入---INSERT，查询---READ等等，后面则是键值，我们取前8个字节。并且为了方便，我们让键和值取相同的值。

- Google Test测试

  ​	单元测试的源文件在test文件夹下，每个测试的名称对应其要测试的目标函数和功能，用以验证FPTree的代码正确性。

## 文件目录

```
|__gtest: 为Google Test项目目录
|__include: 头文件  
   |__fptree: fptree的头文件
      |__fptree.h: fptree的头文件  
   |__utility: fptree所用工具的头文件 
      |__utility.h: 指纹计算等工具函数头文件
      |__clhash.h: 指纹计算所用哈希函数头文件
      |__p_allocator.h: NVM内存分配器头文件
|__src: 源码  
   |__bin: 可执行文件所在文件夹
      |__main: main.cpp的可执行文件
      |__lycsb: lycsb.cpp的可执行文件
      |__ycsb: ycsb.cpp的可执行文件
   |__build: 存放编译生成的.o文件
   |__fptree.cpp: fptree的源文件
   |__clhash.c: 指纹计算的哈希函数源文件  
   |__p_allocator.cpp: NVM内存分配器源文件
   |__lycsb.cpp: LevelDB的YCSB测试代码
   |__ycsb.cpp: FPTreeDB和LevelDB的YCSB对比测试代码
   |__makefile: 编译文件  
|__workloads: 为YCSB测试负载文件，用于YCSB Benchmark测试  
   |__数据量-rw-读比例-写比例-load.txt: YCSB测试数据库装载文件  
   |__数据量-rw-读比例-写比例-run.txt: YCSB测试运行文件  
|__test: 为Google Test用户测试代码文件夹
   |__bin: 单元测试可执行文件所在文件夹
      |__fptree_test: fptree_test.cpp的可执行文件
      |__utility_test: utility_test.cpp的可执行文件
   |__fptree_test.cpp: fptree相关测试  
   |__utility_test.cpp: PAllocator等相关测试  
   |__makefile: gtest单元测试的编译文件   
```

## 系统架构说明

​	整个系统的结构如下所示：

​	![GoogleTest1](C:\Users\xc\Desktop\img\system.png)

​	本次课程设计基于针对NVM优化的数据结构FPTree，实现一个简单的键值存储引擎FPTreeDB。将其包装成一个调用库，供用户程序使用并管理其数据存储，与LevelDB的使用方式类似。

## 编译指令说明

src目录下的makefile测试：

- 编译生成main文件并执行：

  ```
  # make
  # cd bin
  # ./main
  ```

- 编译生成lycsb文件并执行：

  ```
  # make test1
  # cd bin
  # ./lycsb
  ```

- 编译生成ycsb文件并执行：

  ```
  # make test2
  # cd bin
  # ./ycsb
  ```

## 结果

Google Test测试结果：

- utility_test

  ![GoogleTest1](https://github.com/kjhmh2/database418/blob/final/img/GoogleTest1.JPG)

- fptree_test

  ![GoogleTest2](https://github.com/kjhmh2/database418/blob/final/img/GoogleTest2.JPG)

  ![GoogleTest3](https://github.com/kjhmh2/database418/blob/final/img/GoogleTest3.JPG)

src文件夹运行结果：

- main

  ![GoogleTest1](C:\Users\xc\Desktop\img\main.JPG)

- lycsb

  ![GoogleTest1](C:\Users\xc\Desktop\img\lycsb.JPG)

- ycsb

  ![GoogleTest1](C:\Users\xc\Desktop\img\ycsb.JPG)

