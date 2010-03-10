
#include "Path.h"

Path::Path( string aPath )
{
  mPathString = aPath;
  SetPathType();
}

void Path::SetPathType()
{
 // if ( mPathString.length() > 0 && access( mPathString.c_str(), 0 ) == 0 )

  struct stat status;

  if ( mPathString.length() > 0 && stat( mPathString.c_str(), &status ) )
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

Path & Path::operator /(const string &aPathString)
{
  if ( IsDirectory() )
    mPathString += (SLASH + aPathString);
  else 
    mPathString += aPathString;    

  SetPathType();

  return *this;
}

bool Path::IsDirectory()
{
  if ( mPathType == DIRECTORY )
    return true;

  return false;
}

bool Path::IsFile()
{
  if ( mPathType == FILE )
    return true;

  return false;
}

bool Path::Exists()
{
  if ( mPathType != DOESNOTEXIST )
    return true;

  return false;
}

string Path::Filename()
{
  int lSlashPos = mPathString.find_last_of(SLASH);

  if ( lSlashPos == -1 )
    return mPathString;
  else if ( lSlashPos != mPathString.length() - 1 ) // final slash
    return mPathString.substr( lSlashPos + 1 );
  
  return "";
}

string Path::DirectoryPath()
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

  throw string("GAH!");
//  return ""; // don't know what the correct behavior is here, since it doesn't exist, we don't know if it was intended as a directory or file.
}

Path::~Path(void)
{
}
