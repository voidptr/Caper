﻿using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class MapviewMappingEngine : MappingEngine
  {
    const int IndexIncrement = 1000;
    const int StartingJumpIncrement = 10000;
    private const char Tab = '\t';
    private int mLineLengthIsh;
    private long mEndOfFilePos;
    private int mNumberOfWindows;

    private MappingCache CacheA;
    private MappingCache CacheB;

    private MappingStreamReader mStream;
    List<long> mFilePositions;

    private enum Direction
    {
      JumpForward,
      JumpBackward
    }

    public MapviewMappingEngine( string aPath, Sequence aReferenceGenome )
      : base( aPath, aReferenceGenome )
    {
      mNumberOfWindows = (int) Math.Ceiling( ( double ) aReferenceGenome.Bases.Length / IndexIncrement );

      mFilePositions = new List<long>( mNumberOfWindows );
      mLineLengthIsh = 0;
      mEndOfFilePos = 0;
    }

    internal override bool Initialize()
    {
      mStream = new MappingStreamReader( Path );
      InitFileInformation( mStream );

      mFilePositions.Add( 0 ); // the zeroeth one is always at the start of the file.

      for ( int i = 1; i < mNumberOfWindows; i++ )
      {
        // start by setting a thousand line jump forward
        long lPos = FindNextIndexStart( mStream, i * IndexIncrement, StartingJumpIncrement, Direction.JumpForward );

        if ( lPos != -1 )
          mFilePositions.Add( lPos );
        else
          break; // no more targets to be found.
      }

      return true; // test whether the thing finished or not.

    }

    private void InitFileInformation( MappingStreamReader lStream )
    {
      mLineLengthIsh = lStream.ReadLine().Length;

      lStream.BaseStream.Seek( 0, SeekOrigin.End );
      mEndOfFilePos = lStream.Position;

      lStream.Position = 0;
    }

    private long FindNextIndexStart( MappingStreamReader aStream, int aTarget, int aIncrement, Direction aDirection )
    {
      long lPreJumpPos = aStream.Position;

      // execute the jump (as directed)      
      if ( aDirection == Direction.JumpForward )
        SafeSeek( aStream, aStream.Position + aIncrement );
      else
        SafeSeek( aStream, aStream.Position - aIncrement );

      long lJumpPos = aStream.Position;


      // Where are we?
      aStream.ReadLine(); // jump to next line to clear things up.      

      string lLine = aStream.ReadLine();
      int lIndex = GetIndex( GetLinePieces( lLine ) );

      long lPostReadPos = aStream.Position;

      if ( lPreJumpPos == aStream.Position ) // we are bouncing off a wall. This is it.
        if ( aDirection == Direction.JumpForward )
          return -1; // short circuit. we are done.
        else
          return 0; // this is the closest left line to the index we are looking for.


      if ( lIndex < aTarget && aDirection == Direction.JumpForward ) // not far enough, keep going forward
      {
        return FindNextIndexStart( aStream, aTarget, aIncrement, Direction.JumpForward );
      }
      else if ( lIndex >= aTarget && aDirection == Direction.JumpForward ) // too far forward, skip back, slowing down.
      {
        return FindNextIndexStart( aStream, aTarget, aIncrement / 10, Direction.JumpBackward );
      }
      else if ( lIndex >= aTarget && aDirection == Direction.JumpBackward ) // not far enough backward, keep skipping back.
      {
        return FindNextIndexStart( aStream, aTarget, aIncrement, Direction.JumpBackward );
      }
      else if ( lIndex < aTarget && aDirection == Direction.JumpBackward ) // too far, skip forward, slowing down.
      {
        if ( ( aIncrement / 10 ) <= mLineLengthIsh ) // our expected jumps are now small enough that we just need to plough forward, line by line
        {
          while ( aStream.Position < mEndOfFilePos )
          {
            long lPosLine = aStream.Position;
            
            int lLineIndex = GetIndex( GetLinePieces( aStream.ReadLine() ) );

            if ( lLineIndex >= aTarget )// FOUND IT
            {
              return lPosLine;
            }
          }
          return ( -1 ); // the target we seek doesn't exist in the file.          
        }
        else // rather than below, where we organize an even smaller skip forward.
        {
          return FindNextIndexStart( aStream, aTarget, aIncrement / 10, Direction.JumpForward );
        }
      }
      throw new Exception( "BLEHHHH! HOW CAN THIS BE??!?!" );
    }

    private void SafeSeek( MappingStreamReader aStream, long aTargetSeek )
    {
      if ( aTargetSeek >= mEndOfFilePos ) // at the end, back it up by a line and a half.
        aStream.Position = mEndOfFilePos - ( long ) ( mLineLengthIsh * 1.5 );
      else if ( aTargetSeek < 0 )
        aStream.Position = 0;
      else
        aStream.Position = aTargetSeek; // do the seek as requested.
    }

    internal override ICollection<Mapping> GetReads( int aLeft, int aRight )
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
          aRight : (( lCurrentCachePosition + 1 ) * IndexIncrement) - 1;

        MappingCache lAppropriateCache = GetCorrectCache( lLeftPosition, lRightPosition );
        lResult.AddRange( lAppropriateCache.GetReads( lLeftPosition, lRightPosition ) );

        // these only matter if we do end up going to the next cache.
        lCurrentCachePosition++;
        lLeftPosition = lCurrentCachePosition * IndexIncrement; // move the left pointer over to the beginning of the next one.
      }

      return lResult;
    }

    internal MappingCache GetCorrectCache( int aLeft, int aRight )
    {
      if ( CacheA != null && CacheA.LeftIndex <= aLeft && CacheA.RightIndex >= aLeft )
        return CacheA;
      else if ( CacheB != null && CacheB.LeftIndex <= aLeft && CacheB.RightIndex >= aLeft )
        return CacheB;
      else
      {
        RebuildCaches( aLeft ); // neither of them have it.
        return GetCorrectCache( aLeft, aRight ); // check again.
      }
    }

    private void RebuildCaches( int aLeft ) // these define the left edge of the problem.
    {
      int lStartingIndex = ( aLeft / IndexIncrement );

      CacheA = RebuildCache( lStartingIndex );

      if ( lStartingIndex + 1 < mFilePositions.Count )
        CacheB = RebuildCache( lStartingIndex + 1 );
      else
        CacheB = null;

    }

    private MappingCache RebuildCache( int lStartingIndex )
    {
      long lStartingPos = mFilePositions[ lStartingIndex ];

      long lCount;
      if ( lStartingIndex + 1 < mFilePositions.Count )
        lCount = mFilePositions[ lStartingIndex + 1 ] - lStartingPos;
      else
        lCount = mEndOfFilePos - lStartingPos;

      byte[] lBlock = new byte[ lCount ];
      mStream.Position = lStartingPos;
      mStream.BaseStream.Read( lBlock, 0, ( int ) lCount );
      
      MappingCache lCache = new MappingCache( new MemoryStream( lBlock ),
        lStartingIndex * IndexIncrement,
        ( ( lStartingIndex + 1 ) * IndexIncrement ) - 1 );
      lCache.Initialize();
      return lCache;
    }

    public static string GetIndexString( string[] aLine )
    {
      return aLine[ 2 ];
    }

    public static string GetSequence( string[] aLine )
    {
      return aLine[ 14 ];
    }

    public static int GetNormalizedIndex( string aValue )
    {
      return System.Convert.ToInt32( aValue ) - 1;
    }

    public static int GetIndex( string[] aLine )
    {
      return GetNormalizedIndex( GetIndexString( aLine ) );
    }

    public static string[] GetLinePieces( string aLine )
    {
      return aLine.Split( Tab );
    }

  }
}
