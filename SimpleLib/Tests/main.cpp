#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <string>

#include <BinaryHeap.h>
#include <Algorithms.h>
#pragma comment(lib, "SimpleLib.lib")

using namespace std;

class BasicTest
{
public:
	vector<int> GenerateRandomNumbers(int num = 10, int min = -100, int max = 100)
	{
		vector<int> result;
		auto randomDevice = mt19937();
		auto uniformDist = uniform_int_distribution<int>(min, max);
		for (int i = 0; i < num; ++i)
			result.push_back(uniformDist(randomDevice));
		return result;
	}
public:
	virtual string GetName() = 0;
	virtual bool Run() = 0;
};

class BinaryHeapTest : public BasicTest
{
public:
	virtual string GetName() override
	{
		return "BinaryHeap";
	}

	virtual bool Run() override
	{
		auto testCase = this->GenerateRandomNumbers();
		//shuffle(testCase.begin(), testCase.end(), mt19937(random_device()()));

		BinaryHeap<int, std::greater<int>> binHeap;
		for (auto val : testCase)
			binHeap.Insert(val);

		sort(testCase.begin(), testCase.end());
		for (auto val : testCase)
		{
			if (binHeap.IsEmpty() || binHeap.Top() != val)
				return false;
			binHeap.Pop();
		}

		return true;
	}
};


class QuickSortTest : public BasicTest
{
public:
	virtual string GetName() override
	{
		return "QuickSortTest";
	}
	virtual bool Run() override
	{
		auto testCase = this->GenerateRandomNumbers();
		auto dup = testCase;
		QuickSort(testCase.begin(), testCase.end());
		sort(dup.begin(), dup.end());

		if (testCase.size() != dup.size())
			return false;

		for (int i = 0; i < testCase.size(); ++i)
		{
			if (testCase[i] != dup[i])
				return false;
		}

		return true;
	}
};

int main()
{
	vector<unique_ptr<BasicTest>> tests;
	tests.push_back(make_unique<BinaryHeapTest>());
	tests.push_back(make_unique<QuickSortTest>());

	int passCounter = 0;

	for (auto& test : tests)
	{
		cout << test->GetName();
		if (!test->Run())
		{
			cout << " Failed.\n";

		}
		else
		{
			cout << " Passed.\n";
			++passCounter;
		}
	}

	cout << passCounter << "/" << tests.size() << " tests passed.\n";

	return 0;
}