using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;

namespace CaperSharp
{
  class FASequenceReader : SequenceReader
  {
    public FASequenceReader( string aPath )
      : base( aPath )
    {
    }

    internal override Sequence Read()
    {
      StreamReader lStream = new StreamReader( Path );

      string lBases = "";
      string lBuffer = "";

      lStream.ReadLine(); // dump the first line that has the name of the sequence.
      lBuffer = lStream.ReadToEnd();

      lBases = Regex.Replace( lBuffer, "\n", "" );

      Sequence lSequence = new Sequence( lBases );
      return lSequence;
    }
  }
}
