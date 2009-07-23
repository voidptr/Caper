#include "stdafx.h"
#include "Pair.h"

using namespace std;

template<typename T> Pair<T>::Pair(T aFirst, T aSecond)
{
	mValues[0] = aFirst;
	mValues[1] = aSecond;
}

template<typename T> T Pair<T>::FirstItem()
{
	return mValues[0];
}

template<typename T> T Pair<T>::SecondItem()
{
	return mValues[1];
}
