#include <BinaryHeap.h>
#include <iostream>
#pragma comment(lib, "SimpleLib.lib")


int main()
{
	BinaryHeap<int, std::greater<int>> h;
	h.Insert(100);
	h.Insert(50);
	h.Insert(1);
	while (!h.IsEmpty())
	{
		std::cout << h.Top() << ", ";
		h.Pop();
	}
	return 0;
}