using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
  class MappingStreamReader : StreamReader
  {
    const int NewLineLength = 1;

    public MappingStreamReader( string aPath )
      : base( aPath )
    {
    }

    public override string ReadLine()
    {
      long lStartingPosition = Position;

      string lLine = base.ReadLine();

      Position = lStartingPosition + lLine.Length + NewLineLength; // +1 for the newline in UNIX

      return lLine;
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
