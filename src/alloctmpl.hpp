#ifndef _alloctmpl_h_
#define _alloctmpl_h_


#include "ptr.hpp"
#include "lock.h"

#include <stdint.h>
#include <algorithm>
#include <iostream>

template <class T>
class apAllocator_
{
  template <class F> friend F* clone(const F* );
public :
  explicit apAllocator_(unsigned int size,
      unsigned int align = 0) : pRaw_(0),
    pData_(0),size_(size), align_(align) { allocate(); }

  ~apAllocator_() { deallocate(); }

  T* pData          () { return pData_; }
  const T* pData    () const { return pData_; }
  unsigned int size () const { return size_; }
  unsigned int align() const { return align_; }

  //bool lock  () const { return lock_.lock(); }
  //bool unlock() const { return lock_.unlock(); }

protected :
  apAllocator_* clone() const;

private :
  void allocate  ();
  void deallocate();
  T* alignPointer(void*);

  apAllocator_(const apAllocator_& );//无赋制构造与赋值
  apAllocator_& operator=(const apAllocator_&);

  char*        pRaw_;//原始指针
  T*           pData_;//排列之后的指针
  unsigned int size_;//大小，对于T来说
  unsigned int align_;//内存排列情况

  //mutable apLock lock_;//锁
};


template <class T>
void apAllocator_<T>::allocate()
{
  if (size_ == 0) {
    pData_ = new T[1];
    pRaw_  = 0;
    return;
  }
  if (align_ < 2) {
    pData_ = new T[size_];
    pRaw_  = 0;
  }
  else {
    pData_  = new unsigned char[sizeof(T) * size_ ];//+ (align_ - 1)];
    //std::cout << sizeof(T)*size_ + align_ - 1 <<std::endl;
    pRaw_= 0;
    //pData_ = alignPointer(pRaw_);
  }
}


template <class T>
void apAllocator_<T>::deallocate()
{
  if (pRaw_)
    delete [] pRaw_;
  else
    delete [] pData_;
  pRaw_  = 0;
  pData_ = 0;
}


template <class T>
T* apAllocator_<T>::alignPointer(void* raw)
{
  T* p = reinterpret_cast<T*>(
     (reinterpret_cast<uintptr_t>(raw) + align_ - 1)
     & ~(align_ - 1)
//这个如果不用reinterpret_cast之类的行不行，会不会可以，感觉是不是
//这样转换会出错，如果用强制转换不知道会不会可以
  );
  return p;
}


template <class T>
apAllocator_<T>* apAllocator_<T>::clone() const
{
  //根据应用的条件来说，好像不加锁也可以阿？？？
  //lock();// 加锁,克隆当前对象，这期间size_, align_不要被改变
         // 好像也没有人可以改变？
         // 不过其中的单个元素可能会改变，通过apAlloc的operator[]
         // 好像没有办法来限制operator[]对数据的改变
         // 不过我们会用到clone的场合一般都是调用[]然后我们要
         // make_unique，然后才会调用clone
  apAllocator_* p = new apAllocator_(this->size_, this->align_);
  std::copy(pData_, pData_ + size_, p->pData_);
         // 最后几个多出来的 浪费的的内存空间 不会拷贝
         // 这个无所谓了 反正是垃圾数据,应该是在前面多出来
  //unlock();
  return p;
}




template <class T, class A = apAllocator_<T> >
class apAlloc
{
public :
  static apAlloc& gNull();

  apAlloc();
  explicit apAlloc(unsigned int size, unsigned int align=0);
  //Ptr需要==，否则调用Ptr的operator bool
  bool isNull()  const { return pMem_ == gNull().pMem_; }
  bool isUnique()const { return pMem_.isUnique(); }

  size_t ref        () const { return pMem_.ref(); }
  unsigned int size () const { return pMem_->size(); }
  const T* data     () const { return pMem_->pData(); }
  T* data           ()       { return pMem_->pData(); }

  //这两个接口基本是弃用了，根本不会用到
  const T& operator[] (unsigned int index) const;
  T&       operator[] (unsigned int index);

  void make_unique() { pMem_.make_unique(); }
  void duplicate  () { pMem_.make_unique(); }
private :
  Ptr<A> pMem_;
  static apAlloc* sNull_;
};


template <class T, class A>
apAlloc<T, A>* apAlloc<T, A>::sNull_ = 0;


template <class T, class A>
apAlloc<T, A>& apAlloc<T, A>::gNull()
{
  if (!sNull_)
    sNull_ = new apAlloc(0);
  return *sNull_;
}


template <class T, class A>
apAlloc<T, A>::apAlloc()
{
  pMem_ = gNull().pMem_;
}


template <class T, class A>
apAlloc<T, A>::apAlloc(unsigned int size, unsigned int align)
{
  pMem_ = new A(size, align);
}


template <class T, class A>
const T& apAlloc<T, A>::operator[](unsigned int index) const
{
  if (index >= size()) throw std::range_error("Index out of range");
  return *(data() + index);
}

//这个不是线程安全的，返回引用和指针不可能做到线程安全，除非在使用的地方加锁
template <class T, class A>
T& apAlloc<T, A>::operator[](unsigned int index)
{
  if (index >= size()) throw std::range_error("Index out of range");
  pMem_.make_unique();
  return *(data() + index);
}

//在书本代码中，apAlloc的operator=没有办法对当前对象加锁，会有一个问题
/*
a=b b=c
在a=b的时候如果另一个线程在b=c这个就会混乱了，a复制的可能有以前b的一部分然后还有c的一部分
锁应该在apAlloc中也有一个,真是麻烦，还不如就在Ptr里面全局锁，感觉对多线程依赖也不强阿
我们在apImageStorage里面有了加锁操作才来进行对apAlloc的操作，所以不是太需要担心的,不对
这么说也不通阿，因为apAllocator_是在类外面可以公用的，所以还是会怕有问题，所以会有lockStorage？
那个lockStorage有没有用阿？
不对，还是可以的，apImageStorage里面已经封锁了入口，所以不会去复制了apAlloc了，所以还是安全的，应该是的
不对，
a=c
a= b  c =d 
这样对于引用计数的修改还是有问题阿，所以还是要加锁
*/
#endif
