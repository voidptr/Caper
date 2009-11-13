using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  public abstract class MappingsPreparer
  {
    private const char Tab = '\t';
    private const char NewLine = '\n';

    public string Path { get; private set; }

    public MappingsPreparer( string aPath )
    {
      Path = aPath;
    }

    public string Initialize()
    {
      if ( !IsSorted() )
        //return SortMappings();
        return SortMappingsInMemory();

      return Path;
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

        if ( lContig.CompareTo( lLastContig ) < 0 || ( lContig == lLastContig && lIndex < lLastIndex ) )
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

    private string SortMappingsInMemory()
    {
      StreamReader lStream = new StreamReader( Path );
      
      SortedDictionary<string, List<string>> lCompleteFile = new SortedDictionary<string, List<string>>();
      while ( lStream.Peek() > -1 )
      {
        string lLine = lStream.ReadLine();
        string lContig = GetContigIdent( lLine );
        if ( !lCompleteFile.ContainsKey( lContig ) )
          lCompleteFile.Add( lContig, new List<string>() );

        lCompleteFile[ lContig ].Add( lLine );
      }
      lStream.Close();

      string lSortedOutputFile = string.Format( "{0}.SORTED", Path );
      StreamWriter lSortedOutputWriter = new StreamWriter( new FileStream( lSortedOutputFile, FileMode.Create, FileAccess.Write ) );

      foreach ( string lContig in lCompleteFile.Keys )
      {
        Introsort( lCompleteFile[ lContig ], 0, lCompleteFile[ lContig ].Count - 1, 0, ( int ) Math.Floor( Math.Log( lCompleteFile[ lContig ].Count ) ) );

        foreach ( string lLine in lCompleteFile[ lContig ] )
        {
          lSortedOutputWriter.Write( lLine );
          lSortedOutputWriter.Write( NewLine );
        }
      }

      lSortedOutputWriter.Close();

      return lSortedOutputFile;
    }

    private string SortMappings()
    {
      SortedDictionary<string, string> lList = GetContigsFromMappingsFile();

      string lSortedOutputFile = string.Format( "{0}.SORTED", Path );
      StreamWriter lSortedOutputWriter = new StreamWriter( new FileStream( lSortedOutputFile, FileMode.Create, FileAccess.Write ) );

      StreamReader lStream = new StreamReader( Path );
      foreach ( string lContigKey in lList.Keys )
      {
        lStream.BaseStream.Seek( 0, SeekOrigin.Begin );
        lStream.DiscardBufferedData();

        List<string> lLines = new List<string>();
        while ( lStream.Peek() > -1 )
        {
          string lLine = lStream.ReadLine();
          if ( GetContigIdent( lLine ) == lContigKey )
            lLines.Add( lLine );
        }

        Introsort( lLines, 0, lLines.Count - 1, 0, ( int ) Math.Floor( Math.Log( lLines.Count ) ) );

        foreach ( string lLine in lLines )
        {
          lSortedOutputWriter.Write( lLine );
          lSortedOutputWriter.Write( NewLine );
        }
      }
      lStream.Close();

      lSortedOutputWriter.Close();

      return lSortedOutputFile;
    }


    private SortedDictionary<string, string> GetContigsFromMappingsFile()
    {
      StreamReader lStream = new StreamReader( Path );

      SortedDictionary<string, string> lList = new SortedDictionary<string, string>();
      while ( lStream.Peek() > -1 )
      {
        string lContig = GetContigIdent( lStream.ReadLine() );
        if ( !lList.ContainsKey( lContig ) )
          lList.Add( lContig, lContig );
      }

      lStream.Close();

      return lList;
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

    private int Partition( List<string> aLines, int aFirstIndex, int aLastIndex, int aPivotIndex )
    {
      string lPivotLine = aLines[ aPivotIndex ]; // fetch the pivot
      string lContig = GetContigIdent( lPivotLine );

      int lStoreIndex = aFirstIndex;
      for ( int lIndex = aFirstIndex; lIndex < aLastIndex; lIndex++ ) // proceed from left to right, don't do the last one.
      {
        string lTestLine = aLines[ lIndex ];

        if ( GetIndex( lTestLine ) <= GetIndex( lPivotLine ) )
        {
          Swap( aLines, lIndex, lStoreIndex );

          lStoreIndex++; // increment the store position
        }
      }
      Swap( aLines, lStoreIndex, aPivotIndex );
      return lStoreIndex; // return the final position of the pivot
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

    private void Swap( List<string> aLines, int lIndex, int lStoreIndex )
    {
      if ( lIndex != lStoreIndex )
      {
        string lIndexLine = aLines[ lIndex ];
        aLines[ lIndex ] = aLines[ lStoreIndex ];
        aLines[ lStoreIndex ] = lIndexLine;
      }
    }

    public abstract int GetIndex( string aLine1 );

    public abstract string GetContigIdent( string aLine );

    public static string[] GetLinePieces( string aLine )
    {
      return aLine.Split( Tab );
    }

  }
}
