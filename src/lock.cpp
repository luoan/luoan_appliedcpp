#include "lock.h"

apMutexAttribute* apMutexAttribute::sOnly_ = 0;

apMutexAttribute& apMutexAttribute::gOnly ()
{
  if (!sOnly_)
    sOnly_ = new apMutexAttribute ();
  return *sOnly_;
}

apMutexAttribute::apMutexAttribute ()
{
  pthread_mutexattr_init (&attr_);
  pthread_mutexattr_settype (value(),/*&attr_,*/ PTHREAD_MUTEX_RECURSIVE);
  // 不要value这个函数不可以么？直接用&attr_， 如果这个函数不是接收引用的话
  // 因为在apLock里面要用到&attr_的值,这里用&attr_应该没问题的
  //
}

apMutexAttribute::~apMutexAttribute ()
{
  pthread_mutexattr_destroy(&attr_);
}


