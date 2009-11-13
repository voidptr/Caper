using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  class Program
  {
    const char Colon = ':';
    const int ReadLength = 36;
    const string UsageString = "CaperSharp v0.1\nUsage: caper [-si] -g <referencegenome.fa> <-m|-b> <mappingsfile>\n";
    const string Format = "You can type: \"help\", \"list\", \"info <contig ident>\", \"quit\", \"<contig ident>:<X>:<Y>\", or \"<contig ident>:<X>:<Y>:p\" (for pretty mode)";

    private enum Command
    {
      Help,
      GetReads,
      ListContigs,
      ContigInfo,
      Save,
      Quit,
    }

    private struct Parameters
    {
      public string ContigIdent;
      public int Left;
      public int Right;
      public bool PrettyMode;
      public Command Command;
    }

    static void Main( string[] args )
    {
      if ( args.Length < 3 )
      {
        Console.Write( UsageString );
        return;
      }

      Console.Write( "Reading Genome... " );

      SequenceEngine lSequenceReader = new FASequenceEngine( args[ 1 ] );
      lSequenceReader.Initialize();

      Console.WriteLine( "Done!" );

      MappingEngine lMappingEngine;

      if ( args[ 0 ] == "-b" )
      {
        Console.Write( "Preparing Bowtie Mappings... " );
        MappingsPreparer lPrep = new BowtieMappingsPreparer( args[ 2 ] );
        string lNewPath = lPrep.Initialize();
        lMappingEngine = new BowtieMappingEngine( lNewPath, lSequenceReader.Sequences );
      }
      else if ( args[ 0 ] == "-m" )
      {
        Console.Write( "Preparing Mapview Mappings... " );
        MappingsPreparer lPrep = new MapviewMappingsPreparer( args[ 2 ] );
        string lNewPath = lPrep.Initialize();
        lMappingEngine = new MapviewMappingEngine( lNewPath, lSequenceReader.Sequences );
      }
      else
      {
        Console.WriteLine( "Error: No mapping format was selected. (-b|-m)." );
        Console.WriteLine( UsageString );
        return;
      }
      Console.WriteLine( "Done!" );

      Console.Write( "Initializing Mapping Engine... " );
      lMappingEngine.Initialize();
      Console.WriteLine( "Done!" );
      Console.WriteLine();

      Console.WriteLine( Format );
      Console.Write( "> " );

      string lInput = "";
      while ( true )
      {
        lInput = Console.ReadLine();

        Parameters lCommand;
        try
        {
          lCommand = ParseCommand( lInput, lSequenceReader, lMappingEngine );
        }
        catch ( Exception )
        {
          Console.WriteLine( string.Format( "Invalid Input. {0}", Format ) );
          Console.Write( "> " );
          continue;
        }

        if ( lCommand.Command == Command.Help )
        {
          Console.WriteLine( Format );
        }
        else if ( lCommand.Command == Command.Quit )
        {
          Console.WriteLine( "Bye!" );
          return;
        }
        else if ( lCommand.Command == Command.ListContigs )
        {
          foreach ( string lKey in lSequenceReader.Sequences.Keys )
          {
            int lNumberOfReads = 0;
            if ( lMappingEngine.NumberOfReads.ContainsKey( lKey ) )
              lNumberOfReads = lMappingEngine.NumberOfReads[ lKey ];
            Console.WriteLine( string.Format( "{0} - {1}bp, {2} reads", lKey, lSequenceReader.Sequences[ lKey ].Length, lNumberOfReads ) );
          }
        }
        else if ( lCommand.Command == Command.ContigInfo )
        {
          int lNumberOfReads = 0;
          if ( lMappingEngine.NumberOfReads.ContainsKey( lCommand.ContigIdent ) )
            lNumberOfReads = lMappingEngine.NumberOfReads[ lCommand.ContigIdent ];
          Console.WriteLine( string.Format( "{0} - {1}bp, {2} reads", lCommand.ContigIdent, lSequenceReader.Sequences[ lCommand.ContigIdent ].Length, lNumberOfReads ) );
        }
        else if ( lCommand.Command == Command.Save )
        {
          Console.WriteLine( "This feature is not yet available." );
        }
        else if ( lCommand.Command == Command.GetReads )
        {
          ICollection<Mapping> lMappings = lMappingEngine.GetReads( lCommand.ContigIdent, lCommand.Left, lCommand.Right );
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
        }

        Console.Write( "> " );
      }

      return;
    }

    private static Parameters ParseCommand( string lInput, SequenceEngine lSequenceReader, MappingEngine lMappingReader )
    {
      Parameters lCommand = new Parameters();

      if ( lInput == "help" )
        lCommand.Command = Command.Help;
      else if ( lInput == "list" )
        lCommand.Command = Command.ListContigs;
      else if ( lInput == "save" )
        lCommand.Command = Command.Save;
      else if ( lInput == "quit" || lInput == "exit" )
        lCommand.Command = Command.Quit;
      else if ( lInput.Split( ' ' ).Length == 2 && lInput.Split( ' ' )[ 0 ] == "info" )
      {
        lCommand.Command = Command.ContigInfo;
        lCommand.ContigIdent = lInput.Split( ' ' )[ 1 ];

        if ( !lSequenceReader.Sequences.ContainsKey( lCommand.ContigIdent ) )
          throw new Exception( "Invalid Contig ID." );
      }
      else
      {
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
      }
      return lCommand;
    }
  }
}
