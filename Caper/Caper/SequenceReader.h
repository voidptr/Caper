#pragma once

#include "stdafx.h"
#include "Sequence.h"
#include <fstream>

using namespace std;

class Sequence;

class SequenceReader
{
private:
	string mPath;
public:
	SequenceReader(char * aPath);
	Sequence* Read();
	~SequenceReader(void);
};
