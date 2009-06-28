using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  class Sequence
  {
    public string Bases { get; private set; }
    internal Sequence( string aBases )
    {
      Bases = aBases;
    }
  }
}
