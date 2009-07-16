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
      throw new NotImplementedException();
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

    public abstract string GetContigIdent( string aLine );

    private void SortFile( string aFile )
    {      
      StreamReader lStream = new DecoupledStreamReader( aFile );

      List<string> lFile = ReadAllLines( lStream );

      int lStartPos = 0;
      int lLastLinePos = lFile.Count;

      Quicksort( lFile, lStartPos, lLastLinePos );
    }

    private List<string> ReadAllLines( StreamReader lStream )
    {
      List<string> lFile = new List<string>();
      while ( lStream.Peek() > -1 )
      {
        lFile.Add( lStream.ReadLine() );        
      }
      return lFile;
    }

    private void Quicksort( List<string> aLines, int aFirstIndex, int aLastIndex )
    {
      if ( aLastIndex > aFirstIndex )
      {
        int lPivotNewIndex = Partition( aLines, aFirstIndex, aLastIndex, aLastIndex );
        
        Quicksort( aLines, aFirstIndex, lPivotNewIndex - 1 );
        Quicksort( aLines, lPivotNewIndex + 1, aLastIndex );
      }
    }

    private int Partition( List<string> aLines, int aFirstIndex, int aLastIndex, int aPivotIndex )
    {      
      string lPivotLine = aLines[ aPivotIndex ]; // fetch the pivot

      int lStoreIndex = aFirstIndex;
      for ( int lIndex = aFirstIndex; lIndex < aLastIndex; lIndex++ ) // proceed from left to right, don't do the last one.
      {
        string lTestLine = aLines[ lIndex ];        

        if ( LessThanOrEqual( lTestLine, lPivotLine ) )
        {
          aLines[ lIndex ] = aLines[ lStoreIndex ];
          aLines[ lStoreIndex ] = lTestLine;

          lStoreIndex++; // increment the store position
        }
      }

      aLines[ aPivotIndex ] = aLines[ lStoreIndex ];
      aLines[ lStoreIndex ] = lPivotLine;

      return lStoreIndex; // return the final position of the pivot
    }


    private bool LessThanOrEqual( string aLine1, string aLine2 )
    {
      int lIndex1 = GetIndex( aLine1 );
      int lIndex2 = GetIndex( aLine1 );

      if ( lIndex1 <= lIndex2 )
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
