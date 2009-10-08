#include "MapviewMappingEngine.h"

MapviewMappingEngine::MapviewMappingEngine(string aPath, Sequences * aGenome) 
: MappingEngine(aPath, aGenome), MapviewMappingUtilities()
{
}

MapviewMappingEngine::MapviewMappingEngine( char * aPath, SequenceEngine * lMappingEngine ) :
  MappingEngine(string(aPath), lMappingEngine->mSequences),
  MapviewMappingUtilities()
{
}


string MapviewMappingEngine::GetContigIdent( string & aLine )
{
  return MapviewMappingUtilities::GetContigIdent(aLine);
}

int MapviewMappingEngine::GetIndex( string & aLine )
{
  return MapviewMappingUtilities::GetIndex(aLine);
}

string MapviewMappingEngine::GetName( string & aLine )
{
  return MapviewMappingUtilities::GetName(aLine);
}

string MapviewMappingEngine::GetSequence( string & aLine )
{
  return MapviewMappingUtilities::GetSequence(aLine);
}

string MapviewMappingEngine::GetStrand( string & aLine )
{
  return MapviewMappingUtilities::GetStrand(aLine);
}