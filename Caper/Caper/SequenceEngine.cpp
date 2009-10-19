#include "SequenceEngine.h"

SequenceEngine::SequenceEngine(string aPath)
{
  mPath = aPath;
  mSequences = new Sequences();
  mContigs = new vector<string>();
}

SequenceEngine::~SequenceEngine()
{
  delete mSequences;
  delete mContigs;
}
