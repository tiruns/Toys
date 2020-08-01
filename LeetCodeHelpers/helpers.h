#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <set>
#include <limits>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <list>
#include <queue>
#include <deque>
#include <algorithm>
#include <random>
#include <memory>

using namespace std;

class TreeNode
{
public:
	int val;
	TreeNode* left;
	TreeNode* right;
	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

namespace helpers
{
	vector<int> parse_integers(const char* text);
	vector<string> parse_strings(const char* text);
	vector<TreeNode> parse_btree(const char* text);
	vector<vector<int>> parse_matrix(const char* text);
}
