#ifndef _jpeg_h_
#define _jpeg_h_


#include "io.h"

class apJPEG : public apImageIOBase
{
public :
  static apJPEG& gOnly ();
  
  virtual apDelegateInfo info (const std::string& filename); 

  virtual apImage<apRGB> readRGB  (const std::string& filename);
  virtual apImage<Pel8>  readPel8 (const std::string& filename);

  virtual bool write (const std::string& filename, const apRectImageStorage& pixel, const apDelegateParams& params = sNoParams);

private :
  static apJPEG* sOnly_;

  apJPEG ();
  ~apJPEG();
};



class apJPEG_installer
{
public :
  static apJPEG_installer installer;
private :
  apJPEG_installer();
};

#endif
