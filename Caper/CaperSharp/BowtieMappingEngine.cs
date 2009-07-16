using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class BowtieMappingEngine : MappingEngine
  {
    readonly byte NewLine;
    readonly int LineIshLength;
    const int NewLineLength = 1;

    public BowtieMappingEngine( string aPath, Sequences aReferenceGenome )
      : base( aPath, aReferenceGenome )
    {
      NewLine = Convert.ToByte( '\n' );

      DecoupledStreamReader lStream = new DecoupledStreamReader( Path );
      lStream.Seek( 0, SeekOrigin.Begin );
      LineIshLength = lStream.ReadLine().Length;
      lStream.Close();
    }
    internal override bool Initialize()
    {
      BreakUpFileIntoContigsSortAndRecombine();

      return base.Initialize();
    }

    private void BreakUpFileIntoContigsSortAndRecombine()
    {
      StreamReader lStream = new StreamReader( Path );
      SortedDictionary<string, string> lList = new SortedDictionary<string, string>();

      while ( lStream.Peek() > -1 )
      {
        string lContig = GetContigIdent( lStream.ReadLine() );
        if ( !lList.ContainsKey( lContig ) )
        {
          lList.Add( lContig, lContig );
        }
      }

      foreach ( string lContigKey in lList.Keys )
      {
        FileStream lFileStream = new FileStream( string.Format( "{0}.{1}", Path, lContigKey ), FileMode.Create, FileAccess.Write );
        StreamWriter lWriter = new StreamWriter( lFileStream );

        lStream.BaseStream.Seek( 0, SeekOrigin.Begin );
        lStream.DiscardBufferedData();

        while ( lStream.Peek() > -1 )
        {
          string lLine = lStream.ReadLine();
          if ( GetContigIdent( lLine ) == lContigKey )
          {
            lWriter.Write( lLine );
            lWriter.Write( '\n' );
          }
        }
        lWriter.Close();
      }
      lStream.Close();

      foreach ( string lContigKey in lList.Keys )
      {
        SortFile( string.Format( "{0}.{1}", Path, lContigKey ) );
      }

      FileStream lSortedOutputStream = new FileStream( string.Format( "{0}.SORTED", Path ), FileMode.CreateNew, FileAccess.Write );
      StreamWriter lSortedOutputWriter = new StreamWriter( lSortedOutputStream );

      foreach ( string lContigKey in lList.Keys )
      {
        StreamReader lReader = new StreamReader( string.Format( "{0}.{1}", Path, lContigKey ) );
        while ( lReader.Peek() > -1 )
        {
          lSortedOutputWriter.Write( lReader.ReadLine() );
          lSortedOutputWriter.Write( '\n' );
        }
        lReader.Close();
      }

      lSortedOutputWriter.Close();

    }

    private void SortFile( string lFile )
    {
      FileStream lFileStream = new FileStream( lFile, FileMode.Open, FileAccess.ReadWrite );
      DecoupledStreamReader lStream = new DecoupledStreamReader( lFileStream );

      long lStartPos = 0;
      long lLastLinePos = FindPreviousLine( lStream, lStream.BaseStream.Length );

      Quicksort( lStream, lStartPos, lLastLinePos );
    }

    private void Quicksort( DecoupledStreamReader lStream, long lStartPos, long lLastLinePos )
    {
      if ( lLastLinePos > lStartPos )
      {
        lStream.Seek( lLastLinePos, SeekOrigin.Begin );
        lStream.ReadLine();
        long lNextLineAfterEnd = lStream.Position;

        long lPivotIndex = lLastLinePos; // the pivot is assigned as the rightmost line.
        long lPivotNewIndex = Partition( lStream, lStartPos, lLastLinePos, lPivotIndex );

        long lLinePreviousToPivot = FindPreviousLine( lStream, lPivotNewIndex );

        long lLineAfterPivot = FindNextLine( lStream, lPivotNewIndex );

        long lNewLastLinePos = FindPreviousLine( lStream, lNextLineAfterEnd );

        Quicksort( lStream, lStartPos, lLinePreviousToPivot );
        Quicksort( lStream, lLineAfterPivot, lNewLastLinePos );
      }
    }

    private long FindNextLine( DecoupledStreamReader lStream, long aLinePosition )
    {
      lStream.Seek( aLinePosition, SeekOrigin.Begin );
      lStream.ReadLine();
      return lStream.Position;
    }

    private long FindPreviousLine( DecoupledStreamReader lStream, long aLinePosition )
    {
      if ( aLinePosition == 0 )
        return 0;

      long lBackupAttemptPos = aLinePosition - ( LineIshLength * 3 );
      if ( lBackupAttemptPos < 0 )
        lBackupAttemptPos = 0;

      lStream.Seek( lBackupAttemptPos, SeekOrigin.Begin ); // 3ish from the end of the line we're in.
      lStream.ReadLine(); // advance to next start. // now 2ish from the end

      if ( lStream.Position >= aLinePosition ) // we arrived at our starting point by reading one line, so we must be at the wall
        return 0; // return the previous line, which is wall.

      long lPreviousLinePosition = lBackupAttemptPos;
      //long lPrevPrevLinePos = lBackupAttemptPos;

      string lLine = "";

      while ( lStream.Position != aLinePosition )
      {
        //lPrevPrevLinePos = lPreviousLinePosition;
        lPreviousLinePosition = lStream.Position;
        lLine = lStream.ReadLine();
      }

      //if ( lLine.Length == 0 )
      //  return lPrevPrevLinePos;

      return lPreviousLinePosition;
    }

    private long Partition( DecoupledStreamReader aStream, long aStartPos, long aLastLinePos, long aPivotPosition )
    {
      aStream.Seek( aPivotPosition, SeekOrigin.Begin ); // go to the beginning of the last line.
      string lPivotLine = aStream.ReadLine(); // fetch the pivot

      long lStorePos = aStartPos;

      for ( long lIndex = aStartPos; lIndex < aLastLinePos; ) // proceed from left to right, don't do the last one.
      {
        aStream.Seek( lIndex, SeekOrigin.Begin );
        string lTestLine = aStream.ReadLine();
        long lTestLineIndex = lIndex; // keep this around.
        lIndex = aStream.Position; // increment lIndex

        if ( LessThanOrEqual( lTestLine, lPivotLine ) )
        {
          SwapLines( aStream, lTestLineIndex, lStorePos ); // perform the swap
          lStorePos = FindNextLine( aStream, lStorePos ); // increment the store position
        }
      }
      SwapLines( aStream, lStorePos, aPivotPosition ); // move the pivot to the right place.
      return lStorePos; // return the final position of the pivot
    }

    private void SwapLines( DecoupledStreamReader lStream, long aLine1Pos, long aLine2Pos )
    {
      // TODO, fix the thing that is very wrong. :(

      if ( aLine1Pos == aLine2Pos )
        return;
      else if ( aLine1Pos > aLine2Pos )
      {
        SwapLines( lStream, aLine2Pos, aLine1Pos ); // switch 'em around.
        return;
      }

      lStream.Seek( aLine1Pos, SeekOrigin.Begin );
      string lLine1 = lStream.ReadLine();

      lStream.Seek( aLine2Pos, SeekOrigin.Begin );
      string lLine2 = lStream.ReadLine();
      long lEndOfSegment = lStream.Position;

      if ( lLine1.Length == lLine2.Length ) // They're the same size!
      {
        lStream.Seek( aLine2Pos, SeekOrigin.Begin );
        lStream.Write( CopyCharsToBytes( lLine1.ToCharArray() ), 0, lLine1.Length ); // newlines should already be present

        lStream.Seek( aLine1Pos, SeekOrigin.Begin );
        lStream.Write( CopyCharsToBytes( lLine2.ToCharArray() ), 0, lLine2.Length ); // newlines should already be present
      }
      else
      {
        byte[] lBuffer = new byte[ lEndOfSegment - aLine1Pos ];

        CopyCharsToBytes( lLine2.ToCharArray() ).CopyTo( lBuffer, 0 );
        lBuffer[ lLine2.Length ] = NewLine; // add newline

        lStream.Seek( aLine1Pos + lLine1.Length + NewLineLength, SeekOrigin.Begin );
        lStream.Read( lBuffer,
          lLine2.Length + NewLineLength,
          lBuffer.Length - lLine2.Length - NewLineLength - lLine1.Length - NewLineLength );

        CopyCharsToBytes( lLine1.ToCharArray() ).CopyTo( lBuffer, lBuffer.Length - lLine1.Length - NewLineLength );
        lBuffer[ lBuffer.Length - 1 ] = NewLine;

        lStream.Seek( aLine1Pos, SeekOrigin.Begin );

        char[] lBleh = CopyBytesToChars( lBuffer );
        string lString = new string( lBleh );


        lStream.Write( lBuffer, 0, lBuffer.Length );
      }
      lStream.Flush();

      // DEBUGGING PURPOSES
      //lStream.Close();
      //lStream = new DecoupledStreamReader( new FileStream( Path, FileMode.Open, FileAccess.ReadWrite ) );

      ;
    }

    private char[] CopyBytesToChars( byte[] aByteArray )
    {
      char[] lCharArray = new char[ aByteArray.Length ];

      for ( int i = 0; i < aByteArray.Length; i++ )
      {
        lCharArray[ i ] = ( char ) aByteArray[ i ];
      }

      return lCharArray;
    }

    private byte[] CopyCharsToBytes( char[] aCharArray )
    {
      byte[] lByteArray = new byte[ aCharArray.Length ];

      for ( int i = 0; i < aCharArray.Length; i++ )
      {
        lByteArray[ i ] = ( byte ) aCharArray[ i ];
      }

      return lByteArray;
    }

    private bool LessThanOrEqual( string aLine1, string aLine2 )
    {
      //string lContigIdent1 = GetContigIdent( aLine1 );
      //string lContigIdent2 = GetContigIdent( aLine2 );
      //int lValue = lContigIdent1.CompareTo( lContigIdent2 );

      //if ( lValue == -1 ) // definite less than
      //  return true;
      //else if ( lValue == 1 ) // definite greater than.
      //  return false;

      int lIndex1 = GetIndex( aLine1 );
      int lIndex2 = GetIndex( aLine1 );

      if ( lIndex1 <= lIndex2 )
        return true;

      return false;
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
      return System.Convert.ToInt32( aValue ) - 1;
    }

    public override string GetContigIdent( string aLine )
    {
      return GetLinePieces( aLine )[ 2 ];
    }
  }
}
