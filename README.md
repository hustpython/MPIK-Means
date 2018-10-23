# 并行计算的K-Means聚类算法实现

## 一，实验介绍
聚类是拥有相同属性的对象或记录的集合，属于无监督学习，K-Means聚类算法是其中较为简单的聚类算法之一，具有易理解，运算深度块的特点。

### 1.1 实验内容
通过本次课程我们将使用C++语言实现一个完整的面向对象的可并行K-Means算法。这里我们一起围绕着算法需求实现各种类，最终打造出一个健壮的程序。所以为了更好地完成这个实验，需要你有C++语言基础，会安装一些常用库，喜欢或愿意学习面向对象的编程思维。
### 1.2 实验知识点
- C++语言语法
- K-Means算法思路与实现
- 并行计算思路与实现
- boost库的常用技巧(Smart Pointers,Variant，tokenizer)

### 1.3 实验环境
- Xfce 终端（Xfce Terminal）：
Linux 命令行终端，打开后会进入 Bash 环境，可以用来执行 Linux 命令和调用系统调用。
- GVim：非常好用的编辑器，不会使用的可以参考课程 《Vim编辑器》。
- boost,MPICH2库
### 1.4 适合人群
本课程适合有C++语言基础，对聚类算法感兴趣并希望在动手能力上得到提升的同学。
### 1.5 代码获取

### 1.6 效果图

输出结果文件

<div align=center>

![](doc/res1.png)

</div>


### 1.7，项目结构与框架
项目的整个文件目录:
```
├── clusters
│   ├── distance.hpp
│   └── record.hpp
├── datasets
│   ├── attrinfo.hpp
│   ├── dataset.hpp
│   └── dcattrinfo.hpp
├── mainalgorithm
│   ├── kmean.hpp
│   └── kmeanmain.cpp
└── utilities
    ├── datasetreader.hpp
    ├── exceptions.hpp
    ├── null.hpp
    └── types.hpp
```

这里简单介绍一下功能模块,在具体实践每一个类的时候会有详细UML图或流程图。

主要分为4个模块：数据集类,聚集类,实用工具类,算法类。

- 实用工具类:定义各种需要的数据类型;常用的异常处理；文件读取。

- 数据集类:将文件中的数据通过智能指针建立一个统一数据类，拥有丰富的属性和操作。

- 聚集类:在数据类基础上实现中心簇。

- 算法类:完成对聚集类的初始化，通过算法进行更新迭代，最终实现数据集的聚类并输出聚类结果。



## 二,实验原理
这一章我们将配置好我们的实验环境并介绍一些基础知识。
### 2.1 依赖库安装
安装boost和mpich2
```shell
mpich2下载:
wget -c http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz

解压:
tar xvfz mpich-3.2.1.tar.gz

配置:
cd mpich-3.2.1
./configure

编译:
make

安装:
make install 

boost下载:
wget -c https://dl.bintray.com/boostorg/release/1.68.0/source/boost_1_68_0.tar.gz 

解压
tar xvfz boost_1_68_0.tar.gz
cd boost_1_68_0

编译:
sh bootstrap.sh

修改project-config.jam 文件
第19行添加一句:using mpi;

安装:
./bjam --with-programoptions --with-mpi install
```
自此支持mpi的boost安装完毕！
### 2.2 boost的小技巧
#### Smart Pointers
>在Boost中，智能指针是存储指向动态分配对象的指针的对象。智能指针非常有用，因为它们确保正确销毁动态分配的对象，即使在异常情况下也是如此。事实上，智能指针被视为拥有指向的对象，因此负责在不再需要时删除对象。Boost智能指针库提供了六个智能指针类模板。表给出了这些类模板的描述。本实验中将大量使用智能指针。
<div align=center>

| 类   |      描述      | 
|----------|:-------------:|
| scoped_ptr |  单个对象的简单唯一所有权,不可复制. |
| scoped_array |    数组的简单唯一所有权。不可复制   |  
| shared_ptr | 对象所有权在多个指针之间共享 |  
|shared_array|多个指针共享的数组所有权| 
|weak_ptr|shared_ptr拥有的对象的非拥有观察者| 
|intrusive_ptr|具有嵌入引用计数的对象的共享所有权。| 

</div>

#### Variant versus Any
>Boost Variant类模板是一个安全通用的联合容器，和std::vector不同储存单个类型的多个值，variant可以储存多个类型的单个值，本实验中将使用variant储存双精度和整数类型来表示不同类型的数据。

与variant一样，Boost any是另一个异构容器。虽然Boost anys有许多与Boost variant相同的功能。
根据Boost库文档，Boost variant比Boost any具有以下优势：

1，variant保证其内容的类型是用户指定的有限类型集之一。

2，variant提供对其内容的编译时检查访问。

3，variant通过提供有效的，基于堆栈的存储方案，可以避免动态分配的开销。

同样Boost any也有一些优势:

1,any几乎允许任何类型的内容。

2,很少使用模板元编程技术

3,,,,,,

#### Tokenizer
>Tokenizer提供了一种灵活而简单的方法通过分割符（如:" , ")将一个完整的string分隔开。

 字符串为：”A flexible,easy tokenizer“

 如果通过","分割,则结果为：

 [A flexible]  [ easy tokenizer>]

以" " 为分隔符:
分割结果为：

 [A] [flexible,] [easy] [tokenizer]

### 2.3 K-Means算法思路

<center>

<img src="doc/lct1.png" width="30%" height="30%" />

Ｋ-Means算法流程图

</center>


## 三，实验步骤
### 3.1 数据集的构建
数据对于一个聚类算法来说非常重要,在这里我们将一个数据集描述为一个记录(record),一个记录由一些属性(Attribute)表征.因此自然而然将依次建立attributes,records,最后是数据集datasets.

AttrValue类有一个私有变量,有两个友元函数,一个公有成员函数.

<div align=center>

![](doc/attrvalue.png)

</div>
_value是一个variant类型变量,它可以存储一个双精度或无符号整形的数据,分类数据用无符号整形数据表示.
AttrValue类自身无法存储或获取数据.它的两个友元函数可以获取和修改数据_value.

```c++
class AttrValue 
{
    public:
       friend class DAttrInfo;
       friend class CAttrInfo;
       typedef boost::variant<Real,Size> value_type;
       AttrValue();
    private:
       value_type _value;
};

inline AttrValue::AttrValue(): _value(Null<Size>()) {
    }
```