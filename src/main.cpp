#include "image.h"
#include "jpeg.h"

#include <iostream>
/*
void show(const apImage<Pel8>& image1)
{
  apImage<Pel8>::row_iterator i;
  for (i = image1.row_begin(); i != image1.row_end(); ++i) {
    Pel8* p = i->p;
    for (unsigned int x = 0; x != image1.width(); ++x)
      std::cout << static_cast<int>(*p++);
    std::cout << std::endl;
  }
}

template <class T>
void showrgb(const apImage<T>& rgb)
{
  typename apImage<T>::row_iterator i;
  for (i = rgb.row_begin(); i != rgb.row_end(); ++i) {
    T* p = i->p;
    for (unsigned int x = 0; x != rgb.width(); ++x) {
      std::cout << *p++;
    }
    std::cout << std::endl;
  }
}

void testrgb()
{
  char* cp = new char[200*900];
  cp[0] = 1;
  std::cout << "enter testrgb\n";
  apImage<apRGB> rgb(apRect(0, 0, 2000, 300));
  std::cout << "after apImage\n";
  //const apRGB* p = rgb.rowAddress(1);
  apAlloc<Pel8> alloc;
  alloc.isNull();
  std::cout << "after rgb address\n";


  rgb.setRow(1, apRGB(0, 1, 255));
  std::cout << "after setrow\n";
  rgb.setCol(1, apRGB(3, 3, 3));
  std::cout << "after setCol\n";
  rgb.setPixel(0, 0, apRGB(9, 9, 9));
  std::cout << "enter showrgb\n";
//  showrgb(rgb);
  if (rgb.isNull())
    std::cout <<  "rgb isNull\n";
  else
    std::cout << "rgb is not null\n";
  std::cout << "out showrgb\n";
}
*/

int main()
{
  //testrgb();

  /*
  apRect boundary (0, 0, 16, 16);
  apImage<Pel8> image1(boundary);
  image1.set(0);

  //Pel8 p = image1.getPixel(0, 1);
  //std::cout << (double)p << std::endl;
  apRect window (6, 6, 4, 4);
  apImage<Pel8> image2 = image1;
  image2.window(window);
  image2.set(1);

  show (image1);
  std::cout << std::endl << std::endl;
  Pel8* add = image1.rowAddress(3);
  *add = 3;
  //add = image1.rowAddress(2);
  image1.setRow(2, 5);
  show (image1);


*/


  apImageIOBase* delegate;

  //std::cout << "hehe\n";
  //apImageIODelegateList::gOnly().setDelegate("jpg", &apJPEG::gOnly());
  //加了这句话居然再这句话之前调用了installler，不加又会报错
  //应该是CMakeLists.txt代码写的哪里有错误，直接用g++就不需要，可以直接运行，
  //std::cout << "hehe\n";
  delegate = apImageIODelegateList::gOnly().getDelegate("jpg");

  if (delegate == 0) {
    std::cout << "delegate=0" << std::endl;
    return -1;
  }
  //std::cout << "before readRGB\n";
  //对于codeblock来说要放到工程目录下
  apImage<apRGB> rgb = delegate->readRGB("juren.jpg");
  //std::cout << "after readRGB\n";
  std::cout << rgb.width() << std::endl;
  if (rgb.isNull())
    std::cout << "rgb is Null\n";
  //showrgb(rgb);
  //rgb.set(apRGB(1, 1, 1));
  for (int i = 100; i != 150; i++)
    rgb.setRow(i, apRGB(1, 1, 1));
  for (int i = 900; i != 950; i++ )
    rgb.setCol(i, apRGB(1, 1, 1));
  delegate->write("set_juren.jpg", rgb);

  return 0;
}
//#include "thread.hpp"
/*
apImage<char> ap1(apRect(0, 0, 1, 1));
apImage<char> ap2(apRect(0, 0, 1, 1));
apImage<char> ap3(apRect(0, 0, 1, 1));

class thread1 : public apThread
{
  void thread () {
    ap1 = ap2;
  }
};

class thread2 : public apThread
{
  void thread () {
    ap3 = ap2;
  }
};

int main()
{
  thread1 thread1st;
  thread2 thread2cd;

  thread1st.start();
  thread2cd.start();

  thread1st.wait();
  thread2cd.wait();

  return 0;
}
*/

/*
int main()
{
  apImage<char> ap1;
  std::cout << std::endl;
  apImage<char> ap2 = ap1;
  return 0;
}*/
