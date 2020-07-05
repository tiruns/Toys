#pragma once

#include <vector>

template <class T, class Comp=std::less<T>>
class BinaryHeap
{
public:
	T& Top()
	{
		if (this->IsEmpty())
			throw std::exception("BinaryHeap is empty.");
		return mData[0];
	}
	void Pop()
	{
		if (this->IsEmpty())
			throw std::exception("BinaryHeap is empty.");
		mData.front() = mData.back();
		mData.pop_back();
		if (mData.size() != 0)
			Update(0);
		return;
	}
	void Insert(const T& value)
	{
		mData.push_back(value);
		auto p = mData.size() - 1;
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
		if ((index << 1) >= index)
		{
			auto size = mData.size();
			auto m = index;
			auto l = index << 1;
			auto r = l | 1;

			m = l < size && Comp()(mData[m], mData[l]) ? l : m;
			m = r < size && Comp()(mData[m], mData[r]) ? r : m;
			if (index != m)
			{
				std::swap(mData[index], mData[m]);
				Update(m);
			}
		}
	}
	std::vector<T> mData;
};