#pragma once

template <typename T> class Pair
{
private:
	T mValues[2];
public:
	Pair(T aFirst, T aSecond);
	T FirstItem();
	T SecondItem();
};
