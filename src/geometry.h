#ifndef _geometry_h
#define _geometry_h


#include <iostream>
#include <utility>

typedef unsigned char  Pel8;     // 1-byte
typedef unsigned short Pel16;    // 2-bytes
typedef unsigned int   Pel32;    // 4-bytes  (Unsigned)
typedef int            Pel32s;   // 4-bytes  (Signed)

template <class T> 
const T& apMin (const T& a, const T& b)
{
  return (a < b) ? a : b;
}

template <class T>
const T& apMax (const T& a, const T& b)
{
  return (a > b) ? a : b;
}

class apPoint 
{
public :
  apPoint () : x_(0), y_(0) { }
  apPoint (std::pair<int, int> p) : x_(p.first), 
      y_(p.second) { }

  apPoint (int x, int y) : x_(x), y_(y) { }

  int x () const { return x_; }
  int y () const { return y_; }
  std::pair<int, int> point () const {
    return std::pair<int, int>(x_, y_);
  }

  bool operator == (const apPoint& p) const {
    return x() == p.x() && y() == p.y();
  }

  apPoint& operator += (const apPoint& p) {
    x_ += p.x(); y_ += p.y(); return *this;
  }

private :
  int x_, y_;
};

std::ostream& operator<< (std::ostream& os, const apPoint& );
std::istream& operator>> (std::istream& is, apPoint& );


class apRect 
{
public :
  apRect ();
  apRect (apPoint ul, unsigned int width, unsigned int height);
  apRect (apPoint ul, apPoint lr);
  apRect (int x0, int y0, unsigned int width, unsigned int height);

  const   apPoint& ul () const { return ul_; }
  apPoint          lr () const ;

  int x0 () const { return ul_.x(); }
  int y0 () const { return ul_.y(); }
  int x1 () const { return lr().x(); }
  int y1 () const { return lr().y(); }

  unsigned int width () const { return width_; }
  unsigned int height() const { return height_; }

  bool isNull () const { return width_ == 0 || height_ == 0; }

  bool operator == (const apRect& r) const;
  bool operator != (const apRect& r) const {
    return !operator==(r);
  }

  bool within (const apPoint& p) const;

  apRect intersect (const apRect& r) const;

  void expand (int x, int y);

private :
  apPoint ul_;
  unsigned int width_;
  unsigned int height_;
};

std::ostream& operator<< (std::ostream& os, const apRect& );
std::istream& operator>> (std::istream& is, apRect& );

#endif
