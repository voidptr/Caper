#pragma once

#include "stdafx.h"

class Sequence
{
private: // private member variables
  static const char NewLine = '\n';

  string mBases;
  ifstream * mHandle;
  bool mIsStream;
  long mPosition;
private: // private methods

public: // public member variables (replace with accessors later)
  long Length;
public: // public methods
	Sequence( string & aBases );
  Sequence( ifstream & aHandle, long & aPosition, long & aLength );
  string Substring( long aIndex, long aCount );
  string Substring( long aIndex );
  string ToString();
};
