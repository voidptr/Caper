using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  internal class Mapping
  {
    public int Index { get; private set; }
    public Sequence Sequence { get; private set; }

    public Mapping( int aIndex, Sequence aSequence )
    {
      Index = aIndex;
      Sequence = aSequence;
    }
  }
}
