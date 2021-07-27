#pragma once

#include <cstdint>

#include <iostream>
#include <fstream>

#include <array>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <forward_list>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <functional>


// Leetcode Structures

struct ListNode
{
public:
	int val;
	ListNode* next;
public:
	ListNode() : val(0), next(nullptr) {}
	ListNode(int x) : val(x), next(nullptr) {}
	ListNode(int x, ListNode* next) : val(x), next(next) {}
};

struct TreeNode
{
public:
	int val;
	TreeNode* left;
	TreeNode* right;
public:
	TreeNode() : val(0), left(nullptr), right(nullptr) {}
	TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
	TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};


// Function for parsing test cases

std::vector<int> parse_integers(const char* text);

std::vector<std::string> parse_strings(const char* text);

std::vector<std::vector<int>> parse_matrix(const char* text);

std::vector<TreeNode> parse_binary_tree(const char* text);

std::vector<ListNode> parse_linked_list(const char* text);

