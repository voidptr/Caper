#include "stdafx.h"
#include "MappingFileEngine.h"
#include "zlib.h"

class BundleFileEngine :
  public MappingFileEngine
{

private:
  long long mBundleStartOffset;

public:

  BundleFileEngine( string & aBundlePath ) : MappingFileEngine( aBundlePath, aBundlePath ) 
  {
    mBundleStartOffset = 0;
  }

  // override
  vector<Mapping*> * FetchMappings( string & aContig, long long aWindowNumber )
  {

    MappingIndex::iterator lContigIterator = mMappingIndex.find(aContig);
    if ( lContigIterator == mMappingIndex.end() )
      throw string( "Could not find requested contig." );

    MappingIndex::mapped_type::iterator lWindowIterator = lContigIterator->second.find(aWindowNumber); 

    if ( lWindowIterator == lContigIterator->second.end() )  // non-existent window!
      return new vector<Mapping *>();

    char * lBlock = ReadBlock( lWindowIterator->second.BlockOffset, lWindowIterator->second.CompressedBlockSizeInBytes );

    DecompressBlock( lBlock, lWindowIterator->second.CompressedBlockSizeInBytes, lWindowIterator->second.BlockSizeInBytes );
    
    vector<Mapping*> * lMappings = PrepareMappings( lBlock );
    delete [] lBlock;

    return lMappings;
  }

protected: // overrides
  void PopulateIndex()
  {
    ifstream lStream( mIndexFilePath.c_str(), ios::binary );
    if ( !lStream.is_open() )
      throw string("Could not open mapping index file.");

    ReadIndex( lStream );

    mBundleStartOffset = (long long)lStream.tellg() + 1;

    lStream.close();
  }

  void SeekTo( long long aPosition )
  {
    mMappingFileStream.seekg( aPosition + mBundleStartOffset );
  }

private:
  void DecompressBlock( char * & lBlock, long long aStoredSize, long long aBlockSize )
  {
    unsigned long lUncompressedLength = aBlockSize + 1; // do these go negative?
    Bytef *lUncompressed = new Bytef[ lUncompressedLength ]; // allocate the memory on the fly.
    assert( lUncompressed != NULL ); // temporary. TODO handle this better, clean up after myself.
  
    int lErrorCode = uncompress(lUncompressed, &lUncompressedLength, (Bytef *) lBlock, aStoredSize);
    assert( lErrorCode == 0 ); // temporary. TODO handle this better, clean up after myself.
  
    delete [] lBlock;
  
    lBlock = ( char * ) lUncompressed;
  
    lBlock[ aBlockSize ] = 0; // impose our own null termination BLEH TODO

    //cout << lBlock << endl;
  }


};
