#pragma once
#include "SequenceEngine.h"
#include "Typedefs.h"

#include <algorithm>

class FASequenceEngine :
  public SequenceEngine
{
private:
  static const char GreaterThan = '>';
  static const char NewLine = '\n';
public:
  FASequenceEngine(string & aPath, string & aIndexPath) : SequenceEngine(aPath, aIndexPath) { }

  FASequenceEngine(char * aPath, char * aIndexPath) : SequenceEngine(string(aPath), string (aIndexPath)) { }

  void Initialize()
  {
    ifstream lIndexStream( mIndexPath.c_str(), ios::binary );

    int lLocusCount = 0;
    lIndexStream >> lLocusCount;

    for (int i = 0; i < lLocusCount; i++ )
    {
      string lLocus = "";
      long long lLocusStart = 0;
      long long lCount = 0;

      lIndexStream >> lLocus;
      lIndexStream >> lLocusStart;
      lIndexStream >> lCount;

      Sequence * lSeq = new Sequence( mPath, lLocusStart, lCount );
      mSequences->insert( SequencePair( lLocus, lSeq ) );
      mContigs->push_back( lLocus );
    }

    sort( mContigs->begin(), mContigs->end() );

    lIndexStream.close();
  }
};
