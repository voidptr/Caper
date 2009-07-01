using System.Collections.Generic;

namespace CaperSharp
{
  public class Sequences : SortedDictionary<string, Sequence>
  {
    public long Length
    {
      get
      {
        long lCount = 0;
        foreach ( Sequence lSequence in this.Values )
        {
          lCount += lSequence.Length;
        }
        return lCount;
      }
    }
  }
}
