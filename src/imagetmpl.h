#ifndef _imagetmpl_h
#define _imagetmpl_h


#include "imagetypes.h"
#include "imagestoragetmpl.hpp"


template <class T, class S> class apImage;
template <class T, class S> void set (apImage<T, S>& image, const T& value);


template <class T, class S=apImageStorage<T> >
class apImage
{
public :
  typedef typename S::row_iterator row_iterator;
  typedef typename S::iterator iterator;

  static apImage sNull;

  apImage () { }
  apImage (const apRect& boundary,
          apRectImageStorage::eAlignment align =
          apRectImageStorage::eNoAlign)
    : storage_(boundary, align) { }

  apImage (S storage) : storage_(storage) { }

  virtual ~apImage () { }
  apImage  (const apImage& src);
  apImage& operator=(const apImage& src);

  template <class T1, class S1>
  apImage (const apImage<T1, S1>& src) {
    copy (src.storage(), storage());
  }
  template <class T1, class S1>
  apImage& operator=(const apImage<T1, S1>& src) {
    copy (src.storage(), storage());
    return *this;
  }

  bool lockImage  () const { return storage_.lock(); }
  bool unlockImage() const { return storage_.unlock(); }
  //这两个lock是一样的，没什么差别，因为再实现里面我
  //在Ptr里面加锁，所以对底层数据修改不可共享,需要make_unique或者duplicate
  bool lockState  () const { return storage_.lockState(); }
  bool unlockState() const { return storage_.unlockState(); }

  bool isNull     () const { return storage_.isNull(); }
//int  ref        () const { return storage_.ref(); }
  bool isUnique   () const { return storage_.isUnique(); }
  size_t ref      () const { return storage_.ref(); }
  int  xoffset    () const { return storage_.xoffset(); }
  int  yoffset    () const { return storage_.yoffset(); }

  unsigned int bytesPerPixel () const { return storage_.bytesPerPixel(); }
  unsigned int rowSpacing    () const { return storage_.rowSpacing(); }

  apRectImageStorage::eAlignment alignment () const {
    return storage_.alignment();
  }

  const S& storage () const { return storage_; }
  S&       storage ()       { return storage_; }

  const apRect& boundary () const { return storage_.boundary(); }
  int              x0    () const { return storage_.x0(); }
  int              y0    () const { return storage_.y0(); }
  int              x1    () const { return storage_.x1(); }
  int              y1    () const { return storage_.y1(); }
  unsigned int     width () const { return storage_.width(); }
  unsigned int     height() const { return storage_.height(); }

  const T* base      ()      const { return rowAddress(y0()); }
  T*       base      ()            { return rowAddress(y0()); }
  const T* rowAddress(int y) const { return storage_.rowAddress(y); }
  T*       rowAddress(int y)       { return storage_.rowAddress(y); }

  const T& getPixel (int x, int y)         const   { return storage_.getPixel(x, y); }
  const T& getPixel (const apPoint& point) const   { return storage_.getPixel(point); }
  void     setPixel (int x, int y, const T& pixel) { storage_.setPixel(x, y, pixel); }
  void     setPixel (const apPoint& point, const T& pixel) { storage_.setPixel(point, pixel); }

  void setRow (int y, const T& pixel) { return storage_.setRow(y, pixel); }
  void setCol (int x, const T& pixel) { return storage_.setCol(x, pixel); }

  row_iterator       row_begin ()       { return storage_.row_begin(); }
  const row_iterator row_begin () const { return storage_.row_begin(); }
  row_iterator       row_end   ()       { return storage_.row_end(); }
  const row_iterator row_end   () const { return storage_.row_end(); }

  iterator       begin ()       { return storage_.begin(); }
  const iterator begin () const { return storage_.begin(); }
  iterator       end   ()       { return storage_.end(); }
  const iterator end   () const { return storage_.end(); }

  bool window (const apRect& window) {
    return storage_.window(window);
  }

  void trim ();//线程安全的用于make_unique的程序

  void rebase ();

  apRectImageStorage::eAlignment bestAlignment () const;

  apImage<T, S> align (apRectImageStorage::eAlignment align =
                       apRectImageStorage::eNoAlign);
  //这里的模板参数T, S都是多余的,duplicate函数返回当前apImage的一个
  //副本，不共享apAlloc，而数据都是一样的
  apImage<T, S> duplicate (apRectImageStorage::eAlignment align =
                           apRectImageStorage::eNoAlign) const;

  void set (const T& value) { ::set(*this, value); }
  void add (const T& value);
  void sub (const T& value);
  void mul (const T& value);
  void div (const T& value);
  void scale (float scaling);

  bool isIdentical (const apImage& src) const;

  bool operator== (const apImage& src) const;
  bool operator!= (const apImage& src) const {
    return !operator==(src);
  }

protected :
  S storage_;
};

//一个线程可以获得多个锁,
//就是说可以对一个东西锁多次
//应该是这样吧，不然没法解释居然可以锁两次且执行代码
template <class T, class S=apImageStorage<T> >
class apImageLocker
{
public :
  apImageLocker (const apImage<T, S>& image, bool fullLocking = true)
    : image_(image), fullLocking_(fullLocking), locked_(false)
  { lock(); }
  ~apImageLocker () { unlock(); }

  void lock () {
    if (!locked_)
      fullLocking_ ? image_.lockImage() : image_.lockState();
    locked_ = true;
  }
  void unlock () {
    if (locked_)
      fullLocking_ ? image_.unlockImage() : image_.unlockState();
    locked_ = false;
  }
private :
  bool  fullLocking_;
  bool  locked_;
  const apImage<T, S>& image_;

  apImageLocker  (const apImageLocker& );
  apImageLocker& operator=(const apImageLocker& );
};



template <class T, class S>
apImage<T, S> apImage<T, S>::sNull;

template <class T, class S>
apImage<T, S>::apImage(const apImage& src)
{
  //std::cout << "apImage copy constructor begin\n";
  apImageLocker<T, S> srcLocking(src, false);
  //std::cout << "apImage copy construction after lock\n";
  storage_  = src.storage_;
  //std::cout << "apImage copy constructor end\n";
}
//会锁两次，一个进程可以获得多个锁
//但锁只能给一个进程
//比如赋值构造函数首先构造
//apImageStorageBase apRectImageStorage apImageStorage
//然后进入构造函数实体
//锁住已经构造的apRectImageStorage的lock
//storage_ = src.storage_;
//此处调用的是apImageStorage的编译器合成的operator=
//这个operator=会调用apRectImageStorage的operator=
//又会锁一次apRectImageStorage的lock
template <class T, class S>
apImage<T, S>& apImage<T, S>::operator= (const apImage& src)
{
  //std::cout << "apImage operator= begin\n";
  if (storage_ != src.storage_) {

    apImageLocker<T, S> locking(*this, false);
    //std::cout << "apImage operator= get lock\n";
    apImageLocker<T, S> lockingsrc(src, false);//树上代码这个没有锁定
    //感觉要锁一下比较好把,但是可能是这样阿，对象已经没有了，然后要解锁，
    //没办法解锁，ptr是全局的锁所以没事

    //this锁住????
    //这里呢，复制构造的话，lock_会不会没了
    //感觉也不会吧，因为lock在storage里面
    //我们调用apImageStorage编译器合成的operator=
    //然后调用apRectImageStorage的operator=，那个lock还是会在的，不会复制掉，所以感觉这里可以锁一下
    storage_ = src.storage_;
  }
  //std::cout << "apImage operator= before return\n";
  return *this;
}

template <class T, class S>//make_unique
void apImage<T, S>::trim ()
{
  apImageLocker<T, S> locking(*this, false);
  if (isNull() || isUnique())//||ref() ==1???
    return;

  S storage;
  copy (storage_, storage);
  storage_ = storage;

  //这个函数我们应该不需要把??????????
  //感觉还是需要一下，用来显式make_unique
  //这里我们是想要不共享apAlloc基础存储
  //其实只要调用make_unique就可以了，不用使用那个copy
  //那个copy用于不同对象之间复制用
  //这里其实只要调用apRectImageStorage的make_unique或者是duplicate
  //就可以了
  //直接storage.duplicate()或者storage.make_unique就可以了
  //storage.duplicate();//错误
  //不对，这个不对，duplicate不是成员函数，而是外部函数
  //所以应该这样写
  //stroage_ = duplicate(stroage_, alignment());
  //这样不如直接用上面书上的代码
}

template <class T, class S>
void apImage<T, S>::rebase ()
{
  trim ();//ptr管理内存，这个应该不需要？？？
  //还是需要的，感觉有点用,显式make_unique
  //这里不调用trim也可以吧，因为rebase里面就会duplicate(make_unique)
  //所以前面调用一次trim有点多余
  storage_.rebase ();
}

template <class T, class S>
apRectImageStorage::eAlignment apImage<T, S>::bestAlignment () const
{
  if (isNull())
    return apRectImageStorage::eNoAlign;

  lockImage();
  const T* p1 = rowAddress(y0());
  const T* p2 = p1;
  if (height() > 1)
    p2 = rowAddress(y0() + 1);
  unlockImage();

  uintptr_t addr1 = reinterpret_cast<uintptr_t>(p1);
  uintptr_t addr2 = reinterpret_cast<uintptr_t>(p2);

  if (addr1 % apRectImageStorage::eQuadWordAlign == 0 &&
      addr2 % apRectImageStorage::eQuadWordAlign == 0)
    return apRectImageStorage::eQuadWordAlign;

  else if (addr1 % apRectImageStorage::eDoubleWordAlign == 0 &&
      addr2 % apRectImageStorage::eDoubleWordAlign == 0)
    return apRectImageStorage::eDoubleWordAlign;

  else if (addr1 % apRectImageStorage::eWordAlign == 0 &&
      addr2 % apRectImageStorage::eWordAlign == 0)
    return apRectImageStorage::eWordAlign;

  return apRectImageStorage::eNoAlign;
}

template <class T, class S>
apImage<T, S> apImage<T, S>::align (apRectImageStorage::eAlignment align)
{
  apImageLocker<T, S> locking(*this);

  if (isNull() || align == apRectImageStorage::eNoAlign)
    return *this;

  const T* p1 = rowAddress(y0());
  const T* p2 = p1;
  if (height() > 1)
    p2 = rowAddress(y0() + 1);

  if (reinterpret_cast<uintptr_t>(p1) % align == 0 &&
      reinterpret_cast<uintptr_t>(p2) % align == 0)
    return *this;

  apImage<T, S> dup (::duplicate(storage_, align));

  return dup;
}

template <class T, class S>
apImage<T, S> apImage<T, S>::duplicate (apRectImageStorage::eAlignment align) const
{
  apImageLocker<T, S> locking (*this);
  apImage<T, S> dup (::duplicate(storage_, align));
  return dup;
}

template <class T, class S>
void set (apImage<T, S>& image, const T& value)
{
  apImageLocker<T, S> srclocking(image);
  typename apImage<T, S>::row_iterator i;
  unsigned int w = image.width();
  T* p;
  for (i = image.row_begin(); i != image.row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ = value;
  }
}

template <> void set (apImage<Pel8>& s, const Pel8& value);
template <> void set (apImage<Pel32s>& s, const Pel32s& value);
template <> void set (apImage<apRGB>& s, const apRGB& value);

template <class T, class S>
void apImage<T, S>::add (const T& value)
{
  apImageLocker<T, S> locking(*this);

  typename apImage<T, S>::row_iterator i;
  unsigned int w = width();
  T* p;
  for (i = row_begin(); i != row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ += value;
    //apLimit???应该会调用apRGB的operator+=
    //但是如果不是的话那么就不会调用了，会益处
  }
}

template <class T, class S>
void apImage<T, S>::sub (const T& value)
{
  apImageLocker<T, S> locking(*this);

  typename apImage<T, S>::row_iterator i;
  unsigned int w = width();
  T* p;
  for (i = row_begin(); i != row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ -= value;
  }
}

template <class T, class S>
void apImage<T, S>::mul (const T& value)
{
  apImageLocker<T, S> locking(*this);

  typename apImage<T, S>::row_iterator i;
  unsigned int w = width();
  T* p;
  for (i = row_begin(); i != row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ *= value;
  }
}

template <class T, class S>
void apImage<T, S>::div (const T& value)
{
  apImageLocker<T, S> locking(*this);

  typename apImage<T, S>::row_iterator i;
  unsigned int w = width();
  T* p;
  for (i = row_begin(); i != row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ /= value;
  }
}

template <class T, class S>
void apImage<T, S>::scale (float scaling)
{
  apImageLocker<T, S> locking(*this);

  typename apImage<T, S>::row_iterator i;
  unsigned int w = width();
  T* p;
  for (i = row_begin(); i != row_end(); i++) {
    p = i->p;
    for (unsigned int x = 0; x < w; x++)
      *p++ = ::scale<T>(*p, scaling);
  }
}


template <class T, class S>
bool apImage<T, S>::isIdentical (const apImage& src) const
{
  apImageLocker<T, S> locking(*this);
  apImageLocker<T, S> srcLocking(src);

  bool state = true;
  if (boundary() != src.boundary() ||
      base() != src.base())
    state = false;

  return state;
}

template <class T, class S>
bool apImage<T, S>::operator== (const apImage<T, S>& src) const
{
  apImageLocker<T, S> locking(*this);
  apImageLocker<T, S> srcLocking(src);

  if (boundary() != src.boundary())
    return false;

  typename apImage<T, S>::row_iterator i1;
  typename apImage<T, S>::row_iterator i2;
  unsigned int w = width();
  const T* p1;
  const T* p2;
  for (i1 = row_begin(), i2 = row_begin(); i1 != row_end(); i1++, i2++) {
    p1 = i1->p;
    p2 = i2->p;
    for (unsigned int x = 0; x < w; x++)
      if (*p1++ != *p2++)
        return false;
  }
  return true;
}



#endif
