using System;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace CaperSharp
{
  public class Sequence
  {
    private const char NewLine = '\n';

    private string mBases;

    private StreamReader mHandle;
    private long mPosition;
    public long Length { get; private set; }

    private bool mIsStream;

    internal Sequence(string aBases)
    {
      mBases = aBases;
      mIsStream = false;
      Length = aBases.Length;
    }

    internal Sequence(Stream aHandle, long aPosition, long aLength)
    {
      mHandle = new StreamReader(aHandle);
      mPosition = aPosition;
      Length = aLength;

      mIsStream = true;
    }

    internal char this[int aIndex]
    {
      get { return mBases[aIndex]; }
    }

    internal string Substring(long aIndex, long aCount)
    {
      if (!mIsStream)
        return mBases.Substring((int)aIndex, (int)aCount);
      else
      {
        if (aCount > Length)
          throw new Exception("Could not seek as far as requested.");

        mHandle.BaseStream.Seek(mPosition + aIndex, SeekOrigin.Begin);
        mHandle.DiscardBufferedData();
        char[] lBuffer = new char[aCount];
        if (mHandle.Read(lBuffer, 0, (int)aCount) < aCount)
          throw new Exception("Could not read sequence from handle.");

        string lBufferedString = new string(lBuffer);

        return Regex.Replace(lBufferedString, "\n", "");
      }
    }

    public override string ToString()
    {
      return Substring(0, Length);

    }
  }
}
