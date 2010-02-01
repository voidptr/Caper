#include "SequenceEngine.h"

SequenceEngine::SequenceEngine(string aPath, string aIndexPath)
{
  mPath = aPath;
  mIndexPath = aIndexPath;

  mSequences = new Sequences();
  mContigs = new vector<string>();
}

SequenceEngine::~SequenceEngine()
{
  delete mSequences;
  delete mContigs;
}
