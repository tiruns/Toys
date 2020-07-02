#pragma once

#include <vector>

template <class T, class Comp=std::less<T>>
class BinaryHeap
{
public:
	T& Top()
	{
		return mData[0];
	}
	void Pop()
	{
		mData.front() = mData.back();
		mData.pop_back();
		if (mData.size() != 0)
			Update(0);
	}
	void Insert(const T& value)
	{
		mData.push_back(value);
		int p = mData.size() - 1;
		while (p != 0)
		{
			if (Comp()(mData[p >> 1], mData[p]))
			{
				std::swap(mData[p >> 1], mData[p]);
				p >>= 1;
			}
			else
			{
				break;
			}
		}
	}
	bool IsEmpty()
	{
		return mData.size() == 0;
	}
private:
	void Update(size_t index)
	{
		int size = mData.size();
		int m = index;
		m = ((index << 1) < size && Comp()(mData[m], mData[index << 1])) ?
			(m << 1) : m;
		m = ((index << 1 | 1) < size && Comp()(mData[m], mData[index << 1 | 1])) ?
			(m << 1 | 1) : m;
		if (index != m)
		{
			std::swap(mData[index], mData[m]);
			Update(m);
		}
	}
	std::vector<T> mData;
};