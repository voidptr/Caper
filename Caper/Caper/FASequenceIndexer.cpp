#include "FASequenceIndexer.h"

FASequenceIndexer::FASequenceIndexer(string & aPath, string & aSavePath) : 
  SequenceIndexer(aPath, aSavePath) { }

FASequenceIndexer::FASequenceIndexer(char * aPath, char * aSavePath) : 
  SequenceIndexer(string(aPath), string(aSavePath)) { }

void FASequenceIndexer::Index()
{
  ifstream lStream( mPath.c_str(), ios_base::binary );

  if ( !lStream.is_open() )
    throw string("Could not open reference genome file.");
  
  string lLocus = "";
  long lLocusStart = 0;
  long lCount = 0;

  string lLine = "";

  boost::filesystem::path lPath(mPath);
  boost::filesystem::path lSavePath(mSavePath);

  string lOutputPath; 
  if (!(boost::filesystem::is_directory( lSavePath )) ) //not a directory
  {
    lOutputPath = lSavePath.file_string() + lPath.filename() + ".indexed"; // TODO, make this be user definable.
  }
  else
  {
    boost::filesystem::path lIndexPath( lSavePath );
    lIndexPath /= lPath.filename();

    lOutputPath = lIndexPath.file_string() + ".indexed";
  }

  ofstream lOutStream(lOutputPath.c_str());

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
          Sequence * lSeq = new Sequence( lOutputPath, lLocusStart, lCount );
          mSequences->insert( SequencePair (lLocus, lSeq ) );
          mContigs->push_back( lLocus );
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

  Sequence * lSeq = new Sequence( lOutputPath, lLocusStart, lCount );
          
  mSequences->insert( SequencePair( lLocus, lSeq ) );
  mContigs->push_back( lLocus );

  sort( mContigs->begin(), mContigs->end() );

  lStream.close();

  SaveIndex();
}


void FASequenceIndexer::SaveIndex()
{  
  boost::filesystem::path lPath(mPath);
  boost::filesystem::path lSavePath(mSavePath);

  string lSavedIndexFile; 
  if (!(boost::filesystem::is_directory( lSavePath )) ) //not a directory
  {
    lSavedIndexFile = lSavePath.file_string() + lPath.filename() + ".genomeindex"; // TODO, make this be user definable.
  }
  else
  {
    boost::filesystem::path lIndexPath( lSavePath );
    lIndexPath /= lPath.filename();

    lSavedIndexFile = lIndexPath.file_string() + ".genomeindex";
  }
  ofstream lIndexStream( lSavedIndexFile.c_str(), ios::binary );

  lIndexStream << mSequences->size() << endl;

  Sequences::iterator lSeqIterator;
  for (lSeqIterator = mSequences->begin(); lSeqIterator != mSequences->end(); lSeqIterator++ )
  {
    lIndexStream << lSeqIterator->first << Tab << lSeqIterator->second->mPosition << Tab << lSeqIterator->second->Length << endl;
  }

  lIndexStream.close();
}
