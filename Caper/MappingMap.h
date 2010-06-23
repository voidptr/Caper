#include "Line.h"
#include "IndexAndOffset.h"

class MappingMap : public map<string, vector<IndexAndOffset> >
{
private:

  bool mInsertionWasInSortedOrder;
  bool mMapIsFinalized;
  bool mMapWasRearrangedBySort;

  string mLastContig;
  long long mLastIndex;

  class SortMappings
  {
    MappingMap * mMappingMap;
  public:
    explicit SortMappings( MappingMap * pf ) : mMappingMap(pf) {}
    bool operator() (IndexAndOffset aLeft, IndexAndOffset aRight ) 
    {
      return aLeft.Index < aRight.Index;
    }
  };

public:

  MappingMap(void)
  {
    mInsertionWasInSortedOrder = true; // assume it came in sorted.
    mMapIsFinalized = false; // hasn't been finalized.
    mMapWasRearrangedBySort = false; // hasn't been done.

    mLastContig = "";
    mLastIndex = -1;
  }

  bool InsertionWasInSortedOrder()
  {
    return mInsertionWasInSortedOrder;
  }

  bool MapIsFinalized()
  {
    return mMapIsFinalized;
  }

  bool MapWasRearrangedBySort()
  {
    return mMapWasRearrangedBySort;
  }

  void AddLine( string aContig, long long aIndex, long long aPosition, long long aLength )
  {
    if ( mMapIsFinalized )
      throw string("No new lines may be added. Mapping has been finalized");

    IndexAndOffset lIndexAndOffset;
    lIndexAndOffset.Index = aIndex;
    lIndexAndOffset.Offset = aPosition;
    lIndexAndOffset.Length = aLength;

    //if ( find( aContig ) == end() )
    //{
    //  insert( pair<string, vector<IndexAndOffset> >( aContig, vector<IndexAndOffset>() ) );
    //}


    // Will this work? (TODO)
    (*this)[ aContig ].push_back( lIndexAndOffset );

//    find( aContig )->second.push_back( lIndexAndOffset );

    ReevaluateSort( aContig, aIndex );
  }

  void Finalize()
  {
    if ( !mInsertionWasInSortedOrder )
    {
      for ( MappingMap::iterator i = begin(); i != end(); ++i )
      {
        sort( i->second.begin(), i->second.end(), SortMappings(this) );
      }
      mMapWasRearrangedBySort = true;
    }
    mMapIsFinalized = true; // the map load has finished
  }

private:
  void ReevaluateSort( string aContig, long long aIndex )
  {
    if ( InsertionWasInSortedOrder() && mLastContig.compare( aContig ) > 0 && mLastIndex > aIndex )
      mInsertionWasInSortedOrder = false;
    
    mLastContig = aContig;
    mLastIndex = aIndex;    
  }
};


