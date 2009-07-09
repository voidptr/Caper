using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class DecoupledStreamReader : StreamReader
  {
    const int NewLineLength = 1;

    public DecoupledStreamReader( string aPath )
      : base( aPath )
    {
      Position = 0;
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
  }

}
