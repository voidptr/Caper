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
    const int StartingJumpIncrement = 10000;
    private const char Tab = '\t';
    private int mLineLengthIsh;
    private long mEndOfFilePos;

    private MappingCache CacheA;
    private MappingCache CacheB;

    private DecoupledStreamReader mStream;

    private Dictionary<string, List<long>> mFilePositions;
    private Dictionary<string, int> mNumberOfWindows;


    private enum Direction
    {
      JumpForward,
      JumpBackward
    }

    public MappingEngine( string aPath, Sequences aReferenceGenome )
    {
      ReferenceGenome = aReferenceGenome;
      Path = aPath;

      mNumberOfWindows = new Dictionary<string, int>();
      mFilePositions = new Dictionary<string, List<long>>();

      mLineLengthIsh = 0;
      mEndOfFilePos = 0;
    }

    /// <summary>
    /// This assumes that all the mappings are sorted by contig ident and by index.
    /// </summary>
    private void PrepareFilePositions()
    {
      int lEndOfPrevious = 0;
      foreach ( string lContigIdent in ReferenceGenome.Keys )
      {
        mFilePositions.Add( lContigIdent,
          new List<long>( ( int ) ReferenceGenome[ lContigIdent ].Length ) );

        mFilePositions[ lContigIdent ].Add( lEndOfPrevious ); // the new one starts where the previous left off

        for ( int i = 1 ; i < mNumberOfWindows[ lContigIdent ] ; i++ )
        {
          // start by setting a thousand line jump forward
          long lPos = FindNextIndexStart(
            mStream,
            lContigIdent,
            i * IndexIncrement,
            StartingJumpIncrement,
            Direction.JumpForward );

          if ( lPos != -1 )
            mFilePositions[ lContigIdent ].Add( lPos );
          else
            break; // no more targets to be found.
        }
      }
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
      mStream = new DecoupledStreamReader( Path );
      InitFileInformation( mStream );
      GenerateNumberOfWindows();
      PrepareFilePositions();

      return true; // test whether the thing finished or not.

    }

    private void InitFileInformation( DecoupledStreamReader lStream )
    {
      string lLine = lStream.ReadLine();
      mLineLengthIsh = lLine.Length;
      ReadLength = GetSequence( lLine ).Length;

      lStream.BaseStream.Seek( 0, SeekOrigin.End );
      mEndOfFilePos = lStream.Position;
      lStream.Position = 0;
    }

    private long FindNextIndexStart(
      DecoupledStreamReader aStream,
      string aTargetContigIdent,
      int aTargetIndex,
      int aIncrement,
      Direction aDirection)
    {
      long lPreJumpPosition = aStream.Position;

      // execute the jump (as directed)      
      if ( aDirection == Direction.JumpForward )
        SafeSeek( aStream, aStream.Position + aIncrement );
      else
        SafeSeek( aStream, aStream.Position - aIncrement );

      // Where are we?
      aStream.ReadLine(); // go to the start of the next line to clear things up. (this behavior affects the whole algorithm. keep in mind)
      long lPostJumpPos = aStream.Position; // where is this new line?

      string lOrientationLine = aStream.ReadLine();
      int lIndex = GetIndex( lOrientationLine );
      string lContigIdent = GetContigIdent( lOrientationLine );

      if ( lContigIdent != aTargetContigIdent ) // back up and let's try this again, taking smaller steps.
      {
        if ( aIncrement / 10 <= mLineLengthIsh ) // the next jump is too tiny to be a meaningful jump (we would stay in place). Well crap. 
          // this is the end of the line
        {
          return -1;
        }

        // back this train up.
        aStream.Position = lPreJumpPosition;
        return FindNextIndexStart( aStream, aTargetContigIdent, aTargetIndex, aIncrement /10, aDirection );
      }

      if ( lPreJumpPosition == aStream.Position ) // we are bouncing off an edge wall. This is it.
        if ( aDirection == Direction.JumpForward )
          return -1; // short circuit. we are done. // TODO, see if this is feasible for the corner cases.
        else
          return 0; // this is the closest left line to the index we are looking for.

      if ( lIndex < aTargetIndex && aDirection == Direction.JumpForward ) // not far enough, keep going forward
      {
        return FindNextIndexStart( aStream, aTargetContigIdent, aTargetIndex, aIncrement, Direction.JumpForward );
      }
      else if ( lIndex >= aTargetIndex  && aDirection == Direction.JumpForward ) // too far forward, skip back, slowing down.
      {
        return FindNextIndexStart( aStream, aTargetContigIdent, aTargetIndex, aIncrement / 10, Direction.JumpBackward );
      }
      else if ( lIndex >= aTargetIndex  && aDirection == Direction.JumpBackward ) // not far enough backward, keep skipping back.
      {
        return FindNextIndexStart( aStream, aTargetContigIdent, aTargetIndex, aIncrement, Direction.JumpBackward );
      }
      else if ( lIndex < aTargetIndex && aDirection == Direction.JumpBackward ) // too far back, skip forward, slowing down.
      {
        if ( ( aIncrement / 10 ) <= mLineLengthIsh ) // our expected jumps are now small enough that we just need to plough forward, line by line
        {
          while ( aStream.Position < mEndOfFilePos )
          {
            long lPosLine = aStream.Position;

            string lLine = aStream.ReadLine();

            int lLineIndex = GetIndex( lLine );



            if ( lLineIndex >= aTargetIndex )// FOUND IT
            {
              return lPosLine;
            }
          }
          return ( -1 ); // the target we seek doesn't exist in the file.          
        }
        else // rather than below, where we organize an even smaller skip forward.
        {
          return FindNextIndexStart( aStream, aTargetContigIdent, aTargetIndex, aIncrement / 10, Direction.JumpForward );
        }
      }
      throw new Exception( "WHATHTHT!" );
    }

    //private long FindNextIndexStart_OLD(
    //  DecoupledStreamReader aStream,
    //  string aTargetContigIdent,
    //  int aTarget,
    //  int aIncrement,
    //  int aStartingPosition,
    //  Direction aDirection )
    //{
    //  long lPreJumpPos = aStream.Position;

    //  // execute the jump (as directed)      
    //  if ( aDirection == Direction.JumpForward )
    //    SafeSeek( aStream, aStream.Position + aIncrement );
    //  else
    //    SafeSeek( aStream, aStream.Position - aIncrement );

    //  long lJumpPos = aStream.Position;


    //  // Where are we?
    //  aStream.ReadLine(); // go to the start of the next line to clear things up.      

    //  string lLine = aStream.ReadLine();
    //  int lIndex = GetIndex( lLine );
    //  string lContigIdent = GetContigIdent( lLine );

    //  long lPostReadPos = aStream.Position;

    //  if ( lPreJumpPos == aStream.Position ) // we are bouncing off a wall. This is it.
    //    if ( aDirection == Direction.JumpForward )
    //      return -1; // short circuit. we are done.
    //    else
    //      return 0; // this is the closest left line to the index we are looking for.


    //  if ( aTargetContigIdent == lContigIdent && lIndex < aTarget && aDirection == Direction.JumpForward ) // not far enough, keep going forward
    //  {
    //    return FindNextIndexStart( aStream, aTargetContigIdent, aTarget, aIncrement, Direction.JumpForward );
    //  }
    //  else if ( ( aTargetContigIdent != lContigIdent || lIndex >= aTarget ) && aDirection == Direction.JumpForward ) // too far forward, skip back, slowing down.
    //  {
    //    return FindNextIndexStart( aStream, aTargetContigIdent, aTarget, aIncrement / 10, Direction.JumpBackward );
    //  }
    //  else if ( ( aTargetContigIdent != lContigIdent || lIndex >= aTarget ) && aDirection == Direction.JumpBackward ) // not far enough backward, keep skipping back.
    //  {
    //    return FindNextIndexStart( aStream, aTargetContigIdent, aTarget, aIncrement, Direction.JumpBackward );
    //  }
    //  else if ( ( aTargetContigIdent != lContigIdent || lIndex < aTarget ) && aDirection == Direction.JumpBackward ) // too far back, skip forward, slowing down.
    //  {
    //    if ( ( aIncrement / 10 ) <= mLineLengthIsh ) // our expected jumps are now small enough that we just need to plough forward, line by line
    //    {
    //      while ( aStream.Position < mEndOfFilePos )
    //      {
    //        long lPosLine = aStream.Position;

    //        string lLine = aStream.ReadLine();

    //        int lLineIndex = GetIndex( lLine );



    //        if ( lLineIndex >= aTarget )// FOUND IT
    //        {
    //          return lPosLine;
    //        }
    //      }
    //      return ( -1 ); // the target we seek doesn't exist in the file.          
    //    }
    //    else // rather than below, where we organize an even smaller skip forward.
    //    {
    //      return FindNextIndexStart( aStream, aTarget, aIncrement / 10, Direction.JumpForward );
    //    }
    //  }
    //  throw new Exception( "BLEHHHH! HOW CAN THIS BE??!?!" );
    //}

    private void SafeSeek( DecoupledStreamReader aStream, long aTargetSeek )
    {
      if ( aTargetSeek >= mEndOfFilePos ) // at the end, back it up by a line and a half.
        aStream.Position = mEndOfFilePos - ( long ) ( mLineLengthIsh * 1.5 );
      else if ( aTargetSeek < 0 )
        aStream.Position = 0;
      else
        aStream.Position = aTargetSeek; // do the seek as requested.
    }

    internal virtual ICollection<Mapping> GetReads( string lContigIdent, int aLeft, int aRight )
    {
      List<Mapping> lResult = new List<Mapping>();

      int lStartingCache = aLeft / IndexIncrement;
      int lEndingCache = aRight / IndexIncrement;
      int lCachesRequired = lEndingCache - lStartingCache + 1; // this many caches.
      int lLeftPosition = aLeft; // starting position

      int lCurrentCachePosition = lStartingCache;
      for ( int i = 0 ; i < lCachesRequired ; i++ ) // loop through the total number of caches required.
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

      long lCount;
      if ( lStartingIndex + 1 < mFilePositions.Count )
        lCount = mFilePositions[ aContigIdent ][ lStartingIndex + 1 ] - lStartingPos;
      else
        lCount = mEndOfFilePos - lStartingPos;

      byte[] lBlock = new byte[ lCount ];
      mStream.Position = lStartingPos;
      mStream.BaseStream.Read( lBlock, 0, ( int ) lCount );

      MappingCache lCache = BuildCache( lBlock, aContigIdent, lStartingIndex * IndexIncrement, ( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );

      return lCache;
    }

    public virtual MappingCache BuildCache( byte[] lBlock, string aContigIdent, int aLeft, int aRight )
    {
      List<List<Mapping>> lMappings = new List<List<Mapping>>();
      for ( int i = aLeft ; i <= aRight ; i++ )
      {
        lMappings.Add( null );
      }

      StreamReader lStream = new StreamReader( new MemoryStream( lBlock ) );

      while ( lStream.Peek() >= 0 )
      {
        string lLine = lStream.ReadLine();

        int lIndex = GetIndex( lLine );
        int lPrivateIndex = lIndex - aLeft;

        if ( lMappings[ lPrivateIndex ] == null )
          lMappings[ lPrivateIndex ] = new List<Mapping>();

        lMappings[ lPrivateIndex ].Add( new Mapping( lIndex, new Sequence( GetSequence( lLine ) ) ) );
      }

      return new MappingCache( lMappings, aContigIdent, aLeft, aRight );

    }

    public abstract string GetSequence( string aLine );

    public abstract int GetIndex( string aLine );

    public abstract string GetContigIdent( string aLine );

    public static string[] GetLinePieces( string aLine )
    {
      return aLine.Split( Tab );
    }


  }
}
