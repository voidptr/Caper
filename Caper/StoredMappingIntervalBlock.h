#pragma once
#include "triad.h"

class StoredMappingIntervalBlock :
  public triad<long long, long long, long long>
{
public:

  StoredMappingIntervalBlock( long long aOffset, long long aBlockSize, long long aStoredSize) : triad( aOffset, aBlockSize, aStoredSize ) {}

  // getters
  long long getOffset() const { return first; }
  long long getBlockSize() const { return second; }
  long long getStoredSize() const { return third; }

  // setters
  void setOffset( long long aValue ) { first = aValue; }
  void setBlockSize( long long aValue ) { second = aValue; }
  void setStoredSize( long long aValue ) { third = aValue; }
};
