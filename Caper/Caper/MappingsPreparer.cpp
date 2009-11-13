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
  vector<string> * lMappings = ReadAllLines();

  //SeparateByContigs( lMappings );

  cout << " Sorting " << lMappings->size() << " mappings... " ;
  cout.flush();

  sort( lMappings->begin(), lMappings->end(), SortMapping(this) );

  int lSlashPos = mPath.find_last_of('/'); // todo, finish separating the path here, and make it into the filename that gets saved.

  string lFilename = mPath + ".sorted";
  WriteAllLines( lMappings, lFilename );

  delete lMappings;

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
