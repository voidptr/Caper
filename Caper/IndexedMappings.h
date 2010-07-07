#pragma once

#include "stdafx.h"
#include "Mapping.h"
#include "ReadsAtIndex.h"

class IndexedMappings :
  public map<long long, ReadsAtIndex *>
{
public:

  // this destructor is iffy.
  ~IndexedMappings(void) // not only does this deallocate the internal content, it deletes the pointer, which may not be what we want.
  {
    cout << "~~IndexedMappings - Destructor" << endl;
    for ( IndexedMappings::iterator lIt = this->begin(); lIt != this->end(); ++lIt )
    {
        cout << "~~IndexedMappings - Destructor - inside loop" << endl;
      lIt->second->Destroy();
              cout << "~~IndexedMappings - Destructor - done loop" << endl;
    }
    cout << "~~IndexedMappings - Destructor - Done" << endl;
  }
};

