#include "geometry.h"

#include <iostream>

std::ostream& operator<< (std::ostream& os, const apPoint& point)
{
  os << "apPoint (x, y) is: (" << point.x() << " " << point.y() << ")";

  return os;
}

std::istream& operator>> (std::istream& is, apPoint& point)
{

  int x, y;
  if (is) {
    //std::cout << "please input x and y: ";
    is >> x >> y;
  }
  point = apPoint(x, y);
  return is;
}


/*
 * apRect
 * */
apRect::apRect () : ul_(apPoint(0, 0)), width_(0), height_(0) { }

apRect::apRect (apPoint ul, unsigned int width, unsigned int height) 
  : ul_(ul), width_(width), height_(height) { }

apRect::apRect (apPoint ul, apPoint lr)
  : ul_(ul), width_(lr.x() - ul.x()), height_(lr.y() - ul.y()) { }

apRect::apRect (int x0, int y0, unsigned int width, unsigned int height)
  : ul_ (apPoint(x0, y0)), width_(width), height_(height) { }

apPoint apRect::lr () const 
{
  return apPoint (ul_.x() + width_, ul_.y() + height_);
}

bool apRect::operator== (const apRect& r) const 
{
  return (ul() == r.ul() &&
              width() == r.width() && height() == r.height());
}

bool apRect::within (const apPoint& p) const 
{
  apPoint lr (ul_.x() + width_, ul_.y() + height_);
  return (ul_.x() <= p.x()) && (lr.x() > p.x())
    && (ul_.y() <= p.y()) && (lr.y() > p.y());
}

apRect apRect::intersect (const apRect& r) const 
{
  const apPoint& ul1 = ul();
  const apPoint& ul2 = r.ul();
  int x = apMax (ul1.x(), ul2.x());
  int y = apMax (ul1.y(), ul2.y());
  int w = apMin (ul1.x() + width(), ul2.x() + r.width()) - x;
  int h = apMin (ul1.y() + height(), ul2.y() + r.height()) - y;

  if (w < 0 || h < 0) 
    return apRect ();

  return apRect (x, y, w, h);
}

void apRect::expand (int x, int y)
{
  if (!isNull()) {
    ul_ += apPoint (-x, -y);
    width_ += 2*x;
    height_ += 2*y;
  }
}

std::ostream& operator<< (std::ostream& os, const apRect& rect)
{
  os << "apRect ul is: (" << rect.x0() << " " << rect.y0() 
    << ")\t width is: " << rect.width() << " height is:" << rect.height();
  return os;
}

std::istream& operator>> (std::istream& is, apRect& rect)
{
  int x, y;
  unsigned int width, height;

  is >> x >> y >> width >> height;
  apRect r (x, y, width, height);
  rect = r;
  return is;
}


