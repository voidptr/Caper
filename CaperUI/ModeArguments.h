#pragma once
#include "stdafx.h"

class ModeArguments
{
public:
	virtual bool ProcessArguments( int argc, char * const argv[] ) = 0;
};
