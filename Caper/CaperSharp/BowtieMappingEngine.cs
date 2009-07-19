using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class BowtieMappingEngine : MappingEngine
  {
    public BowtieMappingEngine( string aPath, Sequences aReferenceGenome )
      : base( aPath, aReferenceGenome )
    {
    }
    
    public override string GetSequence( string aLine )
    {
      return GetLinePieces( aLine )[ 4 ];
    }

    public override int GetIndex( string aLine )
    {
      return GetNormalizedIndex( GetLinePieces( aLine )[ 3 ] );
    }

    private int GetNormalizedIndex( string aValue )
    {
      return System.Convert.ToInt32( aValue );
    }

    public override string GetContigIdent( string aLine )
    {
      return GetLinePieces( aLine )[ 2 ];
    }
  }
}
