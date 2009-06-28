using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public abstract class SequenceReader
  {
    public string Path { get; set; }

    public SequenceReader( string aPath )
    {
      Path = aPath;
    }

    internal abstract Sequence Read();
    
  }
}
