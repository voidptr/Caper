using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace CaperSharp
{
  class FASequenceEngine : SequenceEngine
  {
    readonly int GreaterThan;

    public FASequenceEngine(string aPath)
      : base(aPath)
    {
      GreaterThan = Convert.ToInt16('>');
    }

    internal override bool Initialize()
    {
      DecoupledStreamReader lStream = new DecoupledStreamReader(Path);

      string lBuffer;

      while (lStream.Peek() >= 0)
      {
        string lSequenceIdentifier;
        PositionAndCount lPositionAndCount = new PositionAndCount();
        

        lBuffer = lStream.ReadLine(); // fetch the name of the sequence.

        if (lBuffer[0] == GreaterThan)
        {
          lSequenceIdentifier = lBuffer.Substring(1);
        }
        else
        {
          throw new Exception("WHAT!!!??!?!");
        }

        lPositionAndCount.Position = lStream.Position; 

        int lChar;
        while ((lChar = lStream.Read()) >= 0)
        {
          if (lChar == lGreaterThan)
          {
            lStream.Position = lStream.Position - 1;
            lPositionAndCount.Count = lStream.Position - lStartingPosition;

            mSequencePositions.Add(lSequenceIdentifier, lPositionAndCount);

            break;
          }
        }
      }
    }

    internal override Sequence GetSequence(string lIdentifier)
    {
      throw new NotImplementedException();
    }
  }
}
