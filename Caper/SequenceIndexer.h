#pragma once

#include "Typedefs.h"

class SequenceIndexer
{
protected:
  string mPath;
  string mSavePath;

	static const char Tab = '\t';
public:
  Sequences * mSequences;
  vector<string> * mContigs;
public:
 /* SequenceIndexer(string aPath, string aSavePath);
  ~SequenceIndexer(void);*/
  virtual void Index() = 0;
  SequenceIndexer(string aPath, string aSavePath)
  {
    mPath = aPath;
    mSavePath = aSavePath;
    mSequences = new Sequences();
    mContigs = new vector<string>();
  }

  ~SequenceIndexer()
  {
    delete mSequences;
    delete mContigs;
  }
};
