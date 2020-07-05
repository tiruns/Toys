#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <string>

#include <BinaryHeap.h>
#pragma comment(lib, "SimpleLib.lib")

using namespace std;

class BasicTest
{
public:
	virtual string GetName() = 0;
public:
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
		auto testCase = vector<int>();
		for (int i = -5; i <= 5; ++i)
			testCase.push_back(i);
		shuffle(testCase.begin(), testCase.end(), mt19937(random_device()()));

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

int main()
{
	vector<unique_ptr<BasicTest>> tests;
	tests.push_back(make_unique<BinaryHeapTest>());

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