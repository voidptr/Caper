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
    DecoupledStreamReader mStream;

    public FASequenceEngine(string aPath)
      : base(aPath)
    {
      GreaterThan = Convert.ToInt16('>');
      mStream = new DecoupledStreamReader( Path );
    }

    internal override bool Initialize()
    {      
      string lBuffer;

      while (mStream.Peek() >= 0)
      {
        string lSequenceIdentifier;
              
        lBuffer = mStream.ReadLine(); // fetch the name of the sequence.

        if (lBuffer[0] == GreaterThan)
        {
          lSequenceIdentifier = lBuffer.Substring(1);
        }
        else
        {
          throw new Exception("WHAT!!!??!?!");
        }

        long lPosition = mStream.Position; 

        int lChar;
        while ((lChar = mStream.Read()) >= 0)
        {
          if (lChar == GreaterThan)
          {
            mStream.Position = mStream.Position - 1;
            long lCount = mStream.Position - lPosition;

            Sequences.Add(lSequenceIdentifier, new Sequence( mStream.BaseStream, lPosition, lCount ) );
            break;
          }
        }
      }

      return true;
    }
  }
}
