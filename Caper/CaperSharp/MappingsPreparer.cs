using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  public abstract class MappingsPreparer
  {
    private const char Tab = '\t';

    public string Path { get; private set; }

    public MappingsPreparer( string aPath )
    {
      Path = aPath;
    }

    public string Initialize()
    {
      if ( !IsSorted() )
        BreakUpFileIntoContigsSortAndRecombine();

      return string.Format( "{0}.SORTED", Path );
    }

    private bool IsSorted()
    {
      StreamReader lStream = new StreamReader( Path );

      string lLastContig = "";
      int lLastIndex = -1;

      while ( lStream.Peek() > -1 )
      {
        string lLine = lStream.ReadLine();

        string lContig = GetContigIdent( lLine );
        int lIndex = GetIndex( lLine );

        if ( lContig.CompareTo( lLastContig ) < 1 || lIndex < lLastIndex )
        {
          lStream.Close();
          return false;
        }

        lLastContig = lContig;
        lLastIndex = lIndex;
      }
      lStream.Close();
      return true;
    }

    private void BreakUpFileIntoContigsSortAndRecombine()
    {
      SortedDictionary<string, string> lList = GetContigsFromMappingsFile();

      SplitMappingFileIntoContigs( lList );

      SortMappingFiles( lList );

      CombineSortedMappingFiles( lList );
    }

    private void SplitMappingFileIntoContigs( SortedDictionary<string, string> lList )
    {
      StreamReader lStream = new StreamReader( Path );
      foreach ( string lContigKey in lList.Keys )
      {
        lStream.BaseStream.Seek( 0, SeekOrigin.Begin );
        lStream.DiscardBufferedData();

        StreamWriter lWriter = new StreamWriter(
          new FileStream( string.Format( "{0}.{1}", Path, lContigKey ), FileMode.Create, FileAccess.Write ) );

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
    }

    private void SortMappingFiles( SortedDictionary<string, string> lList )
    {
      foreach ( string lContigKey in lList.Keys )      
        SortFile( string.Format( "{0}.{1}", Path, lContigKey ) );
      
    }

    private void CombineSortedMappingFiles( SortedDictionary<string, string> lList )
    {
      FileStream lSortedOutputStream = new FileStream( string.Format( "{0}.SORTED", Path ), FileMode.Create, FileAccess.Write );
      StreamWriter lSortedOutputWriter = new StreamWriter( lSortedOutputStream );

      foreach ( string lContigKey in lList.Keys )
      {
        string lFileName = string.Format( "{0}.{1}", Path, lContigKey );

        StreamReader lReader = new StreamReader( lFileName );
        while ( lReader.Peek() > -1 )
        {
          lSortedOutputWriter.Write( lReader.ReadLine() );
          lSortedOutputWriter.Write( '\n' );
        }
        lReader.Close();
        File.Delete( lFileName );
      }
      lSortedOutputWriter.Close();
    }

    private SortedDictionary<string, string> GetContigsFromMappingsFile()
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

      lStream.Close();

      return lList;
    }

    public abstract string GetContigIdent( string aLine );

    private void SortFile( string aFile )
    {     
      List<string> lFile = ReadAllLines( aFile );
      

      int lStartPos = 0;
      int lLastLinePos = lFile.Count - 1;
      Introsort( lFile, lStartPos, lLastLinePos, 0, ( int ) Math.Floor( Math.Log( lFile.Count ) ) );
      
      WriteAllLines( aFile, lFile );      
    }

    private void WriteAllLines( string aPath, List<string> lFile )
    {
      StreamWriter lOutStream = new StreamWriter( aPath );
      foreach ( string lLine in lFile )
      {
        lOutStream.Write( lLine );
        lOutStream.Write( '\n' );
      }
      lOutStream.Close();
    }

    private List<string> ReadAllLines( string aFile )
    {
      StreamReader lStream = new StreamReader( aFile );
      List<string> lFile = new List<string>();
      while ( lStream.Peek() > -1 )
      {
        lFile.Add( lStream.ReadLine() );
      }
      lStream.Close();
      return lFile;
    }

    private void Introsort( List<string> aLines, int aFirstIndex, int aLastIndex, int aDepth, int aSwitchDepth )
    {
      if ( aLastIndex > aFirstIndex )
      {
        if ( ++aDepth < aSwitchDepth )
        {
          int lPivotNewIndex = Partition( aLines, aFirstIndex, aLastIndex, aLastIndex );

          Introsort( aLines, aFirstIndex, lPivotNewIndex - 1, aDepth, aSwitchDepth );
          Introsort( aLines, lPivotNewIndex + 1, aLastIndex, aDepth, aSwitchDepth );
        }
        else
        {
          Heapsort( aLines, aFirstIndex, aLastIndex - aFirstIndex + 1 );
        }
      }
    }

    private void Heapsort( List<string> aLines, int aOffset, int aCount )
    {
      Heapify( aLines, aOffset, aCount );

      int lEnd = aCount - 1;

      while ( lEnd > 0 )
      {
        Swap( aLines, lEnd + aOffset, aOffset );
        SiftDown( aLines, 0, lEnd - 1, aOffset );
        lEnd--;
      }
    }

    private void Heapify( List<string> aLines, int aOffset, int aCount )
    {
      int lStart = ( ( aCount - 2 ) / 2 );
      while ( lStart >= 0 )
      {
        SiftDown( aLines, lStart, aCount - 1, aOffset );
        lStart--;
      }
    }

    private void SiftDown( List<string> aLines, int aStart, int aEnd, int aOffset )
    {
      int lRoot = aStart;

      while ( ( lRoot * 2 ) + 1 <= aEnd )
      {
        int lChild = lRoot * 2 + 1;

        if ( lChild + 1 <= aEnd &&
          GetIndex( aLines[ lChild + aOffset ] ) < GetIndex( aLines[ lChild + 1 + aOffset ] ) )
        {
          lChild = lChild + 1;
        }

        if ( GetIndex( aLines[ lRoot + aOffset ] ) < GetIndex( aLines[ lChild + aOffset ] ) )
        {
          Swap( aLines, lRoot + aOffset, lChild + aOffset );
          lRoot = lChild;
        }
        else
          return;
      }
    }

    private int Partition( List<string> aLines, int aFirstIndex, int aLastIndex, int aPivotIndex )
    {
      string lPivotLine = aLines[ aPivotIndex ]; // fetch the pivot
      string lContig = GetContigIdent( lPivotLine );

      //Console.WriteLine( string.Format( "PARTITION!! {0} {1} {2} {3}", lContig, aFirstIndex, aLastIndex, aPivotIndex ) );

      int lStoreIndex = aFirstIndex;
      for ( int lIndex = aFirstIndex; lIndex < aLastIndex; lIndex++ ) // proceed from left to right, don't do the last one.
      {
        string lTestLine = aLines[ lIndex ];

        if ( LessThanOrEqual( lTestLine, lPivotLine ) )
        {
          Swap( aLines, lIndex, lStoreIndex );

          lStoreIndex++; // increment the store position
        }
      }
      Swap( aLines, lStoreIndex, aPivotIndex );
      return lStoreIndex; // return the final position of the pivot
    }

    private void Swap( List<string> aLines, int lIndex, int lStoreIndex )
    {
      if ( lIndex != lStoreIndex )
      {
        string lIndexLine = aLines[ lIndex ];
        aLines[ lIndex ] = aLines[ lStoreIndex ];
        aLines[ lStoreIndex ] = lIndexLine;
      }
    }

    private bool LessThanOrEqual( string aLine1, string aLine2 )
    {
      if ( GetIndex( aLine1 ) <= GetIndex( aLine2 ) )
        return true;

      return false;
    }

    public abstract int GetIndex( string aLine1 );

    public static string[] GetLinePieces( string aLine )
    {
      return aLine.Split( Tab );
    }

  }
}
