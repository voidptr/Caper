#pragma once

// Represents the info for a given read window.
struct MappingWindowBlockInfo
{
  MappingWindowBlockInfo() {}

  MappingWindowBlockInfo(
    long long aSortedContigStartingLineNumber,
    long long aBlockOffset,
    long long aNumberOfLines=-1,
    long long aBlockSizeInBytes=-1,
    long long aCompressedBlockSizeInBytes=-1)
  {
    SortedContigStartingLineNumber = aSortedContigStartingLineNumber;
    BlockOffset = aBlockOffset;

    // entered when rebuilding from index file.
    NumberOfLines = aNumberOfLines;
    BlockSizeInBytes = aBlockSizeInBytes;
    CompressedBlockSizeInBytes = aCompressedBlockSizeInBytes;
  }

  long long NumberOfLines;
  long long SortedContigStartingLineNumber;
  long long BlockOffset;
  long long BlockSizeInBytes;
  long long CompressedBlockSizeInBytes;
};

// Hash for all the contigs, and the read windows within them, mapped by the window number.
class MappingIndex :
  public map<string, map<long long, MappingWindowBlockInfo> >
{
private:
  MappingIndex::iterator lCurrentContig;
  bool lFirstTime;

  bool mIsBundle;
  long long mMappingsStartOffset;

public:
  MappingIndex(bool aIsBundle=false, long long aMappingsStartOffset=0)
  {
    //lCurrentContig = NULL;
    lFirstTime = true;
    mIsBundle = aIsBundle;
    mMappingsStartOffset = aMappingsStartOffset;
  }

  bool IsBundle()
  {
    return mIsBundle;
  }

  long long GetMappingStartOffset()
  {
    return mMappingsStartOffset;
  }

  void AddEntry( string aContig,
    long long aWindowNumber,
    long long aSortedContigStartingLineNumber,
    long long aBlockOffset,    
    long long aNumberOfLines=-1,
    long long aBlockSizeInBytes=-1,
    long long aCompressedBlockSizeInBytes=-1)
  {
    if ( lFirstTime || aContig != lCurrentContig->first )
    {
      if ( find( aContig ) == end() )
        insert( MappingIndex::value_type( aContig, MappingIndex::mapped_type() ) );

      lCurrentContig = find(aContig);

      //lCurrentContig = this[aContig];
      lFirstTime = false;
    }

    MappingWindowBlockInfo lInfo( aSortedContigStartingLineNumber, aBlockOffset, aNumberOfLines, aBlockSizeInBytes, aCompressedBlockSizeInBytes );

    lCurrentContig->second.insert( MappingIndex::mapped_type::value_type( aWindowNumber, lInfo ) );
  }

};
