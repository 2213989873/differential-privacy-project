#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;

class Solution {
public:
    int subarraySum(vector<int>& nums, int k) {
        unordered_map<int, int> prefixCount;
        prefixCount[0] = 1;  // 重要！空前缀和
        int prefix = 0, count = 0;
        for (int num : nums) {
            prefix += num;
            // 如果之前存在前缀和为 prefix - k
            // 则中间这段子数组的和为 k
            if (prefixCount.count(prefix - k)) {
                count += prefixCount[prefix - k];
            }
            // 记录当前前缀和
            prefixCount[prefix]++;
        }
        return count;
    }
};

int main() {
    Solution sol;
    vector<int> nums = {1, 2, 1, 2, 1};
    int k = 3;
    cout << sol.subarraySum(nums, k) << endl;  // 输出 4
    return 0;
}