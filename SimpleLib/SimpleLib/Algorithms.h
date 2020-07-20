#pragma once

#include <vector>
#include <random>
#include <cstdlib>


template <class Iter>
void InsertionSort(Iter begin, Iter end)
{
	for (auto i = begin; i < end; ++i)
	{
		for (auto j = begin; j < i; ++j)
		{
			auto v = *i;
			if (*j > v)
			{
				for (auto u = i; u > j; --u)
					*u = *(u - 1);
				*j = v;
				break;
			}
		}
	}
	return;
}

template <class Iter>
void QuickSort(Iter begin, Iter end)
{
	static auto sRandomDevice = std::mt19937_64(std::random_device()());
	static auto sUniformSizeTDist = std::uniform_int_distribution<std::size_t>();

	while (begin < end)
	{
		if (end - begin < 8)
		{
			InsertionSort(begin, end);
			return;
		}

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

template <class T, class Iter>
void CountingSort(Iter begin, Iter end, T minVal = 0, T maxVal = 0)
{
	if (maxVal == minVal)
	{
		for (auto i = begin; i < end; ++i)
		{
			auto v = *i;
			maxVal = std::max(maxVal, *i);
			minVal = std::min(minVal, *i);
		}
	}

	if (maxVal - minVal > 1024)
		throw std::exception("Value range is too large for counting sort.");

	std::vector<T> buf(end - begin);
	std::vector<size_t> nums(maxVal - minVal + 1, 0);
	for (auto i = begin; i < end; ++i)
		nums[*i - minVal]++;

	for (int i = 1; i < nums.size(); ++i)
		nums[i] += nums[i - 1];

	for (auto i = end - 1; i > begin; --i)
		buf[--nums[*i - minVal]] = *i;
	buf[--nums[*begin - minVal]] = *begin;

	auto d = begin;
	for (auto i = buf.begin(); i < buf.end(); ++i, ++d)
		*d = *i;
}

template <class Iter>
Iter KthMax(Iter begin, Iter end, std::ptrdiff_t k)
{
	static auto sRandomDevice = std::mt19937_64(std::random_device()());
	static auto sUniformSizeTDist = std::uniform_int_distribution<std::size_t>();

	if (k >= end - begin)
		return end;
	k = end - begin - k - 1;

	auto l = begin;
	auto r = end - 1;

	while (l < r)
	{
		auto rnd = sUniformSizeTDist(sRandomDevice) % (r - l);
		std::swap(*(l + rnd), *(r - 1));

		auto rval = *r;
		auto w = l;
		for (auto i = w; i < r; ++i)
			if (*i < rval)
				std::swap(*i, *w++);
		std::swap(*w, *r);

		if ((w - begin) < k)
			l = w + 1;
		else if (w - begin > k)
			r = w - 1;
		else
			return w;
	}

	return l;
}