using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  abstract class MappingEngine
  {
    public string Path { get; private set; }
    public Sequence ReferenceGenome { get; private set; }

    public MappingEngine( string aPath, Sequence aReferenceGenome )
    {
      Path = aPath;
      ReferenceGenome = aReferenceGenome;
    }

    internal abstract bool Initialize();

    internal abstract ICollection<Mapping> GetReads( int aLeft, int aRight );    
  }
}
