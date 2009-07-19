using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public class MapviewMappingsPreparer : MappingsPreparer
  {
    public MapviewMappingsPreparer( string aPath ) : base( aPath ) { }

    public override int GetIndex( string aLine )
    {
      return GetNormalizedIndex( GetLinePieces( aLine )[ 2 ] );
    }

    private int GetNormalizedIndex( string aValue )
    {
      return System.Convert.ToInt32( aValue ) - 1;
    }

    public override string GetContigIdent( string aLine )
    {
      return GetLinePieces( aLine )[ 1 ];
    }
  }
}
