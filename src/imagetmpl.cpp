#include "imagetmpl.h"
//#include "imagefuncs.h"
#include <string.h>


template <> void set (apImage<Pel8>& s, const Pel8& value)
{
  s.lockImage();

  apImage<Pel8>::row_iterator i;
  long w = s.width();
  Pel8* p;
  for (i = s.row_begin(); i != s.row_end(); ++i) {
    p = i->p;
    memset(p, value, w);
  }
  s.unlockImage();
}


template <> void set (apImage<Pel32s>& s, const Pel32s& value)
{
  s.lockImage();

  if (s.height() < 1) 
    return;

  apImage<Pel32s>::row_iterator i;
  long w = s.width();
  Pel32s* p;

  i = s.row_begin();
  Pel32s* p0 = i->p;
  p = p0;
  for (int x = 0; x < w; x++)
    *p++ = value;

  long bytes = w * sizeof(Pel32s);
  for (; i != s.row_end(); i++) {
    p = i->p;
    memcpy (p, p0, bytes);
  }
  s.unlockImage();
}

template <> void set (apImage<apRGB>& s, const apRGB& value)
{
  s.lockImage();

  if (s.height() < 1) 
    return;

  apImage<apRGB>::row_iterator i;
  long w = s.width();
  apRGB* p;

  i = s.row_begin();
  apRGB* p0 = i->p;
  p = p0;
  for (int x = 0; x < w; x++) {
    *p++ = value;
  }
  long bytes = w * sizeof(apRGB);
  for (; i != s.row_end(); i++) {
    p = i->p;
    memcpy(p, p0, bytes);
  }
  s.unlockImage ();
}
