#ifndef _lock_h
#define _lock_h


#include <pthread.h>

/*
 * apMutexAttribute
 *
 * */
class apMutexAttribute
{
public :
  static apMutexAttribute& gOnly ();

  pthread_mutexattr_t* value () { return &attr_; }

private :
  static apMutexAttribute* sOnly_;
  
  pthread_mutexattr_t attr_;

  apMutexAttribute  ();
  ~apMutexAttribute ();
};

/*
 * apLock
 *
 * */
class apLock
{
public :
  apLock  () {
    pthread_mutex_init (&lock_, apMutexAttribute::gOnly().value());
  }
  ~apLock () { pthread_mutex_destroy (&lock_); }

  apLock (const apLock&) {
    pthread_mutex_init (&lock_, apMutexAttribute::gOnly().value());
  }

  apLock& operator= (const apLock& ) { return *this; }

  bool lock   () const {
    return pthread_mutex_lock (&lock_) == 0;
  }

  bool unlock () const {
    return pthread_mutex_unlock (&lock_) == 0;
  }

private :
  mutable pthread_mutex_t lock_;
};



/*
 * apLocker
 *
 * */
class apLocker 
{
public :
  apLocker  (apLock& lock) : lock_(lock) { lock_.lock(); }
  ~apLocker () { lock_.unlock(); }

private :
  apLock& lock_;

  apLocker    (const apLocker& );
  apLocker& operator=(const apLocker& );
};

#endif
