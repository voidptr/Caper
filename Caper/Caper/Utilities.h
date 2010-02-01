#pragma once

#include "stdafx.h"

class Utilities
{
public:
  static void Split(const string & aString, vector<string> & aTokens, const string & aDelimiters = " ");
  static void SplitPath(const string & aPath, vector<string> & aTokens);
  static string GetFilename(const string& aPath);
  static string ToUpper( string aString );
};
