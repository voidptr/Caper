using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace CaperSharp
{
  class FASequenceEngine : SequenceEngine
  {
    readonly int GreaterThan;
    

    public FASequenceEngine( string aPath )
      : base( aPath )
    {
      GreaterThan = Convert.ToInt16( '>' );
      
    }

    internal override bool Initialize()
    {
      DecachedStreamReader lStream = new DecachedStreamReader( Path );

      string lLocus = "";
      long lLocusStart = 0;
      long lCount = 0;

      FileStream lFileStream = new FileStream( string.Format( "{0}.tmp", Path ), FileMode.Create, FileAccess.ReadWrite );

      StreamWriter lWriter = new StreamWriter( lFileStream );
      StreamReader lAllReader = new StreamReader( lFileStream );

      while ( lStream.Peek() > -1 )
      {
        string lLine = lStream.ReadLine();
        if ( lLine.Length > 0 )
        {
          if ( lLine[ 0 ] == '>' )
          {
            if ( lLocus.Length > 0 ) // close up the last one.
            {
              lWriter.Write( '\n' );
              Sequences.Add( lLocus, new Sequence( lAllReader.BaseStream, lLocusStart, lCount ) );
              lCount++;
            }

            lLocus = lLine.Substring( 1 ).Split( ',', ' ' )[ 0 ];
            lLocusStart += lCount; // the next line. (incl. \n)
            lCount = 0;
          }
          else
          {
            lWriter.Write( lLine );
            lCount += lLine.Length;
          }
        }
      }

      // do the last kone.
      Sequences.Add( lLocus, new Sequence( lAllReader.BaseStream, lLocusStart, lCount ) );

      lWriter.Flush();

      lStream.Close();

      return true;
    }
  }
}
