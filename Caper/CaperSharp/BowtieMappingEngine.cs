using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  class BowtieMappingEngine : MappingEngine
  {
    public BowtieMappingEngine(string aPath, Sequences aReferenceGenome)
      : base(aPath, aReferenceGenome)
    {
    }
    internal override bool Initialize()
    {
      CheckForSortingAndResort();

      return base.Initialize();
    }

    private void CheckForSortingAndResort()
    {
      throw new NotImplementedException();
    }

    public override string GetSequence(string aLine)
    {
      return GetLinePieces(aLine)[4];
    }

    public override int GetIndex(string aLine)
    {
      return GetNormalizedIndex(GetLinePieces(aLine)[3]);
    }

    private int GetNormalizedIndex(string aValue)
    {
      return System.Convert.ToInt32(aValue) - 1;
    }

    public override string GetContigIdent(string aLine)
    {
      return GetLinePieces(aLine)[2];
    }
  }
}
