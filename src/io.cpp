#include "io.h"

#include <sstream>

apImageIOBase::apImageIOBase () { }
apImageIOBase::~apImageIOBase() { }

apDelegateParams apImageIOBase::sNoParams;



apImageIODelegateList* apImageIODelegateList::sOnly_ = 0;

apImageIODelegateList& apImageIODelegateList::gOnly ()
{
  if (!sOnly_)
    sOnly_ = new apImageIODelegateList ();
  return *sOnly_;
}

apImageIODelegateList::apImageIODelegateList () { }

apImageIOBase* apImageIODelegateList::getDelegate (const std::string& type)
{
  return map_[type];
}

void apImageIODelegateList::setDelegate (const std::string& type, apImageIOBase* object)
{
  map_[type] = object;
}

apImageIOBase* apImageIODelegateList::findDelegate (const std::string& filename)
{
  iterator i;
  for (i = map_.begin(); i != map_.end(); ++i) {
    if (i->second->info(filename).isDelegate)
      return i->second;
  }
  return 0;
}
