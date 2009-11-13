#pragma once
#include "stdafx.h"
#include "Sequence.h"

class SequenceReader
{
private:
	string mPath;
public:
	SequenceReader(char * aPath);
	Sequence* Read();
	~SequenceReader(void);
};
