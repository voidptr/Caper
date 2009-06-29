using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  class Program
  {
    const char Colon = ':';
    const int ReadLength = 36;

    static void Main( string[] args )
    {
      string lUsageString = "CaperSharp v0.1\nUsage: caper <referencegenome.fa> <mapviewmappings.mapview>\n";

      if ( args.Length < 2 )
      {
        Console.Write( lUsageString );
        return;
      }

      SequenceEngine lSequenceReader = new FASequenceEngine( args[ 0 ] );
      lSequenceReader.Initialize();

      MappingEngine lMappingReader = new MapviewMappingEngine( args[ 1 ], lReferenceSequence );
      lMappingReader.Initialize();

      
      Console.Write( "> " );

      string lInput = "";
      while ( ( lInput = Console.ReadLine() ).Length > 0 )
      {
        if ( !IsValid( lInput ) )
        {
          Console.WriteLine( "Invalid Input: Format must be X:Y or X:Y:p (for pretty mode)" );
          Console.Write( "> " );
          continue;
        }

        string[] lBits = lInput.Split( Colon );
        int lLeft = Convert.ToInt32( lBits[ 0 ] );

        int lRight;
        if ( lBits.Length > 1 )
          lRight = Convert.ToInt32( lBits[ 1 ] );
        else
          lRight = lLeft;

        lLeft -= ReadLength;
        if ( lLeft < 0 )
          lLeft = 0;

        if ( lRight > lReferenceSequence.Bases.Length )
        {
          Console.WriteLine( "Your Right Parameter is bigger than your referenge genome." );
          Console.Write( "> " );
          continue;
        }

        ICollection<Mapping> lMappings = lMappingReader.GetReads( lLeft, lRight );
        if ( lBits.Length > 2 ) // engage pretty mode
        {
          Console.WriteLine( lLeft );

          if ( lRight + 1 < lReferenceSequence.Bases.Length )
            Console.WriteLine( lReferenceSequence.Bases.Substring( lLeft, lRight - lLeft + 1 ) );
          else
            Console.WriteLine( lReferenceSequence.Bases.Substring( lLeft ) );

          foreach ( Mapping lMapping in lMappings )
          {
            Console.WriteLine( lMapping.Sequence.Bases.PadLeft( lMapping.Index - lLeft + lMapping.Sequence.Bases.Length ) );
          }
        }
        else
        {
          foreach ( Mapping lMapping in lMappings )
          {
            Console.WriteLine( string.Format( "Index {0}: {1}", lMapping.Index, lMapping.Sequence.Bases ) );
          }
        }

        Console.Write( "> " );
      }

      return;
    }

    private static bool IsValid( string lInput )
    {
      try
      {
        string[] lBits = lInput.Split( Colon );
        int lLeft = Convert.ToInt32( lBits[ 0 ] );

        int lRight;
        if ( lBits.Length > 1 )
          lRight = Convert.ToInt32( lBits[ 1 ] );
        else
          lRight = lLeft;
        
        if ( lLeft > lRight || lLeft < 0 || lRight < 0 )
          return false;
      }
      catch ( Exception )
      {
        return false;
      }

      return true;
    }

  }
}
