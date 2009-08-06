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
  long Length;
  long mPosition;
public: // public methods
	Sequence( string aBases );
  Sequence( Sequence & aSequence );
  Sequence( string & aPath, long & aPosition, long & aLength );
  string Substring( long aIndex, long aCount );
  string Substring( long aIndex );
  string ToString();
  ~Sequence(void);
};
