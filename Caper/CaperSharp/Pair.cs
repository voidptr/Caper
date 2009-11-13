using System;
using System.Collections.Generic;
using System.Text;

namespace CaperSharp
{
  public struct Pair<T>
  {
    public T FirstItem { get; set; }
    public T SecondItem { get; set; }
  }
}
