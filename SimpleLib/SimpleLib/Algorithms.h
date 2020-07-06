#pragma once

#include <vector>
#include <random>
#include <cstdlib>

namespace SimpleLibInternal
{
	auto sRandomDevice = std::mt19937_64(std::random_device()());
	auto sUniformSizeTDist = std::uniform_int_distribution<size_t>();
}

template <class Iter>
void QuickSort(Iter begin, Iter end)
{
	if (begin < end)
	{
		// TODO: a more elegent approach to generate random numbers
		auto rnd = SimpleLibInternal::sUniformSizeTDist(SimpleLibInternal::sRandomDevice) % (end - begin);
		
		std::swap(*(begin + rnd), *(end - 1));

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