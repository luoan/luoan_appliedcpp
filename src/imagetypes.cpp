#include "imagetypes.h"
#include <float.h>
#include <limits.h>


template <> apLimitInfo<Pel8> apLimitInfo<Pel8>::sType (0, UCHAR_MAX);
template <> apLimitInfo<Pel16> apLimitInfo<Pel16>::sType(0, USHRT_MAX);
template <> apLimitInfo<Pel32s> apLimitInfo<Pel32s>::sType(INT_MAX, INT_MAX);
template <> apLimitInfo<Pel32> apLimitInfo<Pel32>::sType(0, UINT_MAX);

template <> apLimitInfo<char> apLimitInfo<char>::sType(SCHAR_MIN, SCHAR_MAX);
template <> apLimitInfo<short> apLimitInfo<short>::sType(SHRT_MIN, SHRT_MAX);

template <> apLimitInfo<float> apLimitInfo<float>::sType(FLT_MIN, FLT_MAX);
template <> apLimitInfo<double> apLimitInfo<double>::sType(DBL_MIN, DBL_MAX);

template <> apLimitInfo<apClampedPel8> apLimitInfo<apClampedPel8>::sType(0, UCHAR_MAX);
template <> apLimitInfo<apClampedPel16> apLimitInfo<apClampedPel16>::sType(0, USHRT_MAX);
template <> apLimitInfo<apClampedPel32s> apLimitInfo<apClampedPel32s>::sType(INT_MIN, INT_MAX);

template <> apLimitInfo<apRGB> apLimitInfo<apRGB>::sType(apRGB(0), apRGB(UCHAR_MAX));
template <> apLimitInfo<apRGBPel32> apLimitInfo<apRGBPel32>::sType(apRGBPel32(0), apRGBPel32(UINT_MAX));
template <> apLimitInfo<apRGBPel32s> apLimitInfo<apRGBPel32s>::sType(apRGBPel32s(INT_MIN), apRGBPel32s(INT_MAX));

template <> Pel8 apLimit<Pel8, Pel32s> (const Pel32s& src)
{
  return static_cast<Pel8>(src < 0 ? 0 : (src > 255 ? 255 : src));
}

template <> Pel8 apLimit<Pel8, Pel8> (const Pel8& src)
{
  return src;
}

template <> Pel32s apLimit<Pel32s, Pel32s> (const Pel32s& src)
{
  return src;
}

template <> Pel32s apLimit<Pel32s, Pel32> (const Pel32& src)
{
  return (src > INT_MAX) ? INT_MAX : src;
}

template <> apClampedPel8 apLimit<apClampedPel8, apClampedPel32s> (const apClampedPel32s& src)
{
  return static_cast<Pel8>(src.val < 0 ? 0 : (src.val > 255 ? 255 : src.val));
}

template <> apClampedPel8 apLimit<apClampedPel8, apClampedPel8> (const apClampedPel8& src)
{
  return src;
}

template <> apClampedPel32s apLimit<apClampedPel32s, apClampedPel32s> (const apClampedPel32s& src)
{
  return src;
}

template <> apRGB apLimit<apRGB, apRGBPel32s> (const apRGBPel32s& src)
{
  apRGB dst;
  dst.red   = static_cast<Pel8>(src.red   < 0 ? 0 : (src.red   > 255 ? 255 : src.red));
  dst.green = static_cast<Pel8>(src.green < 0 ? 0 : (src.green > 255 ? 255 : src.green));
  dst.blue  = static_cast<Pel8>(src.blue  < 0 ? 0 : (src.blue  > 255 ? 255 : src.blue));
  return dst;
}

template <> apRGB apLimit<apRGB, apRGB> (const apRGB& src)
{
  return src;
}

template <> apRGBPel32s apLimit<apRGBPel32s, apRGBPel32s> (const apRGBPel32s& src)
{
  return src;
}




template <> 
std::ostream& operator<< (std::ostream& os, const apClampedTmpl<Pel8>& pel)
{
  os << (Pel32s)pel.val;
  return os;
}

template <>
std::ostream& operator<< (std::ostream& os, const apRGBTmpl<Pel8>& rgb)
{
  os << "(" << (Pel32s)rgb.red << "," << (Pel32s)rgb.green << "," << (Pel32s)rgb.blue << ")";
  return os;
}
