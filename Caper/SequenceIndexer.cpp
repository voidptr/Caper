#include "SequenceIndexer.h"

SequenceIndexer::SequenceIndexer(string aPath, string aSavePath)
{
  mPath = aPath;
  mSavePath = aSavePath;
  mSequences = new Sequences();
  mContigs = new vector<string>();
}

SequenceIndexer::~SequenceIndexer()
{
  delete mSequences;
  delete mContigs;
}
