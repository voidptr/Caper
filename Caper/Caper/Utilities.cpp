#include "Utilities.h"

void Utilities::Split(const string & aString, vector<string> & aTokens, const string & aDelimiters)
{
    long lPreviousPosition = aString.find_first_not_of( aDelimiters, 0 );
    long lDelimiterPosition = aString.find_first_of( aDelimiters, lPreviousPosition );

    while ( string::npos != lDelimiterPosition || string::npos != lPreviousPosition )
    {
        aTokens.push_back( aString.substr( lPreviousPosition, lDelimiterPosition - lPreviousPosition ) );
        lPreviousPosition = aString.find_first_not_of(aDelimiters, lDelimiterPosition);        
        lDelimiterPosition = aString.find_first_of(aDelimiters, lPreviousPosition);
    }
}

void Utilities::SplitPath(const string& aPath,
                      vector<string>& tokens)
{
  // This code isn't super robust. It just tries splitting forward slash first, and if that doesn't work, then backslash. :/
  // This should be platform specific, to avoid errors.
  // Further, this code is just stupidly written. BLeh.
  vector<string> lForwardTokens;
  Utilities::Split( aPath, lForwardTokens, "/" );
  if ( lForwardTokens.size() == 1 && lForwardTokens[0] == aPath ) // no forward slash delimiters were found, do backslash.
  {
    vector<string> lBackslashTokens;
    Utilities::Split( aPath, lBackslashTokens, "\\" ); // whether there were any delimiters or not, use what it dumped out.
    for(int i = 0; i < lBackslashTokens.size(); i++ )
    {
      tokens.push_back( lBackslashTokens[i] );
    }
  }
  else // found some / delimiters, or the thing was empty. either way this is fine.
  {
    for(int i = 0; i < lForwardTokens.size(); i++ )
    {
      tokens.push_back( lForwardTokens[i] );
    }
  }
}

string Utilities::GetFilename(const string &aPath)
{
  vector<string> lPathTokens;
  Utilities::SplitPath( aPath, lPathTokens );

  if ( lPathTokens.size() == 0 ) // blank! The path was empty, OMG
    return "";

  return lPathTokens[ lPathTokens.size() - 1 ]; // return the last token.
}

string Utilities::ToUpper( string aString )
{
  for( int i = 0; i < aString.length(); ++i )
  {
    aString[i] = toupper( aString[i] );
  }

  return aString;
}
