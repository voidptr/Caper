#include "FASequenceEngine.h"

FASequenceEngine::FASequenceEngine(string & aPath) : SequenceEngine(aPath) {}

FASequenceEngine::FASequenceEngine(char * aPath) : SequenceEngine(string(aPath)) { }

void FASequenceEngine::Initialize( string aIndexPath )
{
  ifstream lIndexStream( aIndexPath.c_str(), ios::binary );

  int lLocusCount = 0;
  lIndexStream >> lLocusCount;

  for (int i = 0; i < lLocusCount; i++ )
  {
    string lLocus = "";
    long lLocusStart = 0;
    long lCount =0;

    lIndexStream >> lLocus;
    lIndexStream >> lLocusStart;
    lIndexStream >> lCount;

    Sequence * lSeq = new Sequence( mPath, lLocusStart, lCount );
    mSequences.insert( SequencePair( lLocus, lSeq ) );
  }

  lIndexStream.close();
}

void FASequenceEngine::Initialize()
{
  ifstream lStream( mPath.c_str(), ios_base::binary );

  if ( !lStream.is_open() )
    throw string("Could not open reference genome file.");
  
  string lLocus = "";
  long lLocusStart = 0;
  long lCount = 0;

  string lLine = "";

  string lTmpFilePath = mPath + ".prepared";
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


void FASequenceEngine::SaveIndex( string aSavePath )
{
  string lSavedIndexFile = aSavePath + "saved.refgenomeindex";

  ofstream lIndexStream( lSavedIndexFile.c_str(), ios::binary );

  lIndexStream << mSequences.size() << endl;

  Sequences::iterator lSeqIterator;
  for (lSeqIterator = mSequences.begin(); lSeqIterator != mSequences.end(); lSeqIterator++ )
  {
    lIndexStream << lSeqIterator->first << Tab << lSeqIterator->second->mPosition << Tab << lSeqIterator->second->Length << endl;
  }

  lIndexStream.close();
}
