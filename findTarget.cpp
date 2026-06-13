#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
using namespace std;
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> um;
        vector<int> ans;
        for (int i = 0; i < nums.size(); i++){
                if(um[target - nums[i]]){
                    ans.push_back(um[nums[i]]);
                    ans.push_back(i);
                    break;
                }
                else{
                    um[nums[i]] = i;
                }
        }
        for(int i = 0; i < ans.size(); i++){
            std::cout << ans[i] << std::endl;
        }
        return ans;
    }
};
class Solution2 {
public:
    int countGoodTriplets(vector<int>& arr, int a, int b, int c) {
        int ans = 0;
        for (int i = 0; i < arr.size(); i++){
            for (int j = i + 1; j < arr.size(); j++){
                for (int k = j + 1; k < arr.size(); k++){
                    if((abs(arr[i] - arr[j]) <= a) && (abs(arr[j] - arr[k]) <= b) && (abs(arr[i] - arr[k]) <= c)){
                        ans++;
                    }
                }
            }
        }
        return ans;
    }
};
int main() {
    Solution sol;
    Solution2 sol2;
    vector<int> nums = {2, 7, 11, 15};
    int target = 9;
    vector<int> arr = {3,0,1,1,9,7};
    int a = 7, b = 2, c = 3;
    sol.twoSum(nums, target);
    std::cout << sol2.countGoodTriplets(arr, a, b, c) << std::endl;
    int x = 'a';
    int y = 'Z';
    std::cout << x << " " << y << std::endl;
    return 0;
}