#pragma once

#include "Typedefs.h"

class SequenceEngine
{
protected:
  string mPath;
  string mIndexPath;

	static const char Tab = '\t';

public:
  Sequences * mSequences;
  vector<string> * mContigs;
public:
//  SequenceEngine(string aPath, string aIndexPath);
//  ~SequenceEngine(void);
  virtual void Initialize() = 0;
  //virtual void Initialize( string aIndexPath ) = 0;
  //virtual void SaveIndex( string aSavePath ) = 0;

  SequenceEngine(string aPath, string aIndexPath)
  {
    mPath = aPath;
    mIndexPath = aIndexPath;

    mSequences = new Sequences();
    mContigs = new vector<string>();
  }

  ~SequenceEngine()
  {
    delete mSequences;
    delete mContigs;
  }
};
