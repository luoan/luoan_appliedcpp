#include "jpeg.h"

#include <stdio.h>
#include <iostream>

// 已经不需要了，因为jpeg-7c已经把这个添加到jpeglib.h文件里面去了
// 需要，不然报错，会报错
extern "C" {
#include "jpeglib.h"
}

//那些处理错误的我没有写，到时在说把
//那些是一定要的，因为设置jerror时候要用到
void local_error_exit (jpeg_common_struct* cinfo)
{
  char msg[1024];
  sprintf (msg, "error_exit: %d", cinfo->err->msg_code);

  //throw apJPEGException (msg);
}

void local_emit_message (jpeg_common_struct*, int)
{
  return;
}

void local_output_message (jpeg_common_struct* )
{
  return;
}



apJPEG* apJPEG::sOnly_ = 0;

apJPEG& apJPEG::gOnly()
{
  if (!sOnly_)
    sOnly_ = new apJPEG;
  return *sOnly_;
}

apJPEG::apJPEG () { }
apJPEG::~apJPEG() { }

apDelegateInfo apJPEG::info (const std::string& filename)
{
  apDelegateInfo info;
  info.isDelegate   = false;
  info.boundary     = apRect();
  info.bytesPerPixel= 0;
  info.planes       = 0;

  struct jpeg_decompress_struct cinfo;
  cinfo.output_width = cinfo.output_height = 0;

  FILE* infile;
  if ((infile = fopen(filename.c_str(), "rb")) == 0)
    return info;

  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit     = local_error_exit;
  jerr.emit_message   = local_emit_message;
  jerr.output_message = local_output_message;

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);

  try {
    jpeg_read_header(&cinfo, TRUE);

    if (cinfo.out_color_space == JCS_GRAYSCALE ||
        cinfo.out_color_space == JCS_RGB) {
      
      info.isDelegate = true;
      info.boundary   = apRect (0, 0, cinfo.image_width, cinfo.image_height);
      if (cinfo.out_color_space == JCS_GRAYSCALE)
        info.planes = 1;
      else 
        info.planes = 3;
      info.bytesPerPixel = info.planes;
    }
  }
  
  catch (std::domain_error s) {
  
  }
  //bytePerPixel大小是三？这样可以么？RGB只有三？
  //可能是为了指示是RGB吧，所以是三，实际大小不一定是三
  
  jpeg_destroy_decompress (&cinfo);
  fclose (infile);
  return info;
}



apImage<Pel8> apJPEG::readPel8 (const std::string& filename)
{
  /*apImage<Pel8> gray;
  apImage<apRGB> rgb = readRGB(filename);
  copy (rgb, gray);
  return gray;
  */
  return readRGB (filename);
}

/*
template <class T>
void showrgb(const apImage<T>& rgb)
{
  typename apImage<T>::row_iterator i;
  for (i = rgb.row_begin(); i != rgb.row_end(); ++i) {
    T* p = i->p;
    for (unsigned int x = 0; x != rgb.width(); ++x)
      std::cout << *p++;
    std::cout << std::endl;
  }
}
*/


apImage<apRGB> apJPEG::readRGB (const std::string& filename)
{
  apImage<apRGB> rgbImage;
  //std::cout << "readRGB 1\n";
  struct jpeg_decompress_struct cinfo;
  cinfo.output_width = cinfo.output_height = 0;

  FILE* infile;
  if ((infile = fopen(filename.c_str(), "rb")) == 0) {
    return rgbImage;
  }
  //std::cout << "readRGB 2\n";
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit     = local_error_exit;
  jerr.emit_message   = local_emit_message;
  jerr.output_message = local_output_message;

  jpeg_create_decompress (&cinfo);
  jpeg_stdio_src(&cinfo, infile);

  try {
    //std::cout << "readRGB 3\n";
    jpeg_read_header(&cinfo, TRUE);
    if (cinfo.out_color_space == JCS_UNKNOWN)
      return apImage<apRGB>::sNull;

    jpeg_start_decompress(&cinfo);
    
    if (cinfo.output_width == 0) {
      jpeg_destroy_decompress (&cinfo);
      fclose(infile);
      return rgbImage;
    }
    //std::cout << "readRGB 4\n";
    //std::cout << "readRGB 4.1 output_width:" << cinfo.output_width 
    //  << " output_height:" << cinfo.output_height << "\n";
    
    apRect boundary (0, 0, cinfo.output_width, cinfo.output_height);
    //std::cout << boundary << std::endl;
    rgbImage = apImage<apRGB> (boundary);
    //std::cout << "rgbImage width height: " << rgbImage.width()
      //<< " " << rgbImage.height() << std::endl;
   // showrgb(rgbImage);
    Pel8* buffer[1];
    long y = 0;
    //std::cout << "readRGB 5\n";
    switch (cinfo.out_color_space) {
      case JCS_GRAYSCALE :
        buffer[0] = new Pel8 [cinfo.output_width];
        while (cinfo.output_scanline < cinfo.output_height) {
          //std::cout << "readRGB 6\n";
          apRGB* dst = rgbImage.rowAddress (cinfo.output_scanline);
          y += jpeg_read_scanlines (&cinfo, buffer, 1);
          Pel8* src = buffer[0];
          for (unsigned int x = 0; x < cinfo.output_width; x++)
            *dst++ = *src++;
        }
        delete [] buffer[0];
        break;

      case JCS_RGB :
        //std::cout << "readRGB 7\n";
        while (cinfo.output_scanline < cinfo.output_height) {
          //std::cout << "readRGB 7.1\n";
          //std::cout << "readRGB 7.1.1 " << cinfo.output_scanline << std::endl;
          buffer[0] = reinterpret_cast<unsigned char*>(rgbImage.rowAddress(cinfo.output_scanline));
          //buffer[0] = new Pel8[cinfo.output_width];
          //std::cout << "readRGB 7.2\n";
          y += jpeg_read_scanlines (&cinfo, buffer, 1);
          //std::cout << "readRGB 8\n";
        }
        break;

      default :
        break;
    }
    //std::cout << "readRGB 9\n";
    jpeg_finish_decompress (&cinfo);
  }
  catch (std::domain_error e) {
  
  }

  jpeg_destroy_decompress (&cinfo);
  fclose (infile);
  return rgbImage;
}



bool apJPEG::write (const std::string& filename,
                    const apRectImageStorage& pixels,
                    const apDelegateParams& params)
{
  struct jpeg_compress_struct cinfo;

  FILE* outfile;
  if ((outfile = fopen(filename.c_str(), "wb")) == 0)
    return false;
  //std::cout << "writePixel 0 fopen\n";
  if (pixels.isNull()) {
    //std::cout << "writePixel 0.1 pixels isNull\n";
    fclose (outfile);
    return false;
  }
  //std::cout << "writeImage 1 \n";
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit     = local_error_exit;
  jerr.emit_message   = local_emit_message;
  jerr.output_message = local_output_message;
  //std::cout << "writeImage 2\n";
  jpeg_create_compress (&cinfo);
  jpeg_stdio_dest (&cinfo, outfile);
  //std::cout << "writeImage 3\n";
  if (pixels.bytesPerPixel() == 3) {
    //std::cout << "bytePerPixel == 3\n";
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
  }
  else if (pixels.bytesPerPixel() == 1) {
    cinfo.input_components = 1;
    cinfo.in_color_space = JCS_GRAYSCALE;
  }
  else {
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
  }
  
  //std::cout << "writeImage 4\n";
  cinfo.image_width  = pixels.width ();
  cinfo.image_height = pixels.height();
  //std::cout << "writeImage pixels width height: " 
  //  << pixels.width() << " " << pixels.height() << "\n";
  jpeg_set_defaults(&cinfo);

  int quality = static_cast<int>(params.quality);
  if (quality < 0)   quality = 50;
  if (quality > 100) quality = 100;
  jpeg_set_quality (&cinfo, quality, TRUE);
  //std::cout << "writeImage 5\n";
  jpeg_start_compress (&cinfo, TRUE);
  //std::cout << "writeImage 6 start_compress\n";
  const Pel8* buffer[1];
  long y0 = pixels.y0();
  while (cinfo.next_scanline < cinfo.image_height) {
    buffer[0] = pixels.rowAddress_(y0 + cinfo.next_scanline);
    jpeg_write_scanlines (&cinfo, const_cast<Pel8**>(buffer), 1);
  }

  jpeg_finish_compress (&cinfo);
  jpeg_destroy_compress(&cinfo);

  fclose(outfile);

  return false;
}




apJPEG_installer apJPEG_installer::installer;

apJPEG_installer::apJPEG_installer ()
{
  std::cout << "call jpeg_installer\n";
  apImageIODelegateList::gOnly().setDelegate("jpg", &apJPEG::gOnly());
}


