#ifndef _thread_h
#define _thread_h


#include <pthread.h>
#include <time.h>

class apThread
{
public :
  apThread () : threadid_(0) { }
  ~apThread() { if (threadid_ != 0) stop(); }

  //int threadid () const { return (int)threadid_; }
  pthread_t threadid () const { return threadid_; }

  bool start () {
    int status;
    status = pthread_create(&threadid_, NULL, thread_, this);
    return (status == 0);
  }
  
  bool stop () {
    pthread_cancel (threadid_);
    return true;
  }

  bool wait (unsigned int seconds = 0) {
    char* rc;
    pthread_join(threadid_, (void**)&rc);
    return true;
  }

  void sleep (unsigned int msec) {
    struct timespec spec, rspec;
    spec.tv_sec = msec / 1000;
    spec.tv_nsec = (msec % 1000) * 1000000;
    nanosleep (&spec, &rspec);
  }

protected :
  pthread_t threadid_;

  //
  static void* thread_ (void* obj) {
    apThread* t = reinterpret_cast<apThread*>(obj);
    t->thread();
    return 0;
  }

  virtual void thread() { }
};

#endif
