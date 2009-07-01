using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public abstract class SequenceEngine
  {
    public string Path { get; set; }

    public Sequences Sequences { get; private set; }

    public SequenceEngine( string aPath )
    {
      Path = aPath;
      Sequences = new Sequences();
    }

    internal abstract bool Initialize();
  }
}
