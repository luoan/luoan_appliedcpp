#ifndef _imagestorage_h
#define _imagestorage_h


#include "geometry.h"
#include "alloctmpl.hpp"
#include "lock.h"


/*
 * apImageStorageBase
 *
 * */
class apImageStorageBase
{
public :
  apImageStorageBase ();
  apImageStorageBase (const apRect& boundary);

  virtual ~apImageStorageBase ();

  const apRect& boundary () const { return boundary_; }

  int x0  () const { return boundary_.x0(); }
  int y0  () const { return boundary_.y0(); }
  int x1  () const { return boundary_.x1(); }
  int y1  () const { return boundary_.y1(); }
  unsigned int width () const { return boundary_.width(); }
  unsigned int height() const { return boundary_.height(); }

protected :
  apRect boundary_;
};


/*
 * apRectImageStorage
 *
 * */
class apRectImageStorage : public apImageStorageBase
{
public :
  enum eAlignment { eNoAlign=0, eWordAlign=2, eDoubleWordAlign=4, eQuadWordAlign=8,
                  e2ByteAlign=2, e4ByteAlign=4, e8ByteAlign=8, e16ByteAlign=16};

  apRectImageStorage ();
  apRectImageStorage (const apRect& boundary, unsigned int bytesPerPixel,
                      eAlignment align);

  apRectImageStorage (const apRectImageStorage& );
  apRectImageStorage& operator= (const apRectImageStorage& );

  virtual ~apRectImageStorage ();

  bool isNull  () const { return storage_.isNull(); }
  bool isUnique() const { return storage_.isUnique(); }
  size_t ref   () const { return storage_.ref(); }

  bool lockState  () const { /*std::cout << "call lockstate\n";*/ return lock_.lock(); }
  bool unlockState() const { return lock_.unlock(); }
  // lockStorage 交给Ptr去做了还是说给apAllocatorBase???
  // 我在最新的实现里面交给Ptr去做了,在apAlloc的实现里面没有lock
  bool lock  () const { /*std::cout << "call lock\n";*/ return lockState(); }
  bool unlock() const { return unlockState(); }

  int xoffset     () const { return xoffset_; }
  int yoffset     () const { return yoffset_; }
  int bytesPerPixel() const { return bytesPerPixel_; }
  int rowSpacing  () const { return rowSpacing_; }

  eAlignment alignment () const { return align_; }

  bool operator== (const apRectImageStorage& src) const {
    return storage_.data() == src.storage_.data();
  }
  bool operator!= (const apRectImageStorage& src) const {
    return storage_.data() != src.storage_.data();
  }

  bool window (const apRect& window);

  const Pel8* rowAddress_ (int y) const;
  Pel8*       rowAddress_ (int y);

  const Pel8* getAddr (int x, int y) const;
  Pel8*       getAddr (int x, int y);

  void rebase ();

  //duplicate make_unique是一样的
  void duplicate  () { storage_.duplicate(); }
  void make_unique() { storage_.make_unique(); }
protected :
  const Pel8* getPixel (int x, int y) const;//读写锁定是交给更高层来实现
  void        setPixel (int x, int y, const Pel8* pixel);

  void init();//变成空图像

  mutable apLock lock_;// 锁
  apAlloc<Pel8> storage_;// apAlloc对象，默认T为char
  Pel8*         begin_;  // 指向第一个元素的指针
  Pel8*         end_; //指向最后一个元素下一个位置的指针
  eAlignment    align_;// 几个字节对齐
  unsigned int  xoffset_;//偏移，暂时不知道是什么
  unsigned int  yoffset_; //
  unsigned int  bytesPerPixel_;//一个像素多少bytes
  unsigned int  rowSpacing_;
  // 一行的实际bytes数，包括了补全，不包括alignPointer里面的补全
  // 那个是基础内存分配里面自己多出来的align_-1个 bytes
  // 这里的补全每一行都是有补全的
  // 代码里面storage_构造的align_是等于8的，所以apAlloc里面不用再补全了
  // 如果storage_构造的align_设置成了7（举个例子），那么就要多出6个Pel8
  // 浪费0-6个
};


/*
 * apRectImageStorageLocker
 *
 * */
class apRectImageStorageLocker
{
public :
  apRectImageStorageLocker (const apRectImageStorage& image) : image_(image) {
    image_.lockState();
  }
  ~apRectImageStorageLocker () { image_.unlockState(); }

private :
  const apRectImageStorage& image_;

  apRectImageStorageLocker (const apRectImageStorageLocker&);
  apRectImageStorageLocker& operator=(const apRectImageStorageLocker& );
};


#endif
