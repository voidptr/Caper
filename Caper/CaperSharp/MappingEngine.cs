using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  abstract class MappingEngine
  {
    public int ReadLength;

    public string Path { get; private set; }
    private Sequences ReferenceGenome { get; set; }

    const int IndexIncrement = 1000;
    private const char Tab = '\t';
    private const char NewLine = '\n';
    private long mEndOfFilePos;

    private MappingCache CacheA;
    private MappingCache CacheB;

    private DecachedStreamReader mStream;

    private Dictionary<string, List<long>> mFilePositions;
    private Dictionary<string, int> mNumberOfWindows;
    private Dictionary<string, Pair<long>> mContigBorders;
    private Dictionary<string, int> mSortedContigIdents;

    public Dictionary<string, int> NumberOfReads;

    public MappingEngine( string aPath, Sequences aReferenceGenome )
    {
      ReferenceGenome = aReferenceGenome;
      Path = aPath;

      mNumberOfWindows = new Dictionary<string, int>();
      mFilePositions = new Dictionary<string, List<long>>();
      mContigBorders = new Dictionary<string, Pair<long>>();
      mSortedContigIdents = new Dictionary<string, int>();

      NumberOfReads = new Dictionary<string, int>();

      mEndOfFilePos = 0;
    }

    /// <summary>
    /// This assumes that all the mappings are sorted by contig ident and by index.
    /// </summary>
    private void PrepareFilePositions()
    {
      int i = 0;
      foreach ( string lContigIdent in ReferenceGenome.Keys )
        mSortedContigIdents.Add( lContigIdent, i++ );

      mContigBorders = FindContigBorders();

      mFilePositions = BuildMappingsIndex();
    }

    private Dictionary<string, List<long>> BuildMappingsIndex()
    {
      DecachedStreamReader lReader = new DecachedStreamReader( Path );

      Dictionary<string, List<long>> lIndexes = new Dictionary<string, List<long>>();

      string lContig = "";
      long lCurrentPosition = 0;
      int lTargetIndex = 0;
      int lReadCount = 0;

      while ( lReader.Peek() > -1 )
      {
        lCurrentPosition = lReader.Position;

        string lLine = lReader.ReadLine();
        string lCurrentContig = GetContigIdent( lLine );
        int lCurrentIndex = GetIndex( lLine );

        if ( lContig != lCurrentContig )
        {
          if ( lContig.Length > 0 )
          {
            NumberOfReads.Add( lContig, lReadCount );
            lReadCount = 0;
          }

          lTargetIndex = 0;
          lContig = lCurrentContig;
          lIndexes.Add( lContig, new List<long>() );
        }

        if ( lCurrentIndex >= lTargetIndex )
        {
          if ( lCurrentIndex >= lTargetIndex + IndexIncrement ) // overshoot
          {
            lIndexes[ lContig ].Add( -1 ); // indicate that this section is empty.
            lReader.Position = lCurrentPosition; // unconsume the line, since we'll need it in a minute.
          }
          else
            lIndexes[ lContig ].Add( lCurrentPosition );

          lTargetIndex += IndexIncrement;
        }
        lReadCount++;
      }
      NumberOfReads.Add( lContig, lReadCount ); // last one.

      lReader.Close();

      return lIndexes;
    }

    private Dictionary<string, Pair<long>> FindContigBorders()
    {
      DecachedStreamReader lReader = new DecachedStreamReader( Path );

      Dictionary<string, Pair<long>> lBorders = new Dictionary<string, Pair<long>>();

      string lContig = "";
      
      long lCurrentLinePosition = 0;
      while ( lReader.Peek() > -1 )
      {
        
        lCurrentLinePosition = lReader.Position;
        string lCurrentContig = GetContigIdent( lReader.ReadLine() );

        if ( lCurrentContig != lContig )
        {
          if ( lContig.Length > 0 ) // close up the previous one.
          {
            Pair<long> lItem = lBorders[ lContig ];
            lItem.SecondItem = lCurrentLinePosition - 1; // index of last char of section
            lBorders[ lContig ] = lItem;
          }
          lBorders[ lCurrentContig ] = new Pair<long>() { FirstItem = lCurrentLinePosition };
          lContig = lCurrentContig;
        }
      }
      // close up the last contig

      Pair<long> lLastContig = lBorders[ lContig ];
      lLastContig.SecondItem = lReader.Position - 1; // index of last char of file
      lBorders[ lContig ] = lLastContig;

      lReader.Close();

      return lBorders;
    }

    private void GenerateNumberOfWindows()
    {
      foreach ( string lContigIdent in ReferenceGenome.Keys )
      {
        mNumberOfWindows.Add( lContigIdent,
         ( int ) Math.Ceiling( ( double ) ReferenceGenome[ lContigIdent ].Length / IndexIncrement ) );
      }
    }

    internal virtual bool Initialize()
    {
      mStream = new DecachedStreamReader( Path );
      InitFileInformation( mStream );
      GenerateNumberOfWindows();
      PrepareFilePositions();

      return true; // test whether the thing finished or not.
    }

    private void InitFileInformation( DecachedStreamReader lStream )
    {
      string lLine = lStream.ReadLine();
      ReadLength = GetSequence( lLine ).Length;

      lStream.BaseStream.Seek( 0, SeekOrigin.End );
      mEndOfFilePos = lStream.Position;
      lStream.Position = 0;
    }

    internal virtual ICollection<Mapping> GetReads( string lContigIdent, int aLeft, int aRight )
    {
      List<Mapping> lResult = new List<Mapping>();

      int lStartingCache = aLeft / IndexIncrement;
      int lEndingCache = aRight / IndexIncrement;
      int lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
      int lLeftPosition = aLeft; // starting position

      int lCurrentCachePosition = lStartingCache;
      for ( int i = 0; i < lCachesRequired; i++ ) // loop through the total number of caches required.
      {
        int lRightPosition = aRight < ( lCurrentCachePosition + 1 ) * IndexIncrement ?
          aRight : ( ( lCurrentCachePosition + 1 ) * IndexIncrement ) - 1;

        MappingCache lAppropriateCache = GetCorrectCache( lContigIdent, lLeftPosition, lRightPosition );
        lResult.AddRange( lAppropriateCache.GetReads( lLeftPosition, lRightPosition ) );

        // these only matter if we do end up going to the next cache.
        lCurrentCachePosition++;
        lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
      }

      return lResult;
    }

    internal MappingCache GetCorrectCache( string aContigIdent, int aLeft, int aRight )
    {
      if ( CacheA != null && CacheA.ContigIdent == aContigIdent && CacheA.LeftIndex <= aLeft && CacheA.RightIndex >= aLeft )
        return CacheA;
      else if ( CacheB != null && CacheB.ContigIdent == aContigIdent && CacheB.LeftIndex <= aLeft && CacheB.RightIndex >= aLeft )
        return CacheB;
      else
      {
        RebuildCaches( aContigIdent, aLeft ); // neither of them have it.
        return GetCorrectCache( aContigIdent, aLeft, aRight ); // check again.
      }
    }

    private void RebuildCaches( string aContigIdent, int aLeft ) // these define the left edge of the problem.
    {
      int lStartingIndex = ( aLeft / IndexIncrement );

      CacheA = RebuildCache( aContigIdent, lStartingIndex );

      if ( lStartingIndex + 1 < mFilePositions.Count )
        CacheB = RebuildCache( aContigIdent, lStartingIndex + 1 );
      else
        CacheB = null;

    }

    private MappingCache RebuildCache( string aContigIdent, int lStartingIndex )
    {
      long lStartingPos = mFilePositions[ aContigIdent ][ lStartingIndex ];

      if ( lStartingPos == -1 ) // there's no cache here
        return BuildEmptyCache( aContigIdent, lStartingIndex * IndexIncrement, ( lStartingIndex * IndexIncrement ) + IndexIncrement - 1 );

      long lCount = -1;
      if ( lStartingIndex + 1 < mFilePositions[ aContigIdent ].Count )
        for ( int i = lStartingIndex + 1; i < mFilePositions[ aContigIdent ].Count; i++ )
          if ( mFilePositions[ aContigIdent ][ i ] > -1 )
          {
            lCount = mFilePositions[ aContigIdent ][ i ] - lStartingPos;
            break;
          }

      if ( lCount == -1 ) // it's at the edge of the contig!
        lCount = mContigBorders[ aContigIdent ].SecondItem - lStartingPos + 1;

      byte[] lBlock = new byte[ lCount ];
      mStream.Position = lStartingPos;
      mStream.BaseStream.Read( lBlock, 0, ( int ) lCount );

      MappingCache lCache = BuildCache( lBlock, aContigIdent, lStartingIndex * IndexIncrement, ( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );

      return lCache;
    }

    private MappingCache BuildEmptyCache( string aContigIdent, int aLeft, int aRight )
    {
      List<List<Mapping>> lMappings = new List<List<Mapping>>();
      for ( int i = aLeft; i <= aRight; i++ )
      {
        lMappings.Add( null );
      }

      return new MappingCache( lMappings, aContigIdent, aLeft, aRight );
    }


    public virtual MappingCache BuildCache( byte[] lBlock, string aContigIdent, int aLeft, int aRight )
    {
      MappingCache lCache = BuildEmptyCache( aContigIdent, aLeft, aRight );

      StreamReader lStream = new StreamReader( new MemoryStream( lBlock ) );

      while ( lStream.Peek() >= 0 )
      {
        string lLine = lStream.ReadLine();

        int lIndex = GetIndex( lLine );
        int lPrivateIndex = lIndex - aLeft;

        if ( lCache.Sequences[ lPrivateIndex ] == null )
          lCache.Sequences[ lPrivateIndex ] = new List<Mapping>();

        lCache.Sequences[ lPrivateIndex ].Add( new Mapping( lIndex, new Sequence( GetSequence( lLine ) ) ) );
      }

      return lCache;
    }

    public abstract string GetSequence( string aLine );

    public abstract int GetIndex( string aLine );

    public abstract string GetContigIdent( string aLine );

    public static string[] GetLinePieces( string aLine )
    {
      return aLine.Split( Tab );
    }

    internal void SaveReadIndexToFile()
    {
      throw new NotImplementedException();
    }
  }
}
