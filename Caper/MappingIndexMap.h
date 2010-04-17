#pragma once

#include "MappingWindowBlockInfo.h"

class MappingIndexMap : public map<string, map<long long, MappingWindowBlockInfo> >
{
public:
  void AddEntry( string aContig, long long aWindow, long long aMappingFileMapVectorIndex )
  {
    if ( find( aContig ) == end() )
    {
      insert( pair<string, map<long long, MappingWindowBlockInfo> >( aContig, map<long long, MappingWindowBlockInfo>() ) );
    }

    MappingWindowBlockInfo lInfo;
    lInfo.StartingLineVectorIndex = aMappingFileMapVectorIndex;
    
    find( aContig )->second.insert( pair<long long, MappingWindowBlockInfo>( aWindow, lInfo ) );
  }

  void UpdateWithBlockSize( string aContig, long long aWindow, long long aOriginalBlockSize, long long aCompressedBlockSize )
  {
    map<long long, MappingWindowBlockInfo>::iterator lIt = find( aContig )->second.find( aWindow );

    lIt->second.CompressedBlockSizeInBytes = aCompressedBlockSize;
    lIt->second.BlockSizeInBytes = aOriginalBlockSize;
  }
};
