#pragma once

#include "stdafx.h"

class Sequence
{
private: // private member variables
  static const char NewLine = '\n';

  string mBases;
  ifstream mHandle;
  string mPath;

  bool mIsStream;

public: // public member variables (replace with accessors later)
  long long Length;
  long long mPosition;

public: // public methods
 

  
  Sequence( string aBases )
  {
	  mBases = aBases;
    mPath = "";
    mIsStream = false;
    Length = aBases.length();
  }

  Sequence( string & aPath, long long & aPosition, long long & aLength )
  {
    mBases = "";
    mHandle.open( aPath.c_str() );
    mPosition = aPosition;
    Length = aLength;

    mIsStream = true;
  }

  Sequence( Sequence & aSequence )
  {
    mBases = aSequence.mBases;
    mPath = aSequence.mPath;

    mPosition = aSequence.mPosition;
    Length = aSequence.Length;

    mIsStream = aSequence.mIsStream;

    if ( aSequence.mIsStream )
      mHandle.open( aSequence.mPath.c_str() );
  }

  string Substring( long long aIndex )
  {
    return Substring( aIndex, Length - aIndex );
  }

  string Substring( long long aIndex, long long aCount )
  {
    if ( !mIsStream )
      return mBases.substr((int) aIndex, (int) aCount ); // TODO, could create a nasty overflow
    else
    {
      if ( aCount > Length )
        throw string( "could not seek as far as requested" );

      mHandle.seekg( mPosition + aIndex );
      char * lBuffer = new char[aCount + 1];
      mHandle.get( lBuffer, aCount );
      lBuffer[aCount] = 0; // add the null char to terminate the string

      int lCount = mHandle.gcount();

      if ( !mHandle.good() )
        throw string("could not read whole sequence from handle.");
    
      return string(lBuffer);
    }
  }

  string ToString()
  {
    long long lCount =0;
    return Substring(lCount, Length);
  }

  const char * ToStringP() 
  { 
    return ToString().c_str(); 
  }

  ~Sequence(void)
  {
    mHandle.close();
  }
};
