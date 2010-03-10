#pragma once

#include "stdafx.h"

//#include <stdio.h>   // For access().
//#include <io.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().

#if defined( WIN32 )
#define SLASH "\\" 
#else
#define SLASH "/"
#endif

class Path
{
private:
  enum PathType
  {
    FILE,
    DIRECTORY,
    DOESNOTEXIST
  } mPathType;

  void SetPathType();

public:
  string mPathString;

  Path & operator/( const string & aPathString );

  Path(string mPathString);
  bool IsDirectory();
  bool IsFile();
  bool Exists();
  string Filename();
  string DirectoryPath();
  ~Path(void);
};
