#include "Sequence.h"

Sequence::Sequence( string & aBases )
{
	mBases = aBases;
  mIsStream = false;
  Length = aBases.length();
}

Sequence::Sequence(ifstream & aHandle, long & aPosition, long & aLength )
{
  mBases = "";
  mHandle = &aHandle;
  mPosition = aPosition;
  Length = aLength;

  mIsStream = true;
}
string Sequence::Substring( long aIndex )
{
  return Substring( aIndex, Length - aIndex );
}

string Sequence::Substring( long aIndex, long aCount )
{
  if ( !mIsStream )
    return mBases.substr((int) aIndex, (int) aCount );
  else
  {
    if ( aCount > Length )
      throw string( "could not seek as far as requested" );

    mHandle->seekg( mPosition + aIndex );
    char * lBuffer = new char[aCount];
    mHandle->get( lBuffer, aCount );

    if ( mHandle->gcount() < aCount )
      throw string("could not read whole sequence from handle.");
  
    return string(lBuffer);
  }
}

string Sequence::ToString()
{
  long lCount =0;
  return Substring(lCount, Length);
}