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
  protected:
    MappingEngine * mMappingEngine;
    long long mIndex;
    string mContigName;

  public:
    iterator( MappingEngine * aMappingEngine,
      const string & aContigName="", long long aIndex=-1)
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
      cout << "~~MappingEngine::iterator - Next - CurrentIndex: " << mIndex << endl;
      mIndex = mMappingEngine->GetNextIndex( mContigName, mIndex );
      cout << "~~MappingEngine::iterator - Next - NextIndex: " << mIndex << endl;
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

    vector<ReadsAtIndex*> * IntersectFlat()
    {
        return mMappingEngine->GetIntersectionFlat( mContigName, mIndex, mIndex );
    }

  };

public:

  MappingEngine(string & aMappingFilePath, string & aMappingIndexPath )
  {
    mFileEngine = new MappingFileEngine( aMappingFilePath, aMappingIndexPath );
    mMappingCacheFactory = new MappingCacheFactory( mFileEngine );

    mCurrentCache = NULL;
  }

  MappingEngine(string aBundlePath)
  {
    mFileEngine = new BundleFileEngine( aBundlePath );
    mMappingCacheFactory = new MappingCacheFactory( mFileEngine );

    mCurrentCache = NULL;
  }

   MappingEngine(const char * aBundlePath)
   {
     string lBundlePath = string(aBundlePath);

     mFileEngine = new BundleFileEngine( lBundlePath );
     mMappingCacheFactory = new MappingCacheFactory( mFileEngine );

     mCurrentCache = NULL;
   }

  void Initialize()
  {
    mFileEngine->Initialize();
  }

  iterator Begin( const string & aContigIdent="" )
  {
    return iterator( this, aContigIdent );
  }

  iterator At( string & aContigIdent, long long aIndex ) // you can address one, but there may not be anything there.
  {
    return iterator( this, aContigIdent, aIndex );
  }

  iterator At( const char * aContigIdent, long long aIndex ) // you can address one, but there may not be anything there.
  {
    string lContigIdent = string(aContigIdent);
    return At( lContigIdent, aIndex );
  }

  iterator End( string & aContigIdent )
  {
    iterator lIt ( this, aContigIdent ); // does this get destroyed too soon? :/
    lIt.End();
    return lIt;
  }

  ReadsAtIndex * GetReads( string & aContigIdent, long long aIndex )
  {
    cout << "~~MappingEngine - GetReads" << aContigIdent << " index " << aIndex  << " window " << IndexToWindowNumber(aIndex) << endl;

    PopulateCorrectCache( aContigIdent, IndexToWindowNumber(aIndex) );
    cout << "~~MappingEngine - GetReads - Done Populating Correct Cache" << endl;
    return mCurrentCache->GetReads( aIndex );
  }

  ReadsAtIndex * GetReads( const char * aContigIdent, long long aIndex )
  {
    string lContigIdent = string(aContigIdent);
    return GetReads( lContigIdent, aIndex );
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
    bool lStart = true;
    cout << "**********~~MappingEngine - GetIntersection - outside the loop" << endl;
    for ( iterator lIt = At( aContigIdent, lIntersectLeft ); lIt != End( aContigIdent ) && lIt.GetIndex() <= lRight.GetIndex(); lIt.Next() )
    {
      cout << "**********~~MappingEngine - GetIntersection - inside loop" << endl;
      if (!lStart || lIt.GetReads()->size() > 0 ) // kludge to avoid pushing an empty set of reads.
          lReads->insert( IndexedMappings::value_type( lIt.GetIndex(), lIt.GetReads() ) );

      lStart = false;
    }
    cout << "**********~~MappingEngine - GetIntersection - end the loop" << endl;

    return lReads;
  }

  IndexedMappings * GetIntersection( const char * aContigIdent, long long aLeft, long long aRight )
  {
    string lContigIdent = string(aContigIdent);
    return GetIntersection( lContigIdent, aLeft, aRight );
  }

  vector<ReadsAtIndex*> * GetIntersectionFlat( string & aContigIdent, long long aLeft, long long aRight )
  {
    long long lIntersectLeft = aLeft - GetReadLength();
    if ( lIntersectLeft < 0 )
      lIntersectLeft = 0;

    vector<ReadsAtIndex*> * lReads = new vector<ReadsAtIndex*>();

    iterator lRight = At( aContigIdent, aRight ); // just a stake in the ground, probably nothing here.
    bool lStart = true;
    for ( iterator lIt = At( aContigIdent, lIntersectLeft ); lIt != End( aContigIdent ) && lIt.GetIndex() <= lRight.GetIndex(); lIt.Next() )
    {
      if ( !lStart || lIt.GetReads()->size() > 0 ) // kludge to avoid pushing an empty set of reads.
        lReads->push_back( lIt.GetReads() );

      lStart = true;
    }

    return lReads;

  }

  vector<ReadsAtIndex*> * GetIntersectionFlat( const char * aContigIdent, long long aLeft, long long aRight )
  {
    string lContigIdent = string(aContigIdent);
    return GetIntersectionFlat( lContigIdent, aLeft, aRight );
  }

  ~MappingEngine(void)
  {
    cout << "MappingEngine - Destructor" << endl;
    delete mFileEngine;
    delete mMappingCacheFactory;
    cout << "MappingEngine - Destructor - Done" << endl;
  }

private:

  long long GetNextIndex( string & aContig, long long aIndex )
  {
    cout << "~~MappingEngine - GetNextIndex" << endl;
    cout << "~~MappingEngine - GetNextIndex - Populating Correct Cache (1)" << endl;
    PopulateCorrectCache( aContig, IndexToWindowNumber(aIndex) ); // init the current cache, if we haven't already.
    cout << "~~MappingEngine - GetNextIndex - Done Populating Correct Cache (1)" << endl;
    
    if ( mCurrentCache != NULL ) // Hooray! We're in a proper window!x`
    {
      cout << "~~MappingEngine - GetNextIndex - (mCurrentCache != NULL)" << endl;
      // so, we're some place. No idea if it's a no-man's land.
      // so, let's go ahead and ask for it from the current cache.
      long long lNextIndex = mCurrentCache->GetNextIndex( aIndex ); // ask for the next index
      if ( lNextIndex != mCurrentCache->GetEndIndex() ) // hey, we found one in this window!
      {
        cout << "~~MappingEngine - GetNextIndex - (lNextIndex != mCurrentCache->GetEndIndex())" << endl;
        return lNextIndex;
      }
    }

    // so, we're either in a NULL window, or what we want isn't in the current window anyway.
    // either way, leave it to the NextWindow getter in the File Engine.
    long long lNextWindowNumber = mFileEngine->NextWindow( aContig, IndexToWindowNumber(aIndex) ); // figure out what the next cache up is, populate that, and then retrieve the first index.
    if ( lNextWindowNumber != mFileEngine->EndWindow( aContig ) ) // populate the next cache.
    {
            cout << "~~MappingEngine - GetNextIndex - Populating Correct Cache (2)" << endl;
      PopulateCorrectCache( aContig, lNextWindowNumber ); // populate the cache with the next window.
          cout << "~~MappingEngine - GetNextIndex - Done Populating Correct Cache (2)" << endl;

      return mCurrentCache->GetFirstIndex(); // yay! Here's the next index!
    }
    else // there is no next window. we're at the end of this contig.
    {
      cout << "~~MappingEngine - GetNextIndex - There is no next window" << endl;
      return GetEndIndex( aContig );
    }

  }

  long long GetPreviousIndex( string & aContig, long long aIndex )
  {
      cout << "~~MappingEngine - GetPreviousIndex" << endl;
    cout << "~~MappingEngine - GetPreviousIndex - Populating Correct Cache (1)" << endl;

    PopulateCorrectCache( aContig, IndexToWindowNumber(aIndex) ); // init the current cache, if we haven't already.
    cout << "~~MappingEngine - GetPreviousIndex - Done Populating Correct Cache (1)" << endl;

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
        cout << "~~MappingEngine - GetPreviousIndex - Populating Correct Cache (2)" << endl;

      PopulateCorrectCache( aContig, lPreviousWindowNumber ); // populate the cache with the previous window.
          cout << "~~MappingEngine - GetPreviousIndex - Populating Correct Cache (2)" << endl;

      return mCurrentCache->GetLastIndex(); // yay! Here's the previous index!
    }
    else // there is no previous window. we're already at the front end of this contig.
    {
      return GetEndIndex( aContig );
    }
  }

  long long GetEndIndex( const string & aContig="" )
  {
    return -1;
  }

  string GetFirstContig()
  {
    return mFileEngine->FirstContig();
  }

  void PopulateCorrectCache( string & aContigIdent, long long aWindowNumber )
  {
    cout << "~~MappingEngine - PopulateCorrectCache " << aWindowNumber << endl;
    if ( mCurrentCache == NULL ||
      mCurrentCache->ContigIdent != aContigIdent ||
      IndexToWindowNumber( mCurrentCache->LeftIndex ) != aWindowNumber )
    {
      cout << "~~MappingEngine - PopulateCorrectCache - Rebuilding (Destroying, Rebuild)..." << endl;
      DestroyCache();
      mCurrentCache = mMappingCacheFactory->BuildMappingCache( aContigIdent, aWindowNumber );
      cout << "~~MappingEngine - PopulateCorrectCache - Done Rebuilding" << endl;
    }
    else
    {
        cout << "~MappingEngine - PopulateCorrectCache - ALREADY AT CORRECT CACHE!" << endl;
    }
    cout << "~~MappingEngine - PopulateCorrectCache - Done" << endl;
  }

  long long IndexToWindowNumber(long long aIndex )
  {
    return ( aIndex / IndexIncrement );
  }

  void DestroyCache()
  {
    if ( mCurrentCache != NULL )
    {
        cout << "~~MappingEngine - DestroyCache" << endl;
        delete mCurrentCache;
        cout << "~~MappingEngine - DestroyCache - Done" << endl;
        mCurrentCache = NULL;
    }
  }
};
