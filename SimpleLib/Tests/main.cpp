#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <string>
#include <chrono>

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
		auto randomDevice = mt19937(random_device()());
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
		return "BinaryHeapTest";
	}

	virtual bool Run() override
	{
		auto testCase = this->GenerateRandomNumbers();

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

class SortTest : public BasicTest
{
public:
	virtual string GetName() override
	{
		return "SortTest";
	}
	virtual bool Run() override
	{
		if (!this->FunctionTest())
			return false;
#ifdef NDEBUG
		this->PerformanceTest();
#endif
		return true;
	}
private:
	bool FunctionTest()
	{
		// InsertionSort & QuickSort
		auto testCase = this->GenerateRandomNumbers();
		auto dup1 = testCase;
		auto dup2 = testCase;

		std::sort(testCase.begin(), testCase.end());
		InsertionSort(dup1.begin(), dup1.end());
		QuickSort(dup2.begin(), dup2.end());

		if (testCase.size() != dup1.size() || testCase.size() != dup2.size())
			return false;

		for (int i = 0; i < testCase.size(); ++i)
			if (testCase[i] != dup1[i] || testCase[i] != dup2[i])
				return false;

		// CountingSort
		auto testCase2 = this->GenerateRandomNumbers(2000, -8, 8);
		auto dup21 = testCase2;

		std::sort(testCase2.begin(), testCase2.end());
		CountingSort<int>(dup21.begin(), dup21.end());

		if (testCase2.size() != dup21.size())
			return false;

		for (int i = 0; i < testCase2.size(); ++i)
			if (testCase2[i] != dup21[i])
				return false;

		return true;
	}
	void PerformanceTest()
	{
		auto clock = chrono::high_resolution_clock();;
		
		vector<vector<int>> testSetA;
		for (int i = 0; i < 100; ++i)
			testSetA.push_back(this->GenerateRandomNumbers(25000, -10000, 10000));
		for (int i =0; i<20; ++i) // QuickSort() performs badly on these cases
			testSetA.push_back(this->GenerateRandomNumbers(25000, -10, 10));

		for (int i = 40; i < 70; ++i)
			for (int j = 0; j < testSetA[i].size(); ++j)
				testSetA[i][j] += j * 10;
		for (int i = 70; i < 100; ++i)
			for (int j = 0; j < testSetA[i].size(); ++j)
				testSetA[i][j] -= j * 10;

		auto testSetB = testSetA;

		auto timePointA = clock.now();
		for (auto& testCase : testSetA)
			QuickSort(testCase.begin(), testCase.end());
		auto timePointB = clock.now();
		for (auto& testCase : testSetB)
			sort(testCase.begin(), testCase.end());
		auto timePointC = clock.now();

		std::cout << "\tQuickSort: " << static_cast<float>((timePointB - timePointA).count()) / 1e6f << "ms\n";
		std::cout << "\tstd::sort: " << static_cast<float>((timePointC - timePointB).count()) / 1e6f << "ms\n";
		
		return;
	}
};

class KthMaxTest : public BasicTest
{
public:
	virtual string GetName() override
	{
		return "KthMaxTest";
	}

	virtual bool Run() override
	{
		auto testCase = this->GenerateRandomNumbers(30, -500, 500);
		vector<int> answers;
		for (int i = 0; i < testCase.size(); ++i)
		{
			auto dup = testCase;
			answers.push_back(*KthMax(dup.begin(), dup.end(), i));
		}

		sort(testCase.begin(), testCase.end(), [](int l, int r) { return l > r; });

		return testCase == answers;
	}
};

int main()
{
	vector<unique_ptr<BasicTest>> tests;
	tests.push_back(make_unique<BinaryHeapTest>());
	tests.push_back(make_unique<SortTest>());
	tests.push_back(make_unique<KthMaxTest>());

	int passCounter = 0;

	for (auto& test : tests)
	{
		cout << test->GetName() << " Begins\n";
		if (!test->Run())
		{
			cout << test->GetName() << " Failed\n";

		}
		else
		{
			cout << test->GetName() << " Passed\n";
			++passCounter;
		}
	}

	cout << "\n" << passCounter << "/" << tests.size() << " tests passed.\n";

	return 0;
}