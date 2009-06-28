using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class MappingCache
  {
    private const char Tab = '\t';

    public int LeftIndex { get; private set; }
    public int RightIndex { get; private set; }
    public List<List<Mapping>> Sequences { get; private set; }

    private Stream mStream;

    public MappingCache( Stream aStream, int aLeftIndex, int aRightIndex )
    {
      Sequences = new List<List<Mapping>>( aRightIndex - aLeftIndex );

      for ( int i = aLeftIndex; i <= aRightIndex; i++ )
      {
        Sequences.Add( null );
      }

      LeftIndex = aLeftIndex;
      RightIndex = aRightIndex;
      mStream = aStream;
    }

    public bool Initialize()
    {
      StreamReader lStream = new StreamReader( mStream );

      while ( lStream.Peek() >= 0 )
      {
        string[] lLine = MapviewMappingEngine.GetLinePieces( lStream.ReadLine() );

        int lIndex = MapviewMappingEngine.GetIndex( lLine );

        int lPrivateIndex = PrivateIndex( lIndex );

        if ( Sequences[ lPrivateIndex ] == null )
          Sequences[ lPrivateIndex ] = new List<Mapping>();

        Sequences[ lPrivateIndex ].Add( new Mapping( lIndex, new Sequence( MapviewMappingEngine.GetSequence( lLine ) ) ) );
      }

      return true;
    }

    public ICollection<Mapping> GetReads( int aLeft, int aRight )
    {
      List<Mapping> lResult = new List<Mapping>();

      for ( int i = PrivateIndex(aLeft); i <= PrivateIndex(aRight); i++ )
      {
        if ( Sequences[ i ] != null )
          lResult.AddRange( Sequences[ i ] );
      }

      return lResult;
    }

    private int PrivateIndex( int aPublicIndex )
    {
      return aPublicIndex - LeftIndex;
    }
  }
}
