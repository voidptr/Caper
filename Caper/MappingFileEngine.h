#pragma once

#include "stdafx.h"
#include "MappingIndex.h"
#include "MappingUtilitiesFactory.h"

class MappingFileEngine
{
protected: // attributes
  MappingIndex mMappingIndex;
//  map<string, long long> mLastIndexesByContig;

  long long mReadLength;

  string mMappingFilePath;
  string mIndexFilePath;

  ifstream mMappingFileStream;

  MappingUtilities * mMappingUtilities;

public: // attributes
  MappingFileFormat Format;

public: // methods
  MappingFileEngine( string & aMappingFilePath, string & aIndexFilePath )
  {
    mMappingFilePath = aMappingFilePath;
    mIndexFilePath = aIndexFilePath;
  }

  void Initialize()
  {
    PopulateIndex();

    mMappingFileStream.open( mMappingFilePath.c_str(), ios::binary );
    if ( !mMappingFileStream.is_open() )
      throw string("Could not open mapping file.");
  }

  long long GetReadLength()
  {
    return mReadLength;
  }

  string FirstContig()
  {
    if ( mMappingIndex.empty() )
      return "";

    return mMappingIndex.begin()->first;
  }

  string LastContig()
  {
    if ( mMappingIndex.empty() )
      return "";

    return mMappingIndex.rbegin()->first;
  }

  string EndContig()
  {
    return "";
  }

  string NextContig( string & aContig )
  {
    if ( aContig == LastContig() ) // we're at the end
      return EndContig();

    MappingIndex::iterator lContig = mMappingIndex.find( aContig );
    ++lContig;
    return lContig->first;
  }

  string PreviousContig( string & aContig )
  {
    if ( aContig == FirstContig() ) // we're already at the beginning
      return EndContig();

    MappingIndex::iterator lContig = mMappingIndex.find( aContig );
    --lContig;
    return lContig->first;
  }

  long long BeginWindow( const string & aContig="" )
  {
    string lContig = aContig;
    if ( lContig == "" )
      lContig = FirstContig();

    MappingIndex::mapped_type::iterator lWindow = mMappingIndex.find( aContig )->second.begin();

    if ( lWindow == mMappingIndex.find( aContig )->second.end() ) // empty!
      return EndWindow();

    return lWindow->first;
  }

  long long NextWindow( string & aContig, long long aWindowNumber )
  {
    MappingIndex::iterator lContigIt = mMappingIndex.find( aContig );

    MappingIndex::mapped_type::iterator lWindow = mMappingIndex.find( aContig )->second.find( aWindowNumber );

    bool lFake = false;
    if ( lWindow == mMappingIndex.find( aContig )->second.end() ) // can't find the one we're asking for. Throw a stake in the ground and move up, then pull the stake out.
    {
      lFake = true;
      mMappingIndex.find( aContig )->second[ aWindowNumber ]; // insert the fake.
      lWindow = mMappingIndex.find( aContig )->second.find( aWindowNumber );
    }

    ++lWindow; // increment.

    if ( lWindow != mMappingIndex.find( aContig )->second.end() )
    {
      if ( !lFake )
        return lWindow->first;
      else
      {
        long long lWindowNumber = lWindow->first;
        mMappingIndex.find( aContig )->second.erase( aWindowNumber );
        return lWindowNumber;
      }
    }
    else
    {
      if ( lFake )
        mMappingIndex.find( aContig )->second.erase( aWindowNumber );
      return EndWindow(); // we're off the back end
    }
    
  }

  // this method is a repeat of NextWindow (except for the --) (and a spiritual repeat of the similar ones in MappingCache.h) FUUUUUU
  long long PreviousWindow( string & aContig, long long aWindowNumber )
  {
    MappingIndex::iterator lContigIt = mMappingIndex.find( aContig );

    MappingIndex::mapped_type::iterator lWindow = mMappingIndex.find( aContig )->second.find( aWindowNumber );

    bool lFake = false;
    if ( lWindow == mMappingIndex.find( aContig )->second.end() ) // can't find the one we're asking for. Throw a stake in the ground and move up, then pull the stake out.
    {
      lFake = true;
      mMappingIndex.find( aContig )->second[ aWindowNumber ]; // insert the fake.
      lWindow = mMappingIndex.find( aContig )->second.find( aWindowNumber );
    }

    --lWindow; // decrement. 

    if ( lWindow != mMappingIndex.find( aContig )->second.end() )
    {
      if ( !lFake )
        return lWindow->first;
      else
      {
        long long lWindowNumber = lWindow->first;
        mMappingIndex.find( aContig )->second.erase( aWindowNumber );
        return lWindowNumber;
      }
    }
    else
    {
      if ( lFake )
        mMappingIndex.find( aContig )->second.erase( aWindowNumber );
      return EndWindow(); // we're off the back end
    }
  }

  long long EndWindow( const string & aContig="" )
  {
    return -1; // const
  }

  virtual vector<Mapping *> * FetchMappings( string & aContig, long long aWindowNumber )
  {
    MappingIndex::iterator lContigIterator = mMappingIndex.find(aContig);
    if ( lContigIterator == mMappingIndex.end() )
      throw string( "Could not find requested contig." );

    MappingIndex::mapped_type::iterator lWindowIterator = lContigIterator->second.find(aWindowNumber); 

    if ( lWindowIterator == lContigIterator->second.end() ) // non-existent window!
      return NULL;

    char * lBlock = new char[ lWindowIterator->second.BlockSizeInBytes + 1 ]; 
    ReadBlock( lWindowIterator->second.BlockOffset, lWindowIterator->second.CompressedBlockSizeInBytes );

    lBlock[ lWindowIterator->second.BlockSizeInBytes ] = 0; // impose our own null termination. :/

    vector<Mapping *> * lMappings = PrepareMappings( lBlock );
    delete [] lBlock;

    return lMappings;
  }

  ~MappingFileEngine()
  {
    mMappingFileStream.close();
  }

protected: // virtual
  virtual void PopulateIndex()
  {
    ifstream lStream( mIndexFilePath.c_str(), ios::binary );
    if ( !lStream.is_open() )
      throw string("Could not open mapping index file.");

    ReadIndex( lStream );

    lStream.close();
  }

  virtual void SeekTo( long long aPosition )
  {
    mMappingFileStream.seekg( aPosition );
  }

protected: // non-virtual
  void ReadIndex( ifstream & aStream )
  {
    // read the datatype
    string lDatatype;
    aStream >>lDatatype;
    Format = MappingUtilitiesFactory::ToFormat( lDatatype );
    mMappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( Format );
  
    // read the read length
    aStream >> mReadLength;

    // read the number of contigs
    int lContigCount = 0;
    aStream >> lContigCount; // get the count of contigs as the first line.

    // read the data for each contig
    for ( int i = 0; i < lContigCount; i++ )
    {
      string lContigIdent = "";
      int lWindowCount = 0;
      long long lLastIndexInThisContig = 0;
  
      aStream >> lContigIdent;
      aStream >> lWindowCount;

      for (int j = 0; j < lWindowCount; j++ ) // could this break something? see where we are getting this window count.
      {
        long long lWindowNumber;
        long long lOffset;
        int lBlockSize;
        int lStoredSize;
        aStream >> lWindowNumber;
        aStream >> lOffset; // TODO have the reading of these reside in the StoredMappingBlock class (which does not yet exist), same with the out. Need to decouple this crap.
        aStream >> lBlockSize; // ditto
        aStream >> lStoredSize; // ditto
        
        mMappingIndex.AddEntry( lContigIdent, lWindowNumber, -1, lOffset, -1, lBlockSize, lStoredSize );
      }
    }
  }

  // ReadBlock
  // given an existing and initialized stream.
  // read a byte block of arbitrary length from a provided offset
  // return the byte block 
  char * ReadBlock( long long aOffset, long long aLength )
  {
    char * lBlock = new char[ aLength ]; 
    SeekTo( aOffset );
    mMappingFileStream.read( lBlock, aLength );

    return lBlock;
  }
  
  // PrepareMappings
  // from a provided byte block (pointer)
  // prepare a vector of Mapping(s) which represent the lines in that block
  // return the vector.
  vector<Mapping *> * PrepareMappings( char * aBlock )
  {
    vector<Mapping *> * lMappings = new vector<Mapping *>();
  
    istringstream lStream(string(aBlock), ios_base::binary);    
    
    string lLine;
    while ( lStream.peek() >= 0 )
    {
      getline( lStream, lLine );
      if ( lLine.length() < 1 )
        break;
  
	    lMappings->push_back( mMappingUtilities->BuildMapping( lLine ) );
    }

    return lMappings;
  }
};
