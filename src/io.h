#ifndef _io_h_
#define _io_h_


#include "image.h"

#include <string>
#include <map>
#include <iostream>

struct apDelegateInfo
{
  bool         isDelegate;//
  apRect       boundary;//图形边界
  unsigned int bytesPerPixel;
  unsigned int planes;//图像的层，比如RGB是三层
  std::string  format;
};


struct apDelegateParams
{
  float       quality;
  std::string params;

  apDelegateParams(float qual=90.) : quality(qual) { }
};


class apImageIOBase
{
public :
  virtual apDelegateInfo info (const std::string& filename) = 0;

  template <class T, class S>
  void read (const std::string& filename, apImage<T, S>& image)
  {
    if (typeid(apImage<apRGB>) == typeid(image))
      image = readRGB(filename);
    else if (typeid(apImage<Pel8>) == typeid(image))
      image = readPel8(filename);
    else 
      copy (readRGB(filename), image);//那就是会把apImage的apImageStorage部分传送给引用去给apImageStorage赋值,这个是作为apImage的一部分的，因为是引用，所以应该可以把，反正apImage是基于apImageStorage的api
  }

  template <class T, class S>
  void write (const std::string& filename, apImage<T, S>& image,
              const apDelegateParams& params = sNoParams)
  {
    if (typeid(apImage<apRGB>) == typeid(image))
      write (filename, image.storage(), params);
    else if (typeid(apImage<Pel8>) == typeid(image))
      write (filename, image.storage(), params);
    else {
      apImage<apRGB> rgb = image;
      write (filename, rgb.storage(), params);
    }
  }

  virtual apImage<apRGB> readRGB (const std::string& filename) = 0;
  virtual apImage<Pel8>  readPel8(const std::string& filename) = 0;

  virtual bool write (const std::string& filename,
                      const apRectImageStorage& pixels,
                      const apDelegateParams& params) = 0;
protected :
  apImageIOBase ();
  virtual ~apImageIOBase ();

  static apDelegateParams sNoParams;
};



class apImageIODelegateList
{
public :
  static apImageIODelegateList& gOnly();

  apImageIOBase* getDelegate (const std::string& type);
  void setDelegate (const std::string& type, apImageIOBase* object);

  apImageIOBase* findDelegate (const std::string& filename);

private :
  typedef std::map<std::string, apImageIOBase*> map;
  typedef map::iterator iterator;

  map map_;
  
  static apImageIODelegateList* sOnly_;

  apImageIODelegateList ();
};



#endif
