using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  class Program
  {
    const char Colon = ':';
    const int ReadLength = 36;
    const string UsageString = "CaperSharp v0.1\nUsage: caper <referencegenome.fa> <mapviewmappings.mapview>\n";
    const string Format = "Command Format: Contig:X:Y or Contig:X:Y:p (for pretty mode)";

    private struct Parameters
    {
      public string ContigIdent;
      public int Left;
      public int Right;
      public bool PrettyMode;
    }

    static void Main( string[] args )
    {
      if ( args.Length < 2 )
      {
        Console.Write( UsageString );
        return;
      }

      SequenceEngine lSequenceReader = new FASequenceEngine( args[ 0 ] );
      lSequenceReader.Initialize();

      MappingEngine lMappingReader = new MapviewMappingEngine( args[ 1 ], lSequenceReader.Sequences );
      lMappingReader.Initialize();

      Console.WriteLine( Format );
      Console.Write( "> " );

      string lInput = "";
      while ( ( lInput = Console.ReadLine() ).Length > 0 )
      {
        Parameters lCommand;
        try
        {
          lCommand = ParseCommand( lInput, lSequenceReader, lMappingReader );
        }
        catch ( Exception )
        {
          Console.WriteLine( string.Format( "Invalid Input. {0}", Format ) );
          Console.Write( "> " );
          continue;
        }
        
        ICollection<Mapping> lMappings = lMappingReader.GetReads( lCommand.ContigIdent, lCommand.Left, lCommand.Right );
        if ( lCommand.PrettyMode ) // engage pretty mode
        {
          Console.WriteLine( lCommand.Left );
          Console.WriteLine( lSequenceReader.Sequences[ lCommand.ContigIdent ]
            .Substring( lCommand.Left, lCommand.Right - lCommand.Left + 1 ) );

          foreach ( Mapping lMapping in lMappings )
          {
            string lSeqString = lMapping.Sequence.ToString();
            Console.WriteLine( lSeqString.PadLeft(
              lMapping.Index - lCommand.Left + lSeqString.Length ) );
          }
        }
        else
        {
          foreach ( Mapping lMapping in lMappings )
          {
            Console.WriteLine( string.Format( "Index {0}: {1}", lMapping.Index, lMapping.Sequence.ToString() ) );
          }
        }

        Console.Write( "> " );
      }

      return;
    }

    private static Parameters ParseCommand( string lInput, SequenceEngine lSequenceReader, MappingEngine lMappingReader )
    {
      Parameters lCommand = new Parameters();

      string[] lBits = lInput.Split( Colon );
      lCommand.ContigIdent = lBits[ 0 ];

      lCommand.Left = Convert.ToInt32( lBits[ 1 ] );
            
      if ( lBits.Length > 2 )
        lCommand.Right = Convert.ToInt32( lBits[ 2 ] );
      else
        lCommand.Right = lCommand.Left;

      if ( lBits.Length > 3 )
        lCommand.PrettyMode = true;
      else
        lCommand.PrettyMode = false;
      
      if ( lCommand.Left > lCommand.Right || lCommand.Left < 0 || lCommand.Right < 0 )
        throw new Exception( "Invalid values." );

      lCommand.Left -= lMappingReader.ReadLength;
      if ( lCommand.Left < 0 )
        lCommand.Left = 0;
      
      if ( lCommand.Right > lSequenceReader.Sequences[ lCommand.ContigIdent ].Length )      
        throw new Exception( "Your Right Parameter is bigger than your indexed contig." );        
        
      return lCommand;
    }
  }
}
