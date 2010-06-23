#pragma once

#include "stdafx.h"

#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().

class Path
{
private:
  enum PathType
  {
    FILE,
    DIRECTORY,
    DOESNOTEXIST
  } mPathType;

  void SetPathType()
  {
    struct stat status;

    if ( mPathString.length() > 0 && stat( mPathString.c_str(), &status ) == 0 )
    {
      if ( status.st_mode & S_IFDIR )
      {
          mPathType = DIRECTORY; // it's a directory!
      }
      else
      {
          mPathType = FILE; // it's a file!
      } 
    }
    else
      mPathType = DOESNOTEXIST; // doesn't even exist.
  }

public:
  string mPathString;

  Path( string aPath )
  {
    mPathString = aPath;
    SetPathType();
  }

  //Path & operator/( const string & aPathString );

  //Path(string mPathString);
  //bool IsDirectory();
  //bool IsFile();
  //bool Exists();
  //string Filename();
  //string DirectoryPath();
  //~Path(void);

  Path & operator /(const string &aPathString)
  {
    if ( IsDirectory() )
      mPathString += (SLASH + aPathString);
    else 
      mPathString += aPathString;    

    SetPathType();

    return *this;
  }

  bool IsDirectory()
  {
    if ( mPathType == DIRECTORY )
      return true;

    return false;
  }

  bool IsFile()
  {
    if ( mPathType == FILE )
      return true;

    return false;
  }

  bool Exists()
  {
    if ( mPathType != DOESNOTEXIST )
      return true;

    return false;
  }

  string Filename()
  {
    int lSlashPos = mPathString.find_last_of(SLASH);

    if ( lSlashPos == -1 )
      return mPathString;
    else if ( lSlashPos != mPathString.length() - 1 ) // final slash
      return mPathString.substr( lSlashPos + 1 );
    
    return "";
  }

  string DirectoryPath()
  {
    if ( IsFile() )
    {
      int lSlashPos = mPathString.find_last_of(SLASH);

      if ( lSlashPos == -1 ) // no slash found, just return "";
        return "";
      else if ( lSlashPos != mPathString.length() - 1 ) // final slash
        return mPathString.substr( 0, lSlashPos + 1 );
    }
    else if ( IsDirectory() )
    {
      return mPathString;
    }

    throw string("File does not exist, no way to know its directory path.");
  //  return ""; // don't know what the correct behavior is here, since it doesn't exist, we don't know if it was intended as a directory or file.
  }

};
