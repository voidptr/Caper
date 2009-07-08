using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class MappingCache
  {
    private const char Tab = '\t';

    public string ContigIdent { get; private set; }
    public int LeftIndex { get; private set; }
    public int RightIndex { get; private set; }
    public List<List<Mapping>> Sequences { get; private set; }

    public MappingCache( List<List<Mapping>> lMappings, string aContigIdent, int aLeftIndex, int aRightIndex )
    {
      Sequences = lMappings;

      ContigIdent = aContigIdent;
      LeftIndex = aLeftIndex;
      RightIndex = aRightIndex;      
    }

    

    public ICollection<Mapping> GetReads( int aLeft, int aRight )
    {
      List<Mapping> lResult = new List<Mapping>();

      for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
      {
        if ( Sequences[ i ] != null )
          lResult.AddRange( Sequences[ i ] );
      }

      return lResult;
    }

    private int PrivateIndex( int aPublicIndex )
    {
      return aPublicIndex - LeftIndex;
    }
  }
}
