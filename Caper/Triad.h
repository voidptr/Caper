#pragma once

// utility standard header
//#include <iosfwd>

#ifdef _MSC_VER
 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
#endif  /* _MSC_VER */

		// TEMPLATE FUNCTION swap (from <algorithm>)
template<class _Ty> inline
	void swap(_Ty& _Left, _Ty& _Right)
	{	// exchange values stored at _Left and _Right
	if (&_Left != &_Right)
		{	// different, worth swapping
		_Ty _Tmp = _Left;

		_Left = _Right;
		_Right = _Tmp;
		}
	}

		// TEMPLATE STRUCT triad
template<class _Ty1,
	class _Ty2, 
  class _Ty3> struct triad
	{	// store a trio of values
	typedef triad<_Ty1, _Ty2, _Ty3> _Myt;
	typedef _Ty1 first_type;
	typedef _Ty2 second_type;
  typedef _Ty3 third_type;

	triad()
		: first(_Ty1()), second(_Ty2()), third(_Ty3())
		{	// construct from defaults
		}

	triad(const _Ty1& _Val1, const _Ty2& _Val2, const _Ty3& _Val3)
		: first(_Val1), second(_Val2), third(_Val3)
		{	// construct from specified values
		}

	template<class _Other1,
		class _Other2,
    class _Other3>
		triad(const triad<_Other1, _Other2, _Other3>& _Right)
		: first(_Right.first), second(_Right.second), third(_Right.third)
		{	// construct from compatible triad
		}

	void swap(_Myt& _Right)
		{	// exchange contents with _Right
		if (this != &_Right)
			{	// different, worth swapping
			std::swap(first, _Right.first);
			std::swap(second, _Right.second);
      std::swap(third, _Right.third);
			}
		}

	_Ty1 first;	// the first stored value
	_Ty2 second;	// the second stored value
  _Ty3 third;
	};

		// triad TEMPLATE FUNCTIONS
template<class _Ty1,
	class _Ty2, 
  class _Ty3> inline
	bool operator==(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test for pair equality
	return (_Left.first == _Right.first && _Left.second == _Right.second && _Left.third == _Right.third);
	}

template<class _Ty1,
	class _Ty2,
  class _Ty3> inline
	bool operator!=(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test for pair inequality
	return (!(_Left == _Right));
	}

template<class _Ty1,
	class _Ty2, 
  class _Ty3> inline
	bool operator<(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test if _Left < _Right for pairs


    if ( _Left.first < _Right.first )
      return true;
    else
    {
      if ( !(_Right.first < _Left.first ) ) // effectively equal
      {
        if ( _Left.second < _Right.second )
          return true;
        else
        {
          if ( !(_Right.second < _Left.second ) )
          {
            if ( _Left.third < _Right.third )
              return true;
            else
              return !(_Right.third < _Left.third );
          }
          else
            return false;
        }
      }
      else
        return false;
    }
    
//	return (_Left.first < _Right.first || // first element is less than OR
//		!(_Right.first < _Left.first) && _Left.second < _Right.second);

	}

template<class _Ty1,
	class _Ty2, 
  class _Ty3> inline
	bool operator>(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test if _Left > _Right for pairs
	return (_Right < _Left);
	}

template<class _Ty1,
	class _Ty2,
  class _Ty3> inline
	bool operator<=(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test if _Left <= _Right for pairs
	return (!(_Right < _Left));
	}

template<class _Ty1,
	class _Ty2,
  class _Ty3> inline
	bool operator>=(const triad<_Ty1, _Ty2, _Ty3>& _Left,
		const triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// test if _Left >= _Right for pairs
	return (!(_Left < _Right));
	}

template<class _Ty1,
	class _Ty2,
  class _Ty3> inline
	triad<_Ty1, _Ty2, _Ty3> make_triad(_Ty1 _Val1, _Ty2 _Val2, _Ty3 _Val3)
	{	// return pair composed from arguments
	return (triad<_Ty1, _Ty2, _Ty3>(_Val1, _Val2, _Val3));
	}

template<class _Ty1,
	class _Ty2,
  class _Ty3> inline
	void swap(triad<_Ty1, _Ty2, _Ty3>& _Left, triad<_Ty1, _Ty2, _Ty3>& _Right)
	{	// swap _Left and _Right pairs
	_Left.swap(_Right);
	}

		// TEMPLATE OPERATORS
	namespace rel_ops
		{	// nested namespace to hide relational operators from std
template<class _Ty> inline
	bool  operator!=(const _Ty& _Left, const _Ty& _Right)
	{	// test for inequality, in terms of equality
	return (!(_Left == _Right));
	}

template<class _Ty> inline
	bool operator>(const _Ty& _Left, const _Ty& _Right)
	{	// test if _Left > _Right, in terms of operator<
	return (_Right < _Left);
	}

template<class _Ty> inline
	bool operator<=(const _Ty& _Left, const _Ty& _Right)
	{	// test if _Left <= _Right, in terms of operator<
	return (!(_Right < _Left));
	}

template<class _Ty> inline
	bool operator>=(const _Ty& _Left, const _Ty& _Right)
	{	// test if _Left >= _Right, in terms of operator<
	return (!(_Left < _Right));
	}
		}
#ifdef _MSC_VER
 #pragma warning(pop)
 #pragma pack(pop)
#endif  /* _MSC_VER */

/*
 * This file is derived from software bearing the following 
 * restrictions:
 *
 * Copyright (c) 1992-2006 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Hewlett-Packard Company makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 V5.02:0009 */

