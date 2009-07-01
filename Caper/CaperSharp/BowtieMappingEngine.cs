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
            throw new NotImplementedException();
        }

        public override int GetIndex(string aLine)
        {
            throw new NotImplementedException();
        }
    }
}
