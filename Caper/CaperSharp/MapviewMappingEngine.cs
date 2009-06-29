﻿using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace CaperSharp
{
    class MapviewMappingEngine : MappingEngine
    {

        public MapviewMappingEngine(string aPath, Sequence aReferenceGenome)
            : base(aPath, aReferenceGenome)
        {

        }

        private int GetNormalizedIndex(string aValue)
        {
            return System.Convert.ToInt32(aValue) - 1;
        }

        public override string GetSequence(string aLine)
        {
            return GetLinePieces(aLine)[14];
        }

        public override int GetIndex(string aLine)
        {
            return GetNormalizedIndex(GetLinePieces(aLine)[2]);
        }
    }
}
