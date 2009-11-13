using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class DecachedStreamReader : StreamReader
  {
    const int NewLineLength = 1;

    public DecachedStreamReader( string aPath )
      : base( aPath )
    {
      Position = 0;
    }

    public DecachedStreamReader( Stream aStream )
      : base( aStream )
    {
      Position = 0;
    }

    public long Seek( long aOffset, SeekOrigin aSeekOrigin )
    {
      long lOut = this.BaseStream.Seek( aOffset, aSeekOrigin );
      DiscardBufferedData();
      return lOut;
    }

    public override string ReadLine()
    {
      long lStartingPosition = Position;

      string lLine = base.ReadLine();

      if ( lLine != null )
        Position = lStartingPosition + lLine.Length + NewLineLength; // +1 for the newline in UNIX

      return lLine;
    }

    public override int Read()
    {
      long lPos = Position;
      int lReadInt = base.Read();

      Position = lPos + 1;
      return lReadInt;
    }

    public override int Peek()
    {
      long lPos = Position;
      int lPeekInt = base.Peek();

      Position = lPos;
      return lPeekInt;  
    }

    public long Position
    {
      get
      {
        return BaseStream.Position;
      }

      set
      {
        BaseStream.Position = value;
        DiscardBufferedData();
      }
    }

    internal void Write( byte[] aArray, int aOffset, int aCount )
    {
      BaseStream.Write( aArray, aOffset, aCount );
      DiscardBufferedData();
    }

    internal void Read( byte[] aBuffer, int aOffset, int aCount )
    {
      BaseStream.Read( aBuffer, aOffset, aCount );
      DiscardBufferedData();
    }

    internal void Flush()
    {
      BaseStream.Flush();
      DiscardBufferedData();
    }
  }

}
