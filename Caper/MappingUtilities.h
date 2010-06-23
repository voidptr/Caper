#pragma once
#include "stdafx.h"
#include "Mapping.h"
#include "Sequence.h"

class MappingUtilities
{
public:
  virtual vector<string> GetLinePieces( string & aLine )
  {
	  vector<string> lPieces;

	  string lBit;
	  istringstream lStream( aLine );
	  do
	  {
      lBit = "";
		  lStream >> lBit;

		  if ( lBit.length() > 0 )
			  lPieces.push_back( lBit );
		  else
			  break;

	  } while (true);

	  return lPieces;
  }

  virtual string GetLineItem( int aColumn, string & aLine ) // column, line
  {
    string lValue;
    istringstream lStream( aLine );
    for (int i = 0; i <= aColumn; i++)
    {
      lValue = "";
      lStream >> lValue;

      assert( lValue.length() > 0 ); // line doesn't have enough elements
    }
    return lValue;
  }

  virtual int GetIndexColumn() = 0;
  virtual int GetSequenceColumn() = 0;
  virtual int GetNameColumn() = 0;
  virtual int GetContigIdentColumn() = 0;
  virtual int GetStrandColumn() = 0;

  virtual Mapping* BuildMapping( string & aLine )
  {
    vector<string> lPieces( GetLinePieces( aLine ) );

    long long lIndex = atoi(lPieces[ GetIndexColumn() ].c_str()); // todo, move this conversion to the proper method. remove localized atoi use.
     
    string lSeq = lPieces[ GetSequenceColumn() ];
    string lName = lPieces[ GetNameColumn() ];
    string lStrand = lPieces[ GetStrandColumn() ];

    // This is probably not the right place for this, since, concievably, not all mapping
    // types use +/- to denote strand. But where else to put, w/o making
    // a global Orientation enum? This probably needs a neat namespace somewhere.
    Mapping::Orientation lOrientation;
    if ( lStrand[0] == '+' )
      lOrientation = Mapping::PLUS;
    else if ( lStrand[0] == '-' )
      lOrientation = Mapping::MINUS;
    else 
      throw string("Unsupported Orientation Type (not + or -)");

    Mapping * lMapping = new Mapping( lName, lIndex, new Sequence( lSeq ), lOrientation );
    return lMapping;
  }

  virtual string GetContigIdent( string & aLine )
  {
    return GetLineItem( GetContigIdentColumn(), aLine );
  }
   
  virtual long long GetIndex( string & aLine )
  {
    return atoi( GetLineItem( GetIndexColumn(), aLine ).c_str() ); // could be an overflow. :/
  }
   
  virtual string GetSequence( string & aLine )
  {
    return GetLineItem( GetSequenceColumn(), aLine );
  }
   
  virtual string GetName( string & aLine )
  {
    return GetLineItem( GetNameColumn(), aLine );
  }
   
  virtual string GetStrand( string & aLine )
  {
    return GetLineItem( GetStrandColumn(), aLine );
  }

};
