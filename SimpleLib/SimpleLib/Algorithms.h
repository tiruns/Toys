#pragma once

#include <vector>
#include <random>
#include <cstdlib>

template <class Iter>
void QuickSort(Iter begin, Iter end)
{
	static auto sRandomDevice = std::mt19937_64(std::random_device()());
	static auto sUniformSizeTDist = std::uniform_int_distribution<size_t>();

	while (begin < end)
	{
		// TODO: a more elegent approach to generate random numbers
		auto rnd = sUniformSizeTDist(sRandomDevice) % (end - begin);
		std::swap(*(begin + rnd), *(end - 1));

		auto r = end - 1;
		auto rval = *r;
		auto l = begin;
		for (auto i = begin; i < r; ++i)
			if (*i < rval)
				std::swap(*i, *l++);
		std::swap(*l, *r);
		QuickSort(begin, l);
		begin = l + 1;
	}
}