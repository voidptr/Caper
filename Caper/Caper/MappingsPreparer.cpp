#include "MappingsPreparer.h"


MappingsPreparer::MappingsPreparer(string aPath, string aSavePath, MappingFileFormat aFormat)
{
  mPath = aPath; 
  mSavePath = aSavePath;
  mFormat = aFormat;
}

string MappingsPreparer::PrepareMappings()
{  
  MappingUtilities = MappingUtilitiesFactory::BuildMappingUtilities( mFormat );

  if (!IsSorted())
    return SortMappingsAndWriteToTmpFile();

  return mPath;
}

bool MappingsPreparer::IsSorted()
{
  cout << " Are mappings already sorted? ";

  ifstream lStream( mPath.c_str(), ios::binary );

  if ( !lStream.is_open() )
    throw string("Could not open mappings file.");

  string lLastContig = "";
  int lLastIndex = -1;

  string lLine;
  while ( lStream.peek() > -1 )
  {
    getline( lStream, lLine );

    string lContig = MappingUtilities->GetContigIdent( lLine );
    int lIndex = MappingUtilities->GetIndex( lLine );

    if ( lContig.compare( lLastContig ) < 0 || 
      ( lContig == lLastContig && lIndex < lLastIndex ) )
    {
      lStream.close();
      cout << "No." << endl;
      return false;
    }
    lLastContig = lContig;
    lLastIndex = lIndex;
  }
  lStream.close();

  cout << "Yes!" << endl;

  return true;
}

string MappingsPreparer::SortMappingsAndWriteToTmpFile()
{
  cout << " Sorting mappings... ";
  cout.flush();

  vector<MappingIndex> * lMappingKeys = ReadAndInterpretMappingLines();

  int lSlashPos = mPath.find_last_of('/'); // todo, finish separating the path here, and make it into the filename that gets saved.


  boost::filesystem::path lPath(mPath);
  boost::filesystem::path lSavePath(mSavePath);
  string lOutputPath; 
  if (!(boost::filesystem::is_directory( lSavePath )) ) //not a directory
  {
    lOutputPath = lSavePath.file_string() + lPath.filename() + ".sorted"; // TODO, make this be user definable.
  }
  else
  {
    boost::filesystem::path lIndexPath( lSavePath );
    lIndexPath /= lPath.filename();

    lOutputPath = lIndexPath.file_string() + ".sorted";
  }

  //string lFilename = mPath + ".sorted";
  WriteAllLines( lMappingKeys, lOutputPath );

  delete lMappingKeys;

  cout << " Done!" << endl;

  return lOutputPath;  
}

vector<MappingIndex> * MappingsPreparer::ReadAndInterpretMappingLines()
{
  ifstream lStream( mPath.c_str(), ios::binary );

  if ( !lStream.is_open() || lStream.peek() < 0 )
    throw string("Could not open mapping file.");

  // estimate the number of lines
  string lFirstLine;
  getline( lStream, lFirstLine );
  lStream.seekg( 0, ios_base::end );
  int lApproximateLines = lStream.tellg() / lFirstLine.length();
  lStream.seekg( 0, ios_base::beg );

	vector<MappingIndex> * lIndexes = new vector<MappingIndex>();
	lIndexes->reserve( lApproximateLines );

  multimap<string,MappingIndex> lContigs;

  string lLine;
  long lPos = -1;
  while (lStream.peek() > -1 )
  {      
    lPos = lStream.tellg();
    getline( lStream, lLine );

    string lContig = MappingUtilities->GetContigIdent( lLine );
    int lIndex = MappingUtilities->GetIndex( lLine );

    lContigs.insert( pair<string,MappingIndex>( lContig, MappingIndex( MappingKey( lContig, lIndex ), lPos ) ) );
  }

  int lStart = 0;
  multimap<string,MappingIndex>::iterator lEnd = lContigs.end();

  for ( multimap<string,MappingIndex>::iterator i = lContigs.begin(); i != lContigs.end(); )
  {

    pair<multimap<string,MappingIndex>::iterator, multimap<string,MappingIndex>::iterator> ii;

    ii = lContigs.equal_range( i->first );

    int k = 0;
    for ( multimap<string,MappingIndex>::iterator j = ii.first; j != ii.second; ++j, ++k )
    {
      lIndexes->push_back( j->second );
    }    vector<MappingIndex>::iterator lBegin = (lIndexes->begin()) + lStart;

    sort( lBegin, lIndexes->end(), SortMappingIndexes(this) );
    
    i = ii.second;
    lStart += k;
  }

  lStream.close();

  return lIndexes;
}

bool MappingsPreparer::LessThanMappingIndex( MappingIndex & aLeft, MappingIndex & aRight )
{
	if ( aLeft.first.first == aRight.first.first && aLeft.first.second == aRight.first.second ) // if they are identical, woohoo!
		return false;

  string lLeftContig = aLeft.first.first;
  string lRightContig = aRight.first.first;

  if ( lLeftContig.compare( lRightContig ) < 0 )
    return true;
  else if ( lLeftContig == lRightContig )
  {
    int lLeftIndex = aLeft.first.second;
    int lRightIndex = aRight.first.second;

    if ( lLeftIndex < lRightIndex )
      return true;    
    else
      return false;
  }
  else
    return false;
}


void MappingsPreparer::WriteAllLines( vector<MappingIndex> * aMappingKeys, string & aFilename )
{
  ifstream lInStream( mPath.c_str(), ios::binary );
  if ( !lInStream.is_open() || lInStream.peek() < 0 )
    throw string("Could not open mapping file.");

  ofstream lOutStream( aFilename.c_str(), ios::binary );
  if ( !lOutStream.is_open() )
    throw string("Could not write to mappings tmp file.");

  string lLine = "";
  for (vector<MappingIndex>::iterator i = aMappingKeys->begin(); i < aMappingKeys->end(); i++)
  {
    lInStream.seekg( i->second );
    getline( lInStream, lLine );

    lOutStream << lLine << '\n';
  }

  lInStream.close();
  lOutStream.close();
}

