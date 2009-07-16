using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public class BowtieMappingsPreparer : MappingsPreparer
  {
    public BowtieMappingsPreparer( string aPath ) : base( aPath ) { }

    public override int GetIndex( string aLine )
    {
      return GetNormalizedIndex( GetLinePieces( aLine )[ 3 ] );
    }

    private int GetNormalizedIndex( string aValue )
    {
      return System.Convert.ToInt32( aValue ) - 1;
    }

    public override string GetContigIdent( string aLine )
    {
      return GetLinePieces( aLine )[ 2 ];
    }
  }
}
