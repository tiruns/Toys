#pragma once

#include <vector>
#include <cstdlib>

template <class Iter>
void QuickSort(Iter begin, Iter end)
{
	if (begin < end)
	{
		auto r = end - 1;
		auto rval = *r;
		auto l = begin;
		for (auto i = begin; i < r; ++i)
			if (*i < rval)
				std::swap(*i, *l++);
		std::swap(*l, *r);
		QuickSort(begin, l);
		QuickSort(l + 1, end);
	}
}