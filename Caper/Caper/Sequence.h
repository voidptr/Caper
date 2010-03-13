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
  
private: // private methods

public: // public member variables (replace with accessors later)
  long long Length;
  long long mPosition;
public: // public methods
	Sequence( string aBases );
  Sequence( Sequence & aSequence );
  Sequence( string & aPath, long long & aPosition, long long & aLength );
  string Substring( long long aIndex, long long aCount );
  string Substring( long long aIndex );
  string ToString();
  const char * ToStringP() { return ToString().c_str(); }
  ~Sequence(void);
};
