#include "Line.h"
#include "IndexAndOffset.h"

class MappingMap : public map<string, vector<IndexAndOffset> >
{
private:
  bool mIsSorted;

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
    mIsSorted = true; // assume it's sorted.

    mLastContig = "";
    mLastIndex = -1;
  }

  bool IsSorted()
  {
    return mIsSorted;
  }

  void AddLine( string aContig, long long aIndex, long long aPosition )
  {
    IndexAndOffset lIndexAndOffset;
    lIndexAndOffset.Index = aIndex;
    lIndexAndOffset.Offset = aPosition;

    if ( find( aContig ) == end() )
    {
      insert( pair<string, vector<IndexAndOffset> >( aContig, vector<IndexAndOffset>() ) );
    }

    find( aContig )->second.push_back( lIndexAndOffset );

    ReevaluateSort( aContig, aIndex );
  }

  void SortMap()
  {
    for ( MappingMap::iterator i = begin(); i != end(); ++i )
    {
      sort( i->second.begin(), i->second.end(), SortMappings(this) );
    }
    mIsSorted = true;
  }

private:
  void ReevaluateSort( string aContig, long long aIndex )
  {
    if ( IsSorted() && mLastContig.compare( aContig ) > 0 && mLastIndex > aIndex )
      mIsSorted = false;
    
    mLastContig = aContig;
    mLastIndex = aIndex;    
  }
};
