using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
    class BowtieMappingEngine : MappingEngine
    {
        public BowtieMappingEngine(string aPath, Sequence aReferenceGenome)
            : base(aPath, aReferenceGenome)
        {

        }
        public override string GetSequence(string aLine)
        {
            throw new NotImplementedException();
        }

        public override int GetIndex(string aLine)
        {
            throw new NotImplementedException();
        }
    }
}
