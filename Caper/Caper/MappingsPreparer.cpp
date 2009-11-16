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

  ifstream lStream( mPath.c_str() );

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
vector<MappingIndex>* MappingsPreparer::InterpretMappingLines( vector<string>* aMappings )
{
	vector<MappingIndex> * lIndexes = new vector<MappingIndex>();
	lIndexes->reserve( aMappings->size() );

	int j = 0;
	for (vector<string>::iterator i = aMappings->begin(); i != aMappings->end(); ++i, ++j )
	{   
		string lContig = GetContigIdent( *i );
		int lIndex = GetIndex( *i );

		lIndexes->push_back( MappingIndex( MappingKey( lContig, lIndex ), j ) );
	}

	return lIndexes;
}


vector<string>* MappingsPreparer::ReadAllLines()
{
  //char stime[9];

  ifstream lStream( mPath.c_str() );


  if ( lStream.is_open() && lStream.peek() > -1 )
  {
    //// DEBUG
    //_strtime( stime );
    //cout << stime << endl;
    //// END DEBUG

    string lFirstLine;
    getline( lStream, lFirstLine );
    lStream.seekg(0, ios_base::end);
    int lApproximateLines = lStream.tellg() / lFirstLine.length();
    vector<string> * lList = new vector<string>();
    lList->reserve( lApproximateLines );
    lStream.seekg(0, ios_base::beg );

    string lLine;
    while (lStream.peek() > -1 )
    {      
      getline( lStream, lLine );

      lList->push_back( lLine );
    }

    lStream.close();

    //// DEBUG
    //_strtime( stime );
    //cout << stime << endl;
    //// END DEBUG

    return lList;
  }
  else
  {
    throw string("Could not open mapping file.");
  }

  

}
string MappingsPreparer::SortMappingsAndWriteToTmpFile()
{
	cout << " Reading mappings... " << endl ;
  cout.flush();

  vector<string> * lMappings = ReadAllLines();

  cout << " Sorting " << lMappings->size() << " mappings... " ;
  cout.flush();

  vector<MappingIndex> * lMappingKeys = InterpretMappingLines( lMappings );

  sort ( lMappingKeys->begin(), lMappingKeys->end(), SortMappingIndexes(this) );

  vector<string> * lSortedMappingLines = new vector<string>();
  lSortedMappingLines->reserve( lMappings->size() );

  for ( vector<MappingIndex>::iterator i = lMappingKeys->begin(); i < lMappingKeys->end(); i++)
  {
	  lSortedMappingLines->push_back( lMappings->at( (*i).second ) );
  }
  


  //SeparateByContigs( lMappings );



  //sort( lMappings->begin(), lMappings->end(), SortMapping(this) );

  int lSlashPos = mPath.find_last_of('/'); // todo, finish separating the path here, and make it into the filename that gets saved.

  string lFilename = mPath + ".sorted";
  WriteAllLines( lSortedMappingLines, lFilename );

  delete lMappings;
  delete lSortedMappingLines;
  delete lMappingKeys;

  cout << "Done!" << endl;

  return lFilename;  
}

bool MappingsPreparer::SeparateByContigs( vector<string> * aMappings )
{
  map<string, vector<string>*> lContigMappings;
  map<string, vector<string>*>::iterator lIterator;
  

  for (vector<string>::iterator i = aMappings->begin(); i != aMappings->end(); ++i )
  {    
    string lContigIdent = GetContigIdent( *i );

    lIterator = lContigMappings.find( lContigIdent );

    if ( lIterator == lContigMappings.end() )
    {
      lContigMappings.insert( pair<string, vector<string>*>(lContigIdent, new vector<string>()) );
      lIterator = lContigMappings.find( lContigIdent );      
    }
    lIterator->second->push_back( *i );
  }

  return true;
}

bool MappingsPreparer::LessThanMappingLine( string & aLeft, string & aRight )
{
  if ( aLeft == aRight ) // if they are identical, woohoo!
    return false;

  string lLeftContig = GetContigIdent( aLeft );
  string lRightContig = GetContigIdent( aRight );

  if ( lLeftContig.compare( lRightContig ) < 0 )
    return true;
  else if ( lLeftContig == lRightContig )
  {
    int lLeftIndex = GetIndex( aLeft );
    int lRightIndex = GetIndex( aRight );

    if ( lLeftIndex < lRightIndex )
      return true;    
    else
      return false;
  }
  else
    return false;
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


void MappingsPreparer::WriteAllLines( vector<string> *aMappingsFile, string & aFilename )
{
  ofstream lStream( aFilename.c_str() );

  if ( !lStream.is_open() )
    throw string("Could not write to mappings tmp file.");

  for (int i = 0; i < aMappingsFile->size(); i++)
  {
    lStream << aMappingsFile->at(i) << '\n';
  }

  lStream.close();
}
