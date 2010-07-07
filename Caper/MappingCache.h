#pragma once

#include "stdafx.h"
#include "Mapping.h"
#include "Typedefs.h"

class MappingCache
{
private:
	static const char Tab = '\t';

public:
  string ContigIdent;
  long long LeftIndex;
  long long RightIndex;

private:
  IndexedMappings mMappings;

  IndexedMappings::iterator mCachedIterator;

public:
  MappingCache( string & aContigIdent, long long & aLeftIndex, long long & aRightIndex )
  {
    cout << endl;
    cout << "~~MappingCache - Constructor: " << aContigIdent << aLeftIndex << " to " << aRightIndex << endl;
    ContigIdent = aContigIdent;
    LeftIndex = aLeftIndex;
    RightIndex = aRightIndex;

    mCachedIterator = mMappings.begin(); // set it to the init state.
  }

  ReadsAtIndex * GetReads( long long aIndex ) // this could possibly be empty.
  {
    cout << "~~MappingCache - GetReads" << endl;
    if ( mCachedIterator->first != aIndex )
      mCachedIterator = mMappings.find( aIndex );

    if ( mCachedIterator == mMappings.end() ) // empty! Likely we addressed an index that doesn't have an entry. This is fine.
      return new ReadsAtIndex();

    cout << "~~MappingCache - GetReads - Incrementing Reference Count for " << aIndex
        << " from " << mMappings[ aIndex ]->ReferenceCount << " to " << (mMappings[ aIndex ]->ReferenceCount)+1 << endl;
    mMappings[ aIndex ]->ReferenceCount++; // increment the reference count
    return mMappings[ aIndex ];
  }

  long long GetFirstIndex()
  {
    return mMappings.begin()->first;
  }

  long long GetNextIndex( long long aIndex )
  {
    // cached iterator is expired, or bogus.
    if ( mCachedIterator == mMappings.end() || mCachedIterator->first != aIndex ) // refresh, clean it up.
      mCachedIterator = mMappings.find( aIndex );

    // here's a trick. Since we might be in an unpopulated segment, we need to get our bearings
    // so, we'll create a fake entry to orient us, the move from here as normal.
    bool lFake = false; // flag to remember to delete the fake one.
    if ( mCachedIterator == mMappings.end() ) // there's not one where we are.
    {
      lFake = true;
      mMappings[ aIndex ]; // insert the fake.
      mCachedIterator = mMappings.find( aIndex );
    }

    // move up from where we are.
    ++mCachedIterator;

    if ( mCachedIterator != mMappings.end() ) // there's something here, holy shit!
    {
      if ( !lFake )
        return mCachedIterator->first;
      else // delete whatever fake one we created
      {
        long long lNextIndex = mCachedIterator->first;
        mMappings.erase( aIndex );
        return lNextIndex;
      }
    }
    else // next one, if there is one, is off the end of this cache
    {
      if ( lFake ) // erase our futile fake one.
        mMappings.erase( aIndex );

      return GetEndIndex(); // return our sad defeat
    }
  }

  // this method is effectively a fucking carbon copy of GetNextIndex, except for the -- in the very middle. FUUUUUU
  long long GetPreviousIndex( long long aIndex )
  {
    // cached iterator is expired, or bogus.
    if ( mCachedIterator == mMappings.end() || mCachedIterator->first != aIndex ) // refresh, clean it up.
      mCachedIterator = mMappings.find( aIndex );

    // here's a trick. Since we might be in an unpopulated segment, we need to get our bearings
    // so, we'll create a fake entry to orient us, the move from here as normal.
    bool lFake = false; // flag to remember to delete the fake one.
    if ( mCachedIterator == mMappings.end() ) // there's not one where we are.
    {
      lFake = true;
      mMappings[ aIndex ]; // insert the fake.
      mCachedIterator = mMappings.find( aIndex );
    }

    // move down from where we are.
    --mCachedIterator; // f'ing don't repeat yourself, goddamnit

    if ( mCachedIterator != mMappings.end() ) // there's something here, holy shit!
    {
      if ( !lFake )
        return mCachedIterator->first;
      else // delete whatever fake one we created
      {
        long long lNextIndex = mCachedIterator->first;
        mMappings.erase( aIndex );
        return lNextIndex;
      }
    }
    else // next one, if there is one, is off the end of this cache
    {
      if ( lFake ) // erase our futile fake one.
        mMappings.erase( aIndex );

      return GetEndIndex(); // return our sad defeat
    }

  }

  long long GetLastIndex()
  {
    return mMappings.rbegin()->first;
  }

  long long GetEndIndex()
  {
    return -1;
  }

  void Populate( vector<Mapping*> * aMappings )
  {
    for ( vector<Mapping*>::iterator lIt = aMappings->begin(); lIt != aMappings->end(); ++lIt )
	  {
		  AddMapping( *lIt );
	  }

    mCachedIterator = mMappings.begin(); // set the proper location
  }

  ~MappingCache()
  {
   // DestroyMappings();
    cout << "~~MappingCache - Destructor" << endl;   
  }

private:
  void AddMapping( Mapping * aMapping )
  { // I think this ought to work with the mappings.

    IndexedMappings::iterator lIndex = mMappings.insert( IndexedMappings::value_type( aMapping->Index, new ReadsAtIndex() ) ).first;
		lIndex->second->push_back( aMapping );
  }

  int PrivateIndex( long long aPublicIndex )
  {
    return (int) aPublicIndex - LeftIndex; // this is ok, since it'll always be < 1000. I could even do this with a short.
  }

  void DestroyMappings()
  {
    cout << endl;
    cout << "~~MappingCache - DestroyMappings" << endl;
    for ( IndexedMappings::iterator lIt = mMappings.begin(); lIt != mMappings.end(); ++lIt )
    {
        cout << "~~MappingCache - DestroyMappings - KILL" << endl;
      lIt->second->Destroy(); // if no one is using it, don't kill the entries.
      //delete lIt->second; // DELETE in destructor. Woo!
    }
    cout << "~~MappingCache - DONE KILLING " << endl;
    cout << endl;
  }
};

