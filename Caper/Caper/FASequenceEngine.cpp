#include "FASequenceEngine.h"

// TMP for development
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include "Typedefs.h"
// END TMP

FASequenceEngine::FASequenceEngine(string & aPath) : SequenceEngine(aPath) {}

void FASequenceEngine::Initialize()
{
  ifstream lStream( mPath.c_str(), ios_base::binary );

  if ( !lStream.is_open() )
    throw string("Could not open reference genome file.");
  
  string lLocus = "";
  long lLocusStart = 0;
  long lCount = 0;

  string lLine = "";

  string lTmpFilePath = mPath + ".tmp";
  ofstream lOutStream(lTmpFilePath.c_str());

  if ( !lOutStream.is_open() )
    throw string("Could not write to reference genome tmp output file.");

  while ( lStream.peek() > -1 )
  {
    getline( lStream, lLine );
    if ( lLine.length() > 0 )
    {
      if ( lLine[0] == GreaterThan )
      {
        if ( lLocus.length() > 0 ) // there was a previous one, so close it up.
        {
          lOutStream << NewLine;
          Sequence * lSeq = new Sequence( lTmpFilePath, lLocusStart, lCount );
          mSequences.insert( SequencePair (lLocus, lSeq ) );
          lCount++;
        }
        
        // not a fan of this bit. :/
        lLine = lLine.substr(1);
        if ( lLine.find_first_of(' ') != -1 )
          lLocus = lLine.substr(0, lLine.find_first_of(' '));
        else if ( lLine.find_first_of(',') != -1 )
          lLocus = lLine.substr(0, lLine.find_first_of(','));
        else
          lLocus = lLine;

        lLocusStart += lCount;
        lCount = 0;
      }
      else
      {
        lOutStream.write( lLine.c_str(), lLine.length() );
        lCount += lLine.length();
      }
    }
  }

  Sequence * lSeq = new Sequence( lTmpFilePath, lLocusStart, lCount );
          
  mSequences.insert( SequencePair( lLocus, lSeq ) );

  lStream.close();
}
