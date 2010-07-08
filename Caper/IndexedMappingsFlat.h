#pragma once

#include "stdafx.h"
#include "Mapping.h"
#include "ReadsAtIndex.h"

class IndexedMappingsFlat :
  public vector<ReadsAtIndex *>
{
public:

  // this destructor is iffy.
  ~IndexedMappingsFlat(void) // not only does this deallocate the internal content, it deletes the pointer, which may not be what we want.
  {
    //cout << "~~IndexedMappingsFlat - Destructor" << endl;
    for ( IndexedMappingsFlat::iterator lIt = this->begin(); lIt != this->end(); ++lIt )
    {
      //cout << "~~IndexedMappingsFlat - Destructor - inside loop" << endl;
      (*lIt)->Destroy();
      //cout << "~~IndexedMappingsFlat - Destructor - done loop" << endl;
    }
    //cout << "~~IndexedMappingsFlat - Destructor - Done" << endl;
  }
};
