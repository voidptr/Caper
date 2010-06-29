#pragma once

#include "stdafx.h"
#include "MappingFileEngine.h"
#include "BundleFileEngine.h"
#include "MappingCache.h"
#include "MappingCacheFactory.h"

class MappingEngine
{
private:
  static const int IndexIncrement = 1000;

  MappingFileEngine * mFileEngine;
  MappingCacheFactory * mMappingCacheFactory;

  MappingCache * mCurrentCache;

public:
  class iterator
  {
  private:
    MappingEngine * mMappingEngine;
    long long mIndex;
    string mContigName;

  public:
    iterator( MappingEngine * aMappingEngine, 
      string & aContigName=string(""), long long aIndex=-1)
    {
      mMappingEngine = aMappingEngine;

      if ( aContigName == "" )
        mContigName = mMappingEngine->GetFirstContig();
      else
        mContigName = aContigName;

      if ( mIndex == -1 )
        mIndex = mMappingEngine->GetNextIndex( mContigName, mIndex );
      else
        mIndex = aIndex;
    }

    void Next()
    {
      mIndex = mMappingEngine->GetNextIndex( mContigName, mIndex );
    }

    void Previous()
    {
      mIndex = mMappingEngine->GetPreviousIndex( mContigName, mIndex );
    }

    void End()
    {
      mIndex = mMappingEngine->GetEndIndex();
    }

    ReadsAtIndex * GetReads()
    {
      return mMappingEngine->GetReads( mContigName, mIndex );
    }

    IndexedMappings * GetReadsIndexed()
    {
      return mMappingEngine->GetReadsIndexed( mContigName, mIndex );
    }

    long long GetIndex()
    {
      return mIndex;
    }

    bool operator==(const iterator & aOther ) const 
    {
      if ( mIndex == aOther.mIndex && mContigName == aOther.mContigName )
        return true;

      return false;
    }

    bool operator!=(const iterator & aOther) const 
    {
      return !(*this == aOther);
    }

    IndexedMappings * Intersect()
    {
      return mMappingEngine->GetIntersection( mContigName, mIndex, mIndex ); 
    }

  };

public:

  MappingEngine(string & aMappingFilePath, string & aMappingIndexPath )
  {
    mFileEngine = new MappingFileEngine( aMappingFilePath, aMappingIndexPath );
    mMappingCacheFactory = new MappingCacheFactory( mFileEngine );

    mCurrentCache = NULL;
  }

  MappingEngine(string & aBundlePath)
  {
    mFileEngine = new BundleFileEngine( aBundlePath );
    mMappingCacheFactory = new MappingCacheFactory( mFileEngine );

    mCurrentCache = NULL;
  }

  void Initialize()
  {
    mFileEngine->Initialize();
  }

  iterator Begin( string & aContigIdent=string() )
  {
    return iterator( this, aContigIdent );
  }

  iterator At( string & aContigIdent, long long aIndex ) // you can address one, but there may not be anything there.
  {
    return iterator( this, aContigIdent, aIndex );
  }

  iterator End( string & aContigIdent )
  {
    iterator lIt ( this, aContigIdent ); // does this get destroyed too soon? :/
    lIt.End();
    return lIt;
  }

  ReadsAtIndex * GetReads( string & aContigIdent, long long aIndex )
  {
    PopulateCorrectCache( aContigIdent, IndexToWindowNumber(aIndex) );
    return mCurrentCache->GetReads( aIndex );
  }

  IndexedMappings * GetReadsIndexed( string & aContigIdent, long long aIndex )
  {
    IndexedMappings * lIndexedReads = new IndexedMappings();
    
    lIndexedReads->insert( IndexedMappings::value_type( aIndex, GetReads( aContigIdent, aIndex ) ) );
    
    return lIndexedReads;
  }

  long long GetReadLength()
  {
    return mFileEngine->GetReadLength();
  }

  IndexedMappings * GetIntersection( string & aContigIdent, long long aLeft, long long aRight )
  {
    long long lIntersectLeft = aLeft - GetReadLength();
    if ( lIntersectLeft < 0 )
      lIntersectLeft = 0;

    IndexedMappings * lReads = new IndexedMappings();

    iterator lRight = At( aContigIdent, aRight ); // just a stake in the ground, probably nothing here.
    for ( iterator lIt = At( aContigIdent, lIntersectLeft ); lIt != End( aContigIdent ) && lIt.GetIndex() <= lRight.GetIndex(); lIt.Next() )
    {
      lReads->insert( IndexedMappings::value_type( lIt.GetIndex(), lIt.GetReads() ) );
    }
  
    return lReads;
  }

  ~MappingEngine(void)
  {
    delete mFileEngine;
    delete mMappingCacheFactory;
  }

private:
  
  long long GetNextIndex( string & aContig, long long aIndex )
  {
    PopulateCorrectCache( aContig, IndexToWindowNumber(aIndex) ); // init the current cache, if we haven't already.

    if ( mCurrentCache != NULL ) // Hooray! We're in a proper window!x`
    {
      // so, we're some place. No idea if it's a no-man's land.
      // so, let's go ahead and ask for it from the current cache.
      long long lNextIndex = mCurrentCache->GetNextIndex( aIndex ); // ask for the next index
      if ( lNextIndex != mCurrentCache->GetEndIndex() ) // hey, we found one in this window!
      {
        return lNextIndex;
      }
    }

    // so, we're either in a NULL window, or what we want isn't in the current window anyway.
    // either way, leave it to the NextWindow getter in the File Engine.
    long long lNextWindowNumber = mFileEngine->NextWindow( aContig, IndexToWindowNumber(aIndex) ); // figure out what the next cache up is, populate that, and then retrieve the first index.
    if ( lNextWindowNumber != mFileEngine->EndWindow( aContig ) ) // populate the next cache.
    {
      PopulateCorrectCache( aContig, lNextWindowNumber ); // populate the cache with the next window.
      return mCurrentCache->GetFirstIndex(); // yay! Here's the next index!
    }
    else // there is no next window. we're at the end of this contig.
    {
      return GetEndIndex( aContig );
    }
    
  }

  long long GetPreviousIndex( string & aContig, long long aIndex )
  {    
    PopulateCorrectCache( aContig, IndexToWindowNumber(aIndex) ); // init the current cache, if we haven't already.

    if ( mCurrentCache != NULL ) // Hooray! We're in a proper window!
    {
      // so, we're some place, rather than the void. No idea if we're in a no-man's land though.
      // so, let's go ahead and ask for the previous index from the current cache.
      long long lPreviousIndex = mCurrentCache->GetPreviousIndex( aIndex ); // ask for the previous index
      if ( lPreviousIndex != mCurrentCache->GetEndIndex() ) // hey, we found one in this window!
      {
        return lPreviousIndex;
      }
    }

    // still executing, so, we're either in a NULL window, or what we want isn't in the current window anyway.
    // either way, leave it to the PreviousWindow getter in the File Engine.
    long long lPreviousWindowNumber = mFileEngine->PreviousWindow( aContig, IndexToWindowNumber(aIndex) ); // figure out what the next cache up is, populate that, and then retrieve the first index.
    if ( lPreviousWindowNumber != mFileEngine->EndWindow( aContig ) ) // ok, we have a valid previous window
    {
      PopulateCorrectCache( aContig, lPreviousWindowNumber ); // populate the cache with the previous window.
      return mCurrentCache->GetLastIndex(); // yay! Here's the previous index!
    }
    else // there is no previous window. we're already at the front end of this contig.
    {
      return GetEndIndex( aContig );
    }
  }

  long long GetEndIndex( string & aContig=string() )
  {
    return -1;
  }

  string GetFirstContig()
  {
    return mFileEngine->FirstContig();
  }

  void PopulateCorrectCache( string & aContigIdent, long long aWindowNumber )
  { 
    if ( mCurrentCache == NULL || 
      mCurrentCache->ContigIdent != aContigIdent || 
      IndexToWindowNumber( mCurrentCache->LeftIndex ) != aWindowNumber )
    {
      DestroyCache();
      mCurrentCache = mMappingCacheFactory->BuildMappingCache( aContigIdent, aWindowNumber );
    }
  }

  long long IndexToWindowNumber(long long aIndex )
  {
    return ( aIndex / IndexIncrement );
  }

  void DestroyCache()
  {
    if ( mCurrentCache != NULL )
    {
      delete mCurrentCache;
      mCurrentCache = NULL;
    }
  }
};
