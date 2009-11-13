#include "SequenceReader.h"

SequenceReader::SequenceReader(char * aPath)
{
	mPath = aPath;
}

Sequence* SequenceReader::Read()
{
	cout << "Reading Sequence from File: " << mPath << "... ";

	ifstream lFile;
	lFile.open( mPath.c_str() );

	if (!lFile)
	{
		cerr << "Unable to open " << mPath << "\n";
		exit(1);
	}

	string lBuffer;
	string lBases;

	lFile >> lBuffer; // dump the first line that has the name of the sequence.
	while ( lFile >> lBuffer )
	{
			lBases.append( lBuffer.c_str() );
	}
	lFile.close();

	cout << "DONE\n";

	Sequence* lSequence = new Sequence(lBases);
	return lSequence;
}

SequenceReader::~SequenceReader(void)
{
}
