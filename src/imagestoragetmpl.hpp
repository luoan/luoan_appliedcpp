#ifndef _imagestoragetmpl_h
#define _imagestoragetmpl_h


#include "imagestorage.h"
#include <string.h>
#include <typeinfo>
#include <iterator>

/*
 *  apRowIterator
 *  
 * */
template <class T> class apRowIterator
{
public :
  struct current {
    T*  p;
    int bytes;
    int x;
    int y;
  };

  apRowIterator () {
    cur_.p=0; cur_.bytes=0;     cur_.x=0; cur_.y=0;
  }
  apRowIterator (T* p, int bytes, int x, int y) {
    cur_.p=p; cur_.bytes=bytes; cur_.x=x; cur_.y=y;
  }

  current*       operator->()       { return &cur_; }
  const current* operator->() const { return &cur_; }
  current&       operator* ()       { return cur_; }
  const current& operator* () const { return cur_; }

  apRowIterator& operator++ () {
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) + cur_.bytes);
    cur_.y++;
    return *this;
  }

  apRowIterator operator++ (int) {
    apRowIterator t = *this;
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) + cur_.bytes);
    cur_.y++;
    return t;
  }

  apRowIterator& operator-- () {
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) - cur_.bytes);
    cur_.y--;
    return *this;
  }

  apRowIterator operator-- (int) {
    apRowIterator t = *this;
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) - cur_.bytes);
    cur_.y--;
    return t;
  }

  apRowIterator& operator+= (int s) {
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) + s*cur_.bytes);
    cur_.y += s;
    return *this;
  }

  apRowIterator& operator -= (int s) {
    cur_.p = reinterpret_cast<T*>(
        reinterpret_cast<Pel8*>(cur_.p) - s*cur_.bytes);
    cur_.y -= s;
    return *this;
  }

  bool operator == (const apRowIterator& i) const {
    return (cur_.p == i.cur_.p);
    // &i.cur_ == &cur应该也可以
  }
  bool operator != (const apRowIterator& i) const {
    return (cur_.p != i.cur_.p);
  }

private :
  current cur_;
};


/*
 *  apPixelIterator
 *
 * */
template <class T> class apPixelIterator
{
public :
  typedef std::forward_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t differentce_type;
  typedef T* pointer;
  typedef T& reference;

  struct current {
    T*  p;//指针
    int x, y;//当前坐标
    T*  end;//当前行最后一个元素下一个元素的指针
    int x0;//行的第一个起始坐标？
    int width;//行的宽度, 就是宽度，一个宽度是一个bytesPerPixel
    int bytes;//行的实际宽度，包括了填充，以bytes为单位？从bytes大小来说应该是大于等于width因为有对齐补全
  };

  apPixelIterator () {
    cur_.p = 0; cur_.x = 0; cur_.y = 0; cur_.end = 0; cur_.x0 = 0;
    cur_.width = 0; cur_.bytes = 0;
  }
  apPixelIterator (T* p, int bytes, int x, int y, int width) {
    cur_.p = p;     cur_.x = x;
    cur_.y = y;     cur_.x0 = x;
    cur_.bytes = bytes;
    cur_.width = width;
    cur_.end   = cur_.p + cur_.width;
  }

  T*       operator->()       { return cur_.p; }
  const T* operator->() const { return cur_.p; }
  T&       operator* ()       { return *cur_.p; }
  const T& operator* () const { return *cur_.p; }

  apPixelIterator& operator++() {
    cur_.x++;
    cur_.p++;
    if (cur_.p >= cur_.end) {
      cur_.x = cur_.x0;
      cur_.y++;
      cur_.p = reinterpret_cast<T*>(reinterpret_cast<Pel8*>(cur_.p) 
          + cur_.bytes - cur_.width*sizeof(T));
      //感觉这里直接用cur_.p = cur_.p - cur_.width也可以阿
      cur_.end = cur_.p + cur_.width;
    }
    return *this;
  }

  apPixelIterator operator++(int) {
    apPixelIterator t = *this;
    cur_.x++;
    cur_.p++;
    if (cur_.p >= cur_.end) {
      cur_.x = cur_.x0;
      cur_.y++;
      cur_.p = reinterpret_cast<T*>(reinterpret_cast<Pel8*>(cur_.p)
          + cur_.bytes - cur_.width*sizeof(T));
      cur_.end = cur_.p + cur_.width;
    }
    return t;
  }

  bool operator==(const apPixelIterator& i) const {
    return (cur_.p == i.cur_.p);
  }
  bool operator!=(const apPixelIterator& i) const {
    return (cur_.p != i.cur_.p);
  }

private :
  current cur_;
};



/*
 * apImageStorage
 *
 * */
template <class T>
class apImageStorage : public apRectImageStorage
{
public :
  typedef apRowIterator<T> row_iterator;
  typedef apPixelIterator<T> iterator;

  apImageStorage () { }
  apImageStorage (const apRect& boundary,
                  eAlignment align = eNoAlign)//不用加限定符::eNoAlign？？
    //？还是说知道继承，然后我们就知道了可以用父类的东西？？？
    : apRectImageStorage(boundary, sizeof(T), align) { }
  
  virtual ~apImageStorage () { }

  const T* rowAddress (int y) const {
    return reinterpret_cast<const T*>(rowAddress_(y));
  }
  T* rowAddress (int y) {
    return reinterpret_cast<T*>(rowAddress_(y));
  }// 非线程安全, 返回原始指针无法保证线程安全，需要一个全局锁

  const T& getPixel (int x, int y) const;
  const T& getPixel (const apPoint& point) const {
    return getPixel(point.x(), point.y());
  }
  void     setPixel (int x, int y, const T& pixel);
  void     setPixel (const apPoint& point, const T& pixel) {
    setPixel (point.x(), point.y(), pixel);
  }
  //读取和写入是线程安全的,不是很清楚，可能get得到的是当前的？
  //之后是否写入导致改变我们不管
  //应该是这样，之后的我们哪里管的了

  void setRow (int y, const T& pixel);
  void setCol (int x, const T& pixel);
  //给列或行设置一样的值
  
  
  //row iterators 不是线程安全的,这是因为我们得到了迭代器
  //之后还要用，不能保证在使用迭代器的过程中对象存储不会被改变
  //而且也不能保证迭代器还是有效的，因为存储可能发生了变化，需要加锁
  row_iterator row_begin () {
    return row_iterator(
        reinterpret_cast<T*>(begin_), rowSpacing(), x0(), y0());
  }
  const row_iterator row_begin() const {
    return row_iterator(
        reinterpret_cast<T*>(begin_), rowSpacing(), x0(), y0());
  }
  row_iterator row_end() {
    return row_iterator(
        reinterpret_cast<T*>(end_), rowSpacing(), x0(), y0());
  }
  const row_iterator row_end() const {
    return row_iterator(
        reinterpret_cast<T*>(end_), rowSpacing(), x0(), y0());
  }

  //pixel iterators 不是线程安全的,我们可能给一个迭代器件赋值之后
  //apAlloc被make_unique了之类的，地址就没用了,客户要自己加锁
  iterator begin() {
    return iterator(
        reinterpret_cast<T*>(begin_), rowSpacing(), x0(), y0(), width());
  }
  const iterator begin() const {
    return iterator(
        reinterpret_cast<T*>(begin_), rowSpacing(), x0(), y0(), width());
  }
  iterator end () {
    return iterator(
        reinterpret_cast<T*>(end_), rowSpacing(), x0(), y0(), width());
  }
  const iterator end () const {
    return iterator(
        reinterpret_cast<T*>(end_), rowSpacing(), x0(), y0(), width());
  }

};



/*
 * apImageStorageLocker
 *
 * */
template <class T>
class apImageStorageLocker
{
public :
  apImageStorageLocker (const apImageStorage<T>& image) : image_(image) { image_.lock(); }
  ~apImageStorageLocker() { image_.unlock(); }
private :
  const apImageStorage<T>& image_;

  apImageStorageLocker (const apImageStorageLocker& );
  apImageStorageLocker operator=(const apImageStorageLocker& );
};



template <class T>
const T& apImageStorage<T>::getPixel (int x, int y) const 
{
  static T pixel;

  apImageStorageLocker<T> locker(*this);

  const Pel8* p = apRectImageStorage::getPixel(x, y);
  memcpy(&pixel, p, sizeof(T));

  return pixel;
}

template <class T>
void apImageStorage<T>::setPixel(int x, int y, const T& pixel)
{
  apImageStorageLocker<T> locker(*this);
  //make_unique();//duplicate()也行
  // storage_.make_unique()也可以,storage_会继承来，
  // 而且是protected所以可以访问
  // 或者把这个放到apRectImageStorage来执行,这里不执行
  // 如果想要修改共享那么就不make_unique
  const Pel8* p = reinterpret_cast<const Pel8*>(&pixel);
  apRectImageStorage::setPixel(x, y, p);
}

template <class T>
void apImageStorage<T>::setRow (int y, const T& pixel)
{
  apImageStorageLocker<T> locker(*this);

  T* p = rowAddress(y);
  for (unsigned int i = 0; i < width(); ++i) 
    *p++ = pixel;
}

template <class T>
void apImageStorage<T>::setCol(int x, const T& pixel)
{
  apImageStorageLocker<T> locker(*this);

  T* p = reinterpret_cast<T*>(getAddr(x, y0()));
  for (unsigned int i = 0; i < height(); ++i) {
    *p = pixel;
    p = reinterpret_cast<T*>(reinterpret_cast<Pel8*>(p) + rowSpacing_);
  }
}


/*
 * 非成员函数
 *
 * */
template <class T1, class T2>
void copy(const apImageStorage<T1>& src, apImageStorage<T2>& dst,
          bool fastCopy = true)
{
  if (src == dst)
    return;
  apImageStorageLocker<T1> srcLocker(src);

  if (src.boundary() != dst.boundary())
    dst = apImageStorage<T2>(src.boundary(), src.alignment());

  apImageStorageLocker<T2> dstLocker(dst);

  typename apImageStorage<T1>::row_iterator i1;
  typename apImageStorage<T2>::row_iterator i2;
  unsigned int w = src.width();
  int bytes = w * src.bytesPerPixel();

  const T1* p1;
  T2* p2;

  if (typeid(T1) == typeid(T2) && fastCopy) {
    for (i1 = src.row_begin(), i2 = dst.row_begin(); i1 != src.row_end();
        i1++, i2++) {
      p1 = i1->p;
      p2 = i2->p;
      memcpy(p2, p1, bytes);
    }
  }
  else {
    for (i1 = src.row_begin(), i2 = dst.row_begin(); i1 != src.row_end();
        i1++, i2++) {
      p1 = i1->p;
      p2 = i2->p;
      for (unsigned int x = 0; x < w; x++)
        *p2++ = static_cast<T2>(*p1++);
    }
  }
}


//其实还是有用的，复制一个一模一样的apImageStorage出来
//我们显式生成一个和src一样的dst，这两个是完全独立的，因为不共享基础存储
//apAlloc管理的内存是不一样的，只不过内存里面的内容是一样的
template <class T>//生成相同副本，感觉没什么鸟用,有用
apImageStorage<T> duplicate (const apImageStorage<T>& src,
                            apRectImageStorage::eAlignment align)
{
  apImageStorage<T> dst(src.boundary(), align);
  copy(src, dst);
  return dst;
}

#endif
