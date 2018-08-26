#ifndef _ptr_hpp
#define _ptr_hpp


#include "lock.h"
#include <stddef.h>
#include <stdexcept>
#include <iostream>

template <class T>
class Ptr
{
public :
  Ptr () : p(0), refptr(new size_t(1)) { }
  Ptr (T* t) : p(t), refptr(new size_t(1)) { }

  Ptr (const Ptr& rhs) /*: p(rhs.p), refptr(rhs.refptr)*/ {
    apLocker l(lock_);
    //lock();
    //rhs->lock();
    p = rhs.p;
    refptr = rhs.refptr;
    ++*refptr;
    //rhs->unlock();
    //unlock();
  }
  Ptr& operator= (const Ptr& rhs) {
    apLocker l(lock_);
    //lock();
    //rhs->lock();
    ++*rhs.refptr;
    if (--*refptr == 0) {
      delete p;
      delete refptr;
    }
    p = rhs.p;
    refptr = rhs.refptr;
    //rhs->unlock();
    //unlock();//其实apLocker和这个没有什么区别，锁在多线程并不能保证你各个
                //东西先后，但是可以保证原子性，这里用apLocker和单独lock都是没有太大区别的
                //顶多用了aplocker后可以保证返回前不会改变，而之用lock unlock返回前可能会改变而已
    return *this;
  }
  ~Ptr () {
    apLocker l(lock_);
    //lock();
    if (--*refptr == 0) {
      delete p; delete refptr;
    }
    //unlock();
  }

  void make_unique() {
    apLocker l(lock_);
    //lock();
    if (*refptr != 1) {
      --*refptr;
      p = p ? clone(p) : 0;
      refptr = new size_t(1);
    }
    //unlock();
  }

  //这里用src.p == p 好像不行？可以的，要用return ， return不要忘记了阿！！！
  //定义为非成员的好像没见过，只见过非成员的operator+
  bool operator== (const Ptr& src) const { /*return src.p == p;*/return this == &src; }
  bool operator!= (const Ptr& src) const { return operator==(src); }

  operator bool    () const { return p; }
  T* operator->    () const { if (p) return p;  throw std::runtime_error("unbound Ptr"); }
  T& operator*     () const { if (p) return *p; throw std::runtime_error("unbound Ptr"); }

  bool isUnique () const { return *refptr == 1; }
  size_t ref    () const { return *refptr; }
  //static bool lock  () /*const*/ { return lock_.lock(); }
  //static bool unlock() /*const*/ { return lock_.unlock(); }

private :
  T* p;
  size_t* refptr;
  static apLock lock_;//要有初始值
};

template <class T>
apLock Ptr<T>::lock_;//赋初值, lock_()这个是错的

template <class T>
T* clone(const T* tp)
{
  //std::cout << "make_unique   use T* clone() " << std::endl;
  return tp->clone();
}

#endif
