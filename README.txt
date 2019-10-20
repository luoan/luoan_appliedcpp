源代码在src目录下

框架在64位CentOS7下正常运行
框架可以做成库，不过当时只是写着玩玩就没去做这方面的工作了

用图片做了简单的测试 set_juren.jpg 给图片画了一横一竖的粗线条

用ptr管理内存，且加锁，这里句柄ptr其实不加锁也可以，在内存管理基类加锁就行，当初其实可以直接用shared_ptr的，ptr是自己写的一个简易的智能指针
死锁是要自己多线程代码要避免

jpeglib是编译安装的jpeg-9c的静态库和头文件

src目录中的codeblocks是当时调试代码时用codeblocks调试所得

文件全部小写，对象名称与书本代码一样

gcc.txt 编译命令

thread.h
    多线程

lock.h lock.cpp
    加锁

alloctmpl.h ptr.hpp
    内存的分配

geometry.h geometry.cpp
    apPoint apRect点与矩形表示

imagestorage.h imagestorage.cpp
    apImageStorageBase 关于图像的边界
    apRectImageStorage : public apImageStorageBase 非常复杂的类，处理关于内存管理的各个方面
    apImageStorage : public apRectImageStorage 为任意类型定义数据存储

imagestoragetmpl.h
    对apRectImageStorage对象的迭代器,apImageStorage是继承自apRectImageStorage,是一个template，包装一下apRectImageStorage,T是每个像素大小

imagetypes.h
    RGB图像表示，以及钳制对象

imagetmpl.h
    apImage是一个包装器，只是对apImageStorage函数的包装

image.h
    包含imagetmpl.h以及将来加入的图像处理函数的头文件

io.h io.cpp
    文件委托的基类，可以派生出jpeg tiff文件委托。此外还包括所有文件委托的列表

jpeg.h jpeg.cpp
    jpeg文件委托，派生自io.h，此外还包括一个jpeg文件委托的安装器，向列表写入jpeg委托信息

