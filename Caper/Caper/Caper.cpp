
#include "Caper.h"


void Caper::UserInterface(int argc, char * const argv[] )
{
  string lUsageString = "Caper v0.1\nUsage: caper <referencegenome.fa> <mapviewmappings.mapview>\n";

  if ( argc < 3 )
  {
    cout << lUsageString;
    return;
  }

  SequenceReader lSequenceReader(argv[1]);
  Sequence* lReferenceSequence = lSequenceReader.Read();

  MapviewMappingEngine lMapviewMappingEngine(argv[2], lReferenceSequence);
  lMapviewMappingEngine.Initialize();

  cout << "> ";

  string lInput = "";
  while ( cin >> lInput )
  {
    if ( !IsValid( lInput ) )
    {
      cout << "Invalid Input: Format must be X:Y or X:Y:p (for pretty mode)";
      cout << "> ";
      continue;
    }

    string lBits[3];
    GetParameters( lInput, lBits );

    int lLeft = atoi( lBits[ 0 ].c_str() );

    int lRight;
    if ( lBits[1].length() > 0 )
      lRight = atoi( lBits[ 1 ].c_str() );
    else
      lRight = lLeft;

    lLeft -= ReadLength;
    if ( lLeft < 0 )
      lLeft = 0;

    if ( lRight > lReferenceSequence->Bases.length() )
    {
      cout << "Your Right Parameter is bigger than your reference genome.\n";
      cout << "> ";
      continue;
    }

    vector<Mapping*> lMappings = lMapviewMappingEngine.GetReads( lLeft, lRight );
    if ( lBits[2].length() > 0 ) // engage pretty mode
    {    
      cout << lLeft << "\n";

      

      if ( lRight + 1 < lReferenceSequence->Bases.length() )
        cout << lReferenceSequence->Bases.substr( lLeft, lRight - lLeft + 1 ) << "\n";
      else
        cout << lReferenceSequence->Bases.substr( lLeft ) << "\n";

      for ( int i = 0 ; i < lMappings.size(); i++ ) // ( Mapping lMapping in lMappings )
      {

        string lHighlightedString = lMappings[i]->mSequence->Bases;
        for ( int j = 0; j < lHighlightedString.length(); j++ )
        {
          if ( lHighlightedString[j] != lReferenceSequence->Bases[ lMappings[i]->Index + j ] )
          {
            lHighlightedString[j] = toupper( lHighlightedString[j] );
          }
        }

        cout << PadLeft( lMappings[i]->Index - lLeft ) << lHighlightedString << "\n"; 
      }
    }
    else
    {
      for ( int i = 0 ; i < lMappings.size(); i++ ) //foreach ( Mapping lMapping in lMappings )
      {
        cout << "Index " << lMappings[i]->Index << ": " << lMappings[i]->mSequence->Bases << "\n";
      }
    }

    MapviewMappingEngine::DestroyMappings( lMappings );

    cout << "> ";
  }
}


string Caper::PadLeft( int aCount )
{
  string lThing = "";

  for (int i = 0; i < aCount; i++ )
    lThing.append(" ");

  return lThing;
}


bool Caper::IsValid( string lInput )
{
  try
  {
    string lBits[3];
    GetParameters( lInput, lBits );

    int lLeft = atoi( lBits[ 0 ].c_str() );

    int lRight;
    if ( lBits[1].length() > 0 )
      lRight = atoi( lBits[ 1 ].c_str() );
    else
      lRight = lLeft;

    if ( lLeft > lRight || lLeft < 0 || lRight < 0 )
      return false;
  }
  catch ( int i )
  {
    return false;
  }

  return true;
}

void Caper::GetParameters( string aLine, string * aBits )
{
  istringstream lStringStream( aLine );

  for (int i = 0; i < 3; i++ )
  {
    getline( lStringStream, aBits[i], Colon );
  }
}

