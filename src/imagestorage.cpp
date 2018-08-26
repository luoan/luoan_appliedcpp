#include "imagestorage.h"
#include <stdexcept>
#include <string.h>
#include <iostream>

apImageStorageBase::apImageStorageBase () { }

apImageStorageBase::apImageStorageBase (const apRect& boundary)
  : boundary_(boundary) { }

apImageStorageBase::~apImageStorageBase () { }



/*
 * apRectImageStorage
 *
 * */
apRectImageStorage::apRectImageStorage () : begin_(0), end_(0),
  align_(eNoAlign),  xoffset_(0), yoffset_(0),  bytesPerPixel_(0),
  rowSpacing_(0)  { }//std::cout << "apRectImageStorage default constructor\n"; }

apRectImageStorage::apRectImageStorage (const apRect& boundary,
                                        unsigned int bytesPerPixel,
                                        eAlignment align)
  : apImageStorageBase(boundary), begin_(0), end_(0),
    align_(align), xoffset_(0), yoffset_(0), bytesPerPixel_(bytesPerPixel),
    rowSpacing_(0)
{
  //std::cout << "apRectImageStorage copy constructor\n";
  rowSpacing_ = width() * bytesPerPixel_;
  if (align_ != eNoAlign) {
    int alignment = static_cast<int>(align_) - 1;
    rowSpacing_ = (rowSpacing_ + alignment) & (~alignment);
  }
  // 下面这句直接用height()也可以吧，好像也是调用boundary的吧，毕竟
  // 是继承而来的
  storage_ = apAlloc<Pel8>(rowSpacing_ * boundary.height(), 8);
  begin_   = rowAddress_(y0());
  end_     = begin_ + height() * rowSpacing_;
}


apRectImageStorage::~apRectImageStorage () { }
//析构加锁也没什么鸟用，被析构后再被复制还是有错误
//避免写出这样的程序

apRectImageStorage::apRectImageStorage (const apRectImageStorage& src)
  : apImageStorageBase (src) //这里应该是会把src切割成为一个apImageStorageBase, 然后调用编译器合成的复制构造函数，递归进行. 构造时候因该是要先构造基类
{
  //std::cout << "apRectImageStorage begin\n";
  // 对要被复制的对象进行加锁，这样我们复制过程中src不会被改变
  // 复制完成之后src再改变会有make_unique之类的操作，不用我们去关心
  apRectImageStorageLocker srcLocking(src);
  //std::cout << "apRectImageStorage copy construction lock" << std::endl;
  boundary_   = src.boundary_;//加锁后再复制一遍
  storage_    = src.storage_;
  begin_      = src.begin_;
  end_        = src.end_;
  align_      = src.align_;
  yoffset_    = src.yoffset_;
  xoffset_    = src.xoffset_;
  bytesPerPixel_ = src.bytesPerPixel_;
  rowSpacing_ = src.rowSpacing_;
}

apRectImageStorage& apRectImageStorage::operator=(
    const apRectImageStorage& src)
{
  //std::cout << "begin apRectImageStorage operator=\n";
  apRectImageStorageLocker locking(*this);
  // 这个加锁到底是什么情况？？不能改变内容？？？
  // 什么时候不能改变能容
  // 把自己锁上，按道理书本代码apAlloc里面就要这样做才对
  // 有两个锁才对，一个锁自己，一个锁src
  // 但是两个锁仍然对析构无力阿，因为刚好再析构，然后被调用operator=或者赋值构造，那么还是有问题阿（虽然概率小）多线程要避免写出这样的代码，这个是自己要做的，只能从自己方面努力。用一个static锁也不能解决问题
  // 书本代码的apAlloc的operator=之所以不能对当前对象加锁是因为锁不是自己的，
  // pMem_所指向的，加锁之后没有办法解锁，不是一个实体，而对于apRectImageStorage来说锁是类自己定义了一个锁实体的，
  // 所以可以src和当前对象都加锁
  //
  //std::cout << "apRectImageStroage operator= get first lock\n";
  if (this != &src) {
    apRectImageStorageLocker srcLocking (src);
    //std::cout << "apRectImageStorage operator= get second lock\n";
    boundary_   = src.boundary_;

    storage_    = src.storage_;
    begin_      = src.begin_;
    end_        = src.end_;
    align_      = src.align_;
    yoffset_    = src.yoffset_;
    xoffset_    = src.xoffset_;
    bytesPerPixel_ = src.bytesPerPixel_;
    rowSpacing_  = src.rowSpacing_;
  }

  return *this;
}


bool apRectImageStorage::window (const apRect& window)
{
  apRectImageStorageLocker locking(*this);

  apRect intersect = boundary().intersect(window);
  if (intersect.isNull()) {
    init ();//没有交集，所以把当前图像设为空
    return false;
  }

  xoffset_  += intersect.x0() - x0();
  yoffset_  += intersect.y0() - y0();
  boundary_ =  intersect;
  begin_    =  rowAddress_(y0());
  end_      =  begin_ + height() * rowSpacing_;
  //这个不对阿，end_没什么鸟用
  return true;
}

void apRectImageStorage::init()
{
  boundary_   = apRect();
  xoffset_    = 0;
  yoffset_    = 0;
  bytesPerPixel_ = 0;
  rowSpacing_ = 0;
  storage_    = apAlloc<Pel8>();
  begin_      = 0;
  end_        = 0;
}

Pel8* apRectImageStorage::rowAddress_(int y)
{
  if (y < y0() || y >= y1())
    throw std::runtime_error("apRectImageStorage::rowAddress_");

  y -= y0();
  int offset = (y + yoffset_) * rowSpacing_
     + xoffset_ * bytesPerPixel_;
  Pel8* p = storage_.data() + offset;
  return p;
}

const Pel8* apRectImageStorage::rowAddress_(int y) const
{
  return const_cast<apRectImageStorage*>(this)->rowAddress_(y);
}

Pel8* apRectImageStorage::getAddr (int x, int y)
{
  if (x < x0() || x >= x1())
    throw std::domain_error("apRectImageStorage::getAddr");
  return rowAddress_(y) + (x - x0()) * bytesPerPixel_;
}

const Pel8* apRectImageStorage::getAddr (int x, int y) const
{
  return const_cast<apRectImageStorage*>(this)->getAddr(x, y);
}

const Pel8* apRectImageStorage::getPixel (int x, int y) const
{
  return getAddr(x, y);
}

void apRectImageStorage::setPixel(int x, int y,
                                  const Pel8* pixel)
{//make_unique可以在这里或者是再apImageStorage里面执行
  Pel8* addr = getAddr (x, y);
  memcpy(addr, pixel, bytesPerPixel_);
}

void apRectImageStorage::rebase()
{
  storage_.duplicate ();
  //这个我们因该不需要，因为alloctmpl里面我们用ptr来管理指针
  // 还是需要的，这样我们可以显式要修唯一，而不是等到复制时候来做
  // 这样方便一些，且直观一点
  boundary_ = apRect (0, 0, width(), height());
  begin_    = rowAddress_(y0());
  end_      = begin_ + height() * rowSpacing_;
}


