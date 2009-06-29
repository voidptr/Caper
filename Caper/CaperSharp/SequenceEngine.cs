using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public abstract class SequenceEngine
  {
    public string Path { get; set; }

    protected Dictionary<string, long> mSequencePositions;

    protected struct PositionAndCount
    {
      public long Position;
      public long Count;
    }

    public SequenceEngine( string aPath )
    {
      Path = aPath;
      mSequencePositions = new Dictionary<string, PositionAndCount>();
    }

    internal abstract bool Initialize();

    internal abstract Sequence GetSequence(string lIdentifier);
    
  }
}
