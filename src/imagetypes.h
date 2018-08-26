#ifndef _imagetypes_h_
#define _imagetypes_h_


#include "geometry.h"
#include <iostream>

template <class T>
class apLimitInfo
{
public :
  static apLimitInfo<T> sType;

  T minValue;
  T maxValue;

private :
  apLimitInfo(T min, T max) : minValue(min), maxValue(max) { }
};

template <class D, class S>
D apLimit(const S& src)
{
  return src < apLimitInfo<D>::sType.minValue ? apLimitInfo<D>::sType.minValue :
    (src > apLimitInfo<D>::sType.maxValue ? apLimitInfo<D>::sType.maxValue : static_cast<D>(src));
}


template <class D>
D add2 (Pel32s s1, Pel32s s2)
{
  return static_cast<D>(s1 + s2);
}

template <class D>
D sub2 (Pel32s s1, Pel32s s2)
{
  return static_cast<D>(s1 - s2);
}

template <class D>
D mul2 (Pel32s s1, Pel32s s2)
{
  return static_cast<D>(s1 * s2);
}

template <class D>
D div2 (Pel32s s1, Pel32s s2)
{
  return static_cast<D>(s1 / s2);
}

template <class D>
D scale(Pel32s s1, float scaling)
{
  return static_cast<D>(scaling * s1);
}

template <class D>
D scale (Pel8 s1, float scaling)
{
  return static_cast<D>(scaling * s1);
}

template <class D>
D scale (Pel16 s1, float scaling)
{
  return static_cast<D>(scaling * s1);
}

template <class D>
D scale (double s1, float scaling)
{
  return static_cast<D>(scaling * s1);
}




template <class T>
class apClampedTmpl
{
public :
  T val;

  apClampedTmpl ()    : val(0) { }
  apClampedTmpl (T v) : val(v) { }

  operator T () const { return val; }

  apClampedTmpl (const apClampedTmpl& src) {
    val = apLimit<T>(src.val);
  }

  template <class T1> apClampedTmpl(const apClampedTmpl& src) {
    val = apLimit<T>(src.val);
  }

  apClampedTmpl<T>& operator=(const apClampedTmpl& src) {
    val = apLimit<T>(src.val);
    return *this;
  }//这里apClampedTmpl不加上<T>因该也没问题

  apClampedTmpl<T>& operator+= (const apClampedTmpl<T>& s) {
    val = apLimit<T>(val + s.val);
    return *this;
  }

  apClampedTmpl<T>& operator-= (const apClampedTmpl<T>& s) {
    val = apLimit<T>(val - s.val);
    return *this;
  }

  apClampedTmpl<T>& operator*= (const apClampedTmpl<T>& s) {
    val = apLimit<T>(val * s.val);
    return *this;
  }

  apClampedTmpl<T>& operator/= (const apClampedTmpl<T>& s) {
    val = apLimit<T>(val / s.val);
    return *this;
  }

  template <class T2>
  apClampedTmpl<T> operator+ (const apClampedTmpl<T2>& s2) {
    apClampedTmpl<T> dst;
    dst.val = apLimit<T>(val + s2.val);
    return dst;
  }

};

typedef apClampedTmpl<Pel8>   apClampedPel8;
typedef apClampedTmpl<Pel16>  apClampedPel16;
typedef apClampedTmpl<Pel32>  apClampedPel32;
typedef apClampedTmpl<Pel32s> apClampedPel32s;

template <class T>
bool operator== (const apClampedTmpl<T>& s1, const apClampedTmpl<T>& s2)
{
  return s1.val == s2.val;
}

template <class T>
bool operator!= (const apClampedTmpl<T>& s1, const apClampedTmpl<T>& s2)
{
  return s1.val != s2.val;
}

template <class T>
bool operator== (const apClampedTmpl<T>& s1, const T& s2)
{
  return s1.val == s2;
}

template <class T>
bool operator!= (const apClampedTmpl<T>& s1, const T& s2)
{
  return s1.val != s2;
}


template <class T1, class T2>
apClampedTmpl<T1> operator+ (const apClampedTmpl<T1>& s1, const T2& s2)
{
  apClampedTmpl<T1> dst;
  dst.val = apLimit<T1>(s1.val + s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator- (const apClampedTmpl<T1>& s1, const T2& s2)
{
  apClampedTmpl<T1> dst;
  dst.val = apLimit<T1>(s1.val - s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator* (const apClampedTmpl<T1>& s1, const T2& s2)
{
  apClampedTmpl<T1> dst;
  dst.val = apLimit<T1>(s1.val * s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator/ (const apClampedTmpl<T1>& s1, const T2& s2)
{
  apClampedTmpl<T1> dst;
  dst.val = apLimit<T1>(s1.val / s2);
  return dst;
}

template <class T>
std::ostream& operator<< (std::ostream& os, const apClampedTmpl<T>& val)
{
  os << val.val;
  return os;
}

template <> std::ostream& operator<< (std::ostream&, const apClampedTmpl<Pel8>& rgb);




template <> apClampedPel8 apLimit<apClampedPel8, apClampedPel32s> (const apClampedPel32s& src);

template <> Pel8   apLimit<Pel8,   Pel32s> (const Pel32s& src);
template <> Pel8   apLimit<Pel8,   Pel8>   (const Pel8& src);
template <> Pel32s apLimit<Pel32s, Pel32s> (const Pel32s& src);
template <> Pel32s apLimit<Pel32s, Pel32>  (const Pel32& src);

template <> apClampedPel8 apLimit<apClampedPel8, apClampedPel8> (const apClampedPel8 &);

template <> apClampedPel32s apLimit<apClampedPel32s, apClampedPel32s> (const apClampedPel32s& );


template <class D, class S1, class S2>
D add2 (const apClampedTmpl<S1>& s1, const apClampedTmpl<S2>& s2)
{
  return apLimit<D>(s1.val + s2.val);
}

template <class D, class S1, class S2>
D sub2 (const apClampedTmpl<S1>& s1, const apClampedTmpl<S2>& s2)
{
  return apLimit<D>(s1.val - s2.val);
}

template <class D, class S1, class S2>
D mul2 (const apClampedTmpl<S1>& s1, const apClampedTmpl<S2>& s2)
{
  return apLimit<D>(s1.val * s2.val);
}

template <class D, class S1, class S2>
D div2 (const apClampedTmpl<S1>& s1, const apClampedTmpl<S2>& s2)
{
  return apLimit<D>(s1.val / s2.val);
}

template <class D, class S1>
D scale (const apClampedTmpl<S1>& s1, float scaling)
{
  return apLimit<D>(scaling * static_cast<float>(s1.val));
}


template <class T1, class T2>
apClampedTmpl<T1> operator+ (const apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  apClampedTmpl<T1> dst = add2<apClampedTmpl<T1>, apClampedTmpl<T1>, apClampedTmpl<T2> >(s1, s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator- (const apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  apClampedTmpl<T1> dst = sub2<apClampedTmpl<T1>, apClampedTmpl<T1>, apClampedTmpl<T2> >(s1, s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator* (const apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  apClampedTmpl<T1> dst = mul2<apClampedTmpl<T1>, apClampedTmpl<T1>, apClampedTmpl<T2> >(s1, s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1> operator/ (const apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  apClampedTmpl<T1> dst = div2<apClampedTmpl<T1>, apClampedTmpl<T1>, apClampedTmpl<T2> >(s1, s2);
  return dst;
}

template <class T1, class T2>
apClampedTmpl<T1>& operator+=(apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  s1.val = apLimit<T1>(s1.val + s2.val);
  return s1;
}

template <class T1, class T2>
apClampedTmpl<T1>& operator-=(apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  s1.val = apLimit<T1>(s1.val - s2.val);
  return s1;
}

template <class T1, class T2>
apClampedTmpl<T1>& operator*= (apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  s1.val = apLimit<T1>(s1.val * s2.val);
  return s1;
}

template <class T1, class T2>
apClampedTmpl<T1>& operator/= (apClampedTmpl<T1>& s1, const apClampedTmpl<T2>& s2)
{
  s1.val = apLimit<T1>(s1.val, s2.val);
  return s1;
}





template <class T>
class apRGBTmpl
{
public :
  T red;
  T green;
  T blue;

  apRGBTmpl ()             : red(0), green(0), blue(0) { }
  explicit apRGBTmpl(T v)  : red(v), green(v), blue(v) { }
  apRGBTmpl (T r, T g, T b): red(r), green(g), blue(b) { }

  T ts1 () const { return red; }
  T ts2 () const { return green; }
  T ts3 () const { return blue; }

  apRGBTmpl (const apRGBTmpl& s) {
    red = s.red;
    green = s.green;
    blue = s.blue;
  }

  apRGBTmpl& operator=(const apRGBTmpl& src) {
    red = src.red;
    green = src.green;
    blue = src.blue;
    return *this;
  }

  template <class T1> apRGBTmpl (const apRGBTmpl<T1>& s) {
    red   = apLimit<T>(s.red);
    green = apLimit<T>(s.green);
    blue  = apLimit<T>(s.blue);
  }

  template <class T1> apRGBTmpl<T>& operator= (const apRGBTmpl<T1>& src) {
    //apRGBTmpl<T> <T>不加上应该也没问题的
    red   = apLimit<T>(src.red);
    green = apLimit<T>(src.green);
    blue  = apLimit<T>(src.blue);
    return *this;
  }

  apRGBTmpl& operator=(const T& c) {
    red   = c;
    green = c;
    blue  = c;
    return *this;
  }

  operator T() const {
    double v = 0.229 * red + 0.587*green + 0.114*blue;
    return static_cast<T>(v);
  }

  apRGBTmpl<T>& operator+= (const apRGBTmpl<T>& s) {
    red   = apLimit<T>( red + s.red);
    green = apLimit<T>( green + s.green);
    blue  = apLimit<T>( blue + s. blue);
    return *this;
  }

  apRGBTmpl<T>& operator-= (const apRGBTmpl<T>& s) {
    red   = apLimit<T>( red - s.red);
    green = apLimit<T>( green - s. green);
    blue  = apLimit<T>( blue - s.blue);
    return *this;
  }

  apRGBTmpl<T>& operator*= (const apRGBTmpl<T>& s) {
    red   = apLimit<T>( red * s.red);
    green = apLimit<T>( green * s.green);
    blue  = apLimit<T>( blue * s.blue);
    return *this;
  }

  apRGBTmpl<T>& operator/= (const apRGBTmpl<T>& s) {
    red   = apLimit<T>( red / s.red);
    green = apLimit<T>( green / s.green);
    blue  = apLimit<T>( blue / s.blue);
    return *this;
  }

  apRGBTmpl<T>& operator+= (const T& s) {
    red   = apLimit<T>(red + s);
    green = apLimit<T>(green + s);
    blue  = apLimit<T>(blue + s);
    return *this;
  }

  apRGBTmpl<T>& operator-= (const T& s) {
    red   = apLimit<T>(red - s);
    green = apLimit<T>(green - s);
    blue  = apLimit<T>(blue - s);
    return *this;
  }

  apRGBTmpl<T>& operator*= (const T& s) {
    red   = apLimit<T>(red * s);
    green = apLimit<T>(green * s);
    blue  = apLimit<T>(blue * s);
    return *this;
  }

  apRGBTmpl<T>& operator/= (const T& s) {
    red   = apLimit<T>(red / s);
    green = apLimit<T>(green / s);
    blue  = apLimit<T>(blue / s);
    return *this;
  }
};


typedef apRGBTmpl<Pel8>   apRGB;
typedef apRGBTmpl<Pel32>  apRGBPel32;
typedef apRGBTmpl<Pel32s> apRGBPel32s;

template <class T>
bool operator== (const apRGBTmpl<T>& s1, const apRGBTmpl<T>& s2)
{
  return (s1.red == s2.red) && (s1.green == s2.green) && (s1.blue == s2.blue);
}

template <class T>
bool operator!= (const apRGBTmpl<T>& s1, const apRGBTmpl<T>& s2)
{
  return (s1.red != s2.red) || (s1.green != s2.green) || (s1.blue != s2.blue);
}

template <class T1, class T2>
apRGBTmpl<T1> operator+ (const apRGBTmpl<T1>& s1, const T2& s2)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(s1.red + s2);
  dst.green = apLimit<T1>(s1.green+- s2);
  dst.blue  = apLimit<T1>(s1.blue + s2);
  return dst;
}

template <class T1, class T2>
apRGBTmpl<T1> operator- (const apRGBTmpl<T1>& s1, const T2& s2)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(s1.red - s2);
  dst.green = apLimit<T1>(s1.green - s2);
  dst.blue  = apLimit<T1>(s1.blue - s2);
  return dst;
}

template <class T1, class T2>
apRGBTmpl<T1> operator* (const apRGBTmpl<T1>& s1, const T2& s2)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(s1.red * s2);
  dst.green = apLimit<T1>(s1.green * s2);
  dst.blue  = apLimit<T1>(s1.blue * s2);
  return dst;
}

template <class T1, class T2>
apRGBTmpl<T1> operator/ (const apRGBTmpl<T1>& s1, const T2& s2)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(s1.red / s2);
  dst.green = apLimit<T1>(s1.green / s2);
  dst.blue  = apLimit<T1>(s1.blue / s2);
  return dst;
}

template <class T1>
apRGBTmpl<T1> operator+ (const apRGBTmpl<T1>& s1, const double& d)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(d + s1.red);
  dst.green = apLimit<T1>(d + s1.green);
  dst.blue  = apLimit<T1>(d + s1.blue);
  return dst;
}

template <class T1>
apRGBTmpl<T1> operator- (const apRGBTmpl<T1>& s1, const double& d)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(d - s1.red);
  dst.green = apLimit<T1>(d - s1.green);
  dst.blue  = apLimit<T1>(d - s1.blue);
  return dst;
}

template <class T1>
apRGBTmpl<T1> operator* (const apRGBTmpl<T1>& s1, const double& d)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(d * s1.red);
  dst.green = apLimit<T1>(d * s1.green);
  dst.blue  = apLimit<T1>(d * s1.blue);
  return dst;
}

template <class T1>
apRGBTmpl<T1> operator/ (const apRGBTmpl<T1>& s1, const double& d)
{
  apRGBTmpl<T1> dst;
  dst.red   = apLimit<T1>(d / s1.red);
  dst.green = apLimit<T1>(d / s1.green);
  dst.blue  = apLimit<T1>(d / s1.blue);
  return dst;
}

template <class T>
std::ostream& operator<< (std::ostream& os, const apRGBTmpl<T>& rgb)
{
  os << "(" << rgb.red << "," << rgb.green << "," << rgb.blue << ")";
  return os;
}

template <> std::ostream& operator<< (std::ostream& os, const apRGBTmpl<Pel8>& rgb);


template <> apRGB       apLimit<apRGB,  apRGBPel32s>(const apRGBPel32s& src);
template <> apRGB       apLimit<apRGB,  apRGB>      (const apRGB& src);
template <> apRGBPel32s apLimit<apRGBPel32s, apRGBPel32s>(const apRGBPel32s& src);

template <class D, class S1, class S2>
apRGBTmpl<D> apMin(const apRGBTmpl<S1>& s1, const apRGBTmpl<S2>& s2)
{
  apRGBTmpl<D> d;

  d.red   = apLimit<D>(apMin(s1.red, s2.red));
  d.green = apLimit<D>(apMin(s1.green, s2.green));
  d.blue  = apLimit<D>(apMin(s1.blud, s2.blue));
  return d;
}

template <class D, class S1, class S2>
apRGBTmpl<D> add2 (const apRGBTmpl<S1>& s1, const apRGBTmpl<S2>& s2)
{
  apRGBTmpl<D> d;
  d.red   = apLimit<D>( s1.red + s2.red);
  d.green = apLimit<D>( s1.green + s2.green);
  d.blue  = apLimit<D>(s1.blue, + s2.blue);
  return d;
}

template <class D, class S1,class S2>
apRGBTmpl<D> sub2 (const apRGBTmpl<S1>& s1, const apRGBTmpl<S2>& s2)
{
  apRGBTmpl<D> d;
  d.red   = apLimit<D>(s1.red - s2.red);
  d.green = apLimit<D>(s1.green - s2.green);
  d.blue  = apLimit<D>(s1.blue - s2.blue);
  return d;
}

template <class D, class S1, class S2>
apRGBTmpl<D> mul2 (const apRGBTmpl<S1>& s1, const apRGBTmpl<S2>& s2)
{
  apRGBTmpl<D> d;
  d.red   = apLimit<D>( s1.red * s2.red);
  d.green = apLimit<D>( s1.green * s2.green);
  d.blue  = apLimit<D>( s1.blue * s2.blue);
  return d;
}

template <class D, class S1, class S2>
apRGBTmpl<D> div2 (const apRGBTmpl<S2>& s1, const apRGBTmpl<S2>& s2)
{
  apRGBTmpl<D> d;
  d.red   = apLimit<D>( s1.red / s2.red);
  d.green = apLimit<D>( s1.green / s2.green);
  d.blue  = apLimit<D>( s1.blue / s2.blue);
  return d;
}

template <class D, class S1>
apRGBTmpl<D> scale (const apRGBTmpl<S1>& s1, float scaling)
{
  apRGBTmpl<D> d;
  d.red   = apLimit<D>( scaling * static_cast<float>(s1.red));
  d.green = apLimit<D>( scaling * static_cast<float>(s1.green));
  d.blue  = apLimit<D>( scaling * static_cast<float>(s1.blue));
  return d;
}

template <class T1, class T2>
apRGBTmpl<T1> operator+ (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  apRGBTmpl<T1> d;
  d.red   = apLimit<T1>( s1.red + s2.red);
  d.green = apLimit<T1>( s1.green + s2.green);
  d.blue  = apLimit<T1>( s1.blue + s2.blue);
  return d;
}

template <class T1, class T2>
apRGBTmpl<T1> operator- (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  apRGBTmpl<T1> d;
  d.red   = apLimit<T1>(s1.red - s2.red);
  d.green = apLimit<T1>(s1.green - s2.green);
  d.blue  = apLimit<T1>(s1.blue - s2.blue);
  return d;
}

template <class T1, class T2>
apRGBTmpl<T1> operator* (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  apRGBTmpl<T1> d;
  d.red   = apLimit<T1>(s1.red * s2.red);
  d.green = apLimit<T1>(s1.green * s2.green);
  d.blue  = apLimit<T1>(s1.blue * s2.blue);
  return d;
}

template <class T1, class T2>
apRGBTmpl<T1> operator/ (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  apRGBTmpl<T1> d;
  d.red   = apLimit<T1>(s1.red / s2.red);
  d.green = apLimit<T1>(s1.green / s2.green);
  d.blue  = apLimit<T1>(s1.blue / s2.blue);
  return d;
}

template <class T1, class T2>
apRGBTmpl<T1>& operator+= (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  s1.red   = apLimit<T1>(s1.red + s2.red);
  s1.green = apLimit<T1>(s1.green + s2.green);
  s1.blue  = apLimit<T1>(s1.blue + s2.blue);
  return s1;
}

template <class T1, class T2>
apRGBTmpl<T1>& operator-= (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  s1.red   = apLimit<T1>(s1.red + s2.red);
  s1.green = apLimit<T1>(s1.green + s2.green);
  s1.blue  = apLimit<T1>( s1.blue + s2.blue);
  return s1;
}

template <class T1, class T2>
apRGBTmpl<T1>& operator*= (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  s1.red   = apLimit<T1>(s1.red * s2.red);
  s1.green = apLimit<T1>(s1.green * s2.green);
  s1.blue  = apLimit<T1>(s1.blue * s2.blue);
  return s1;
}

template <class T1, class T2>
apRGBTmpl<T1>& operator/= (const apRGBTmpl<T1>& s1, const apRGBTmpl<T2>& s2)
{
  s1.red   = apLimit<T1>(s1.red / s2.red);
  s1.greeb = apLimit<T1>(s1.green / s2.green);
  s1.blue  = apLimit<T1>(s1.blue / s2.blue);
  return s1;
}


#endif
