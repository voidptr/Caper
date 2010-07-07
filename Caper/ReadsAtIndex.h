#pragma once

#include "stdafx.h"
#include "Mapping.h"

class ReadsAtIndex :
  public vector<Mapping*>
{
public:
  int ReferenceCount;

  ReadsAtIndex(void)
  {
    ReferenceCount = 1;
  }

  // there are a few scenarios for destruction.
  // 1. Get rid of the pointer and the data. Easy. Just call delete, and destructor will be called. If it's not in use, kapow!
  // 2. Get rid of the pointer but not the data. Just call delete, and the destructor will be called, which checks whether it's still in use.
  // 3. Decrement the use, but don't deallocate the pointer. For that, call Destroy.

  void Destroy()
  {
     cout << "~~ReadsAtIndex - Destroy:" << ReferenceCount << endl;
     if ( ReferenceCount == 1 ) // call the destructor to wipe us all out.
     {
       cout << "~~ReadsAtIndex - Destroy - Doing It: " << ReferenceCount << endl;
       delete this; // woo! It's all over. Kill the pointer too.
     }
     else if (ReferenceCount > 1 ) // do the decrement
     {
        cout << "~~ReadsAtIndex - Destroy - Decrementing:" << ReferenceCount << " to " << ReferenceCount -1 << endl;
       --ReferenceCount; // just decrement us, but don't delete the pointer itself. That would suck for other people who have to use the pointer.
     }
     else
     {
       cout << "~~ReadsAtIndex - Destroy - Wut?" << endl;
     }
     cout << "~~ReadsAtIndex - Destroy - Done" << endl;
  }

  // this destructor is iffy.
  ~ReadsAtIndex(void) // not only does this deallocate the internal content, it deletes the pointer, which may not be what we want.
  {
    cout << "~~ReadsAtIndex - Destructor" << endl;
    --ReferenceCount;
    cout << "~~ReadsAtIndex - Destructor - ReferenceCount: " << ReferenceCount << endl;
    if ( ReferenceCount == 0 ) // we dont' want to do this again if we already did it
    {
        cout << "~~ReadsAtIndex - Destructor - REALLY DOING IT THIS TIME" << endl;
        for ( ReadsAtIndex::iterator lIt = this->begin(); lIt != this->end(); ++lIt )
       {
         delete *lIt;
       }
    }
    else
    {
        cout << "~~GAH!" << endl;
    }
  }
};

