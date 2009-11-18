#include "MappingsPreparer.h"
#include "BowtieMappingUtilities.h"

MappingsPreparer::MappingsPreparer(string aPath)
{
  mPath = aPath;  
}

string MappingsPreparer::PrepareMappings()
{
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

    string lContig = GetContigIdent( lLine );
    int lIndex = GetIndex( lLine );

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

vector<MappingIndex> * MappingsPreparer::ReadAndInterpretMappingLines()
{
  char stime[9];
  
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

  // DEBUG
  _strtime( stime );
  cout << "Start Overall Operation" << stime << endl;
  // END DEBUG

  string lLine;
  long lPos = -1;
  while (lStream.peek() > -1 )
  {      
    lPos = lStream.tellg();
    getline( lStream, lLine );

    string lContig = GetContigIdent( lLine );
    int lIndex = GetIndex( lLine );

		//lIndexes->push_back( MappingIndex( MappingKey( lContig, lIndex ), lPos ) );  

    lContigs.insert( pair<string,MappingIndex>( lContig, MappingIndex( MappingKey( lContig, lIndex ), lPos ) ) );
  }

  // DEBUG
  _strtime( stime );
  cout << "Start Multimap Subsorting" << stime << endl;
  // END DEBUG

  int lStart = 0;
  multimap<string,MappingIndex>::iterator lEnd = lContigs.end();

  for ( multimap<string,MappingIndex>::iterator i = lContigs.begin(); i != lContigs.end(); )
  {

    pair<multimap<string,MappingIndex>::iterator, multimap<string,MappingIndex>::iterator> ii;

    ii = lContigs.equal_range( i->first );

        // DEBUG
    _strtime( stime );
    cout << "Sorting contig " << i->first << " " << stime << endl;
    // END DEBUG

    int k = 0;
    for ( multimap<string,MappingIndex>::iterator j = ii.first; j != ii.second; ++j, ++k )
    {
      lIndexes->push_back( j->second );
    }    vector<MappingIndex>::iterator lBegin = (lIndexes->begin()) + lStart;

    sort( lBegin, lIndexes->end(), SortMappingIndexes(this) );
    
    i = ii.second;
    lStart += k;
  }

  // DEBUG
  _strtime( stime );
  cout << "Finished overall sorting operation" << stime << endl;
  // END DEBUG

  lStream.close();

	return lIndexes;
}
string MappingsPreparer::SortMappingsAndWriteToTmpFile()
{
  char stime[9];
  // DEBUG
  _strtime( stime );
  cout << stime << endl;
  // END DEBUG

  cout << " Sorting mappings... " ;
  cout.flush();

  vector<MappingIndex> * lMappingKeys = ReadAndInterpretMappingLines();



  // DEBUG
  _strtime( stime );
  cout << stime << endl;
  // END DEBUG

  
 
  int lSlashPos = mPath.find_last_of('/'); // todo, finish separating the path here, and make it into the filename that gets saved.

  string lFilename = mPath + ".sorted";
  WriteAllLines( lMappingKeys, lFilename );

  delete lMappingKeys;

  cout << "Done!" << endl;

  // DEBUG
  _strtime( stime );
  cout << stime << endl;
  // END DEBUG


  return lFilename;  
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
