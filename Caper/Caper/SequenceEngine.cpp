#include "SequenceEngine.h"

SequenceEngine::SequenceEngine(string aPath)
{
  mPath = aPath;
  mSequences = new Sequences();
}

SequenceEngine::~SequenceEngine()
{
  delete mSequences;
}
