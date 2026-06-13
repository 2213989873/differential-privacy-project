#include <vector>
#include <iostream>
using namespace std;
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        if(nums.size() == 1) return nums[0]; 
        int max_prefix = -100000;
        int prefix = 0;
        int pos = 0;
        int pos1 = 0;
        int max_num = -100000;
        vector<int> prefixx;
        for (int i = 0; i < nums.size(); i++){
            prefix += nums[i];
            prefixx.push_back(prefix);
            if (prefix > max_prefix){
                pos = i;
                max_prefix = prefix;
            }
            if (nums[i] > max_num){
                max_num = nums[i];
                pos1 = i;
            }
        }
        int cur = 0;
        int ans = 0;
        if (max_num > max_prefix){
            pos =pos1;
            ans = max_num;
        }
        else{
            ans = max_prefix;
        }
        
        for(int i = 0; i < pos; i++){
            cur = prefixx[pos] - prefixx[i];
            ans = max(ans, cur);
        }
        return ans;
    }
};
int main(){
    Solution sol;
    vector<int> nums = {-2,1,-3,4,-1,2,1,-5,4};
    sol.maxSubArray(nums);
    cout << sol.maxSubArray(nums) << endl;  // 输出 6
    return 0;
}