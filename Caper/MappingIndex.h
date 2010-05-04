#pragma once

struct MappingWindowBlockInfo
{
  MappingWindowBlockInfo(
    //MappingMap::mapped_type::reference aStartingLineReference,
    long long aSortedContigStartingLineNumber, long long aBlockOffset)// : StartingLineReference( aStartingLineReference )
  {
    NumberOfLines = -1;
    BlockOffset = aBlockOffset;
    BlockSizeInBytes = -1;
    CompressedBlockSizeInBytes = -1;
    SortedContigStartingLineNumber = aSortedContigStartingLineNumber;

  }

  //MappingMap::mapped_type::reference StartingLineReference;
  long long NumberOfLines;
  long long SortedContigStartingLineNumber;
  long long BlockOffset;
  long long BlockSizeInBytes;
  long long CompressedBlockSizeInBytes;
};

class MappingIndex :
  public map<string, map<long long, MappingWindowBlockInfo> >
{
private:
  MappingIndex::iterator lCurrentContig;
  bool lFirstTime;

public:
  MappingIndex(void)
  {
    //lCurrentContig = NULL;
    lFirstTime = true;
  }

  void AddEntry( string aContig,
    long long aWindowNumber,
    long long aSortedContigStartingLineNumber,
    long long aBlockOffset)
  {

    if ( lFirstTime || aContig != lCurrentContig->first )
    {
      if ( find( aContig ) == end() )
        insert( MappingIndex::value_type( aContig, MappingIndex::mapped_type() ) );

      lCurrentContig = find(aContig);
      lFirstTime = false;
    }

    MappingWindowBlockInfo lInfo( aSortedContigStartingLineNumber, aBlockOffset );

    lCurrentContig->second.insert( MappingIndex::mapped_type::value_type( aWindowNumber, lInfo ) );
  }


  ~MappingIndex(void)
  {
  }
};
