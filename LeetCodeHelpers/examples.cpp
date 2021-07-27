#include "leetcode.h"

using namespace std;

class Solution {
public:
	void binaryTreeProblem(TreeNode* root) {
		return;
	}
};

int main()
{
	auto nums = parse_integers("[0, -1, 2, -3]");
	auto strs = parse_strings(R"(["aaa", "bbbb", "cdefg"])");
	auto matrix = parse_matrix("[[1,-2,3], [-4,5,-6], [-7,8,-9]]");

	auto btree = parse_binary_tree("[0, 1, 2, null, null, 5, 6]");
	Solution().binaryTreeProblem(&btree.front());

	return 0;
}