#include "helpers.h"

using namespace std;

class Solution {
public:
	void btreeProblem(TreeNode* root) {
		return;
	}
};

int main()
{
	auto nums = helpers::parse_integers("[0, -1, 2, -3]");
	auto strs = helpers::parse_strings(R"(["aaa", "bbbb", "cdefg"])");
    auto matrix = helpers::parse_matrix("[[1,-2,3], [-4,5,-6], [-7,8,-9]]");

	auto btree = helpers::parse_btree("[0, 1, 2, null, null, 5, 6]");
	Solution().btreeProblem(&btree.front());
	return 0;
}