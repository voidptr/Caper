#include "MappingsPreparer.h"
#include "BowtieMappingUtilities.h"

MappingsPreparer::MappingsPreparer(string & aPath)
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
  ifstream lStream( mPath.c_str() );

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
      return false;
    }
    lLastContig = lContig;
    lLastIndex = lIndex;
  }
  lStream.close();
  return true;
}

vector<string>* MappingsPreparer::ReadAllLines()
{
  ifstream lStream( mPath.c_str() );

  vector<string> * lList = new vector<string>();

  while (lStream.peek() > -1 )
  {
    string lLine;
    getline( lStream, lLine );

    lList->push_back( lLine );
  }

  lStream.close();
  return lList;
}
string MappingsPreparer::SortMappingsAndWriteToTmpFile()
{
  vector<string> * lMappings = ReadAllLines();

  std::sort( lMappings->begin(), lMappings->end(), SortMapping(this) );

  string lFilename = mPath + ".stl.tmp";
  WriteAllLines( lMappings, lFilename );

  delete lMappings;

  return lFilename;
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

  for each ( string lLine in *aMappingsFile )
  {
    lStream << lLine << NewLine;
  }

  lStream.close();
}