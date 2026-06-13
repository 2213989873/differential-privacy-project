#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
using namespace std;
class Solution {
public:
    bool barelyOnly(const unordered_map<int,int>& count, int m,int& OnlyNum){
        for (const auto& pair : count) {
            if (pair.second != 0) {
                OnlyNum++;
            }
        }
        if (OnlyNum >= m) return true;
        else return false;
    }
    long long maxSum(vector<int>& nums, int m, int k) {
        unordered_map<int,int> count;
        long long max_sum = 0;
        long long current = 0;
        for (int i = 0; i < k; i++){
            count[nums[i]]++;  
            current += nums[i];
        }
        int OnlyNum = 0;
        bool initOnly = barelyOnly(count, m, OnlyNum);
        if (initOnly) max_sum = current; 
        if (nums.size() == k) {
            if (initOnly) return current; 
            else return 0;
        }
        for (int i = 0; i <= nums.size() - k - 1; i++){
            count[nums[i]]--;
            if (count[nums[i]] == 0) OnlyNum--;
            count[nums[i+k]]++;
            current = current - nums[i] + nums[i+k];
            if (count[nums[i+k]] == 1) OnlyNum++;
            if (OnlyNum >= m && current > max_sum) max_sum = current;

        }
        return max_sum;
    }
    int maxFreeTime(int eventTime, int k, vector<int>& startTime, vector<int>& endTime) {
        int cnt = 0;
        int maxtime = 0;
        for (int i = 0; i < startTime.size() + 1; i++){
            if (i == 0) cnt += startTime[i];
            else if (i == startTime.size()) cnt += eventTime - endTime[i-1];
            else cnt += startTime[i] - endTime[i-1];
            int left = i - k;
            if (left < 0) continue;
            maxtime = max(maxtime, cnt);
            //退出窗口
            if (left == 0) cnt -= startTime[left];
            else cnt -= (startTime[left] - endTime[left-1]);
        } 
        return maxtime;
    }
        int maxSatisfied(vector<int>& customers, vector<int>& grumpy, int minutes) {
        int nums_before_ctrl = 0;
        int nums_after_ctrl = 0;
        int max_plus = 0;
        int plus = 0; 
        for (int i = 0; i < customers.size(); i++){
            if (!grumpy[i]) nums_before_ctrl += customers[i];
            if (grumpy[i]) plus += customers[i];
            int left = i - minutes + 1;
            if (left < 0) continue;
            max_plus = max(max_plus, plus);
            if (grumpy[left]) plus -= customers[left];
        }
        return max_plus + nums_before_ctrl;
    }
      int lengthOfLongestSubstring(const string &s) {
        unordered_map<char, int> uniqueChar;
        int maxLength = 0;
        int currentLength = 0;
        int left = 0;
        for (int i = 0; i < s.size(); i++){
            //find  repeat char
            if (uniqueChar.find(s[i]) != uniqueChar.end()){
                if (uniqueChar[s[i]] < left) {
                    uniqueChar[s[i]] = i;
                    currentLength++;
                    maxLength = max(maxLength, currentLength);
                    continue;
                }
                left = uniqueChar[s[i]] + 1;
                uniqueChar[s[i]] = i;
                currentLength = i - left + 1;
                maxLength = max(maxLength, currentLength);
            }
            else{
                uniqueChar[s[i]] = i;
                currentLength++;
                maxLength = max(maxLength, currentLength);
            }
        }
        return maxLength;
    }
        int maximumLengthSubstring(const string &s) {
        unordered_map<char, int> charCount;
        int currentLength = 0;
        int maxLength = 0;
        int left = 0;
        for (int right = 0; right < s.size(); right++){
            if (charCount[s[right]] < 2){
                charCount[s[right]]++;
                currentLength = right - left + 1;
                maxLength = max(maxLength, currentLength); 
            }
            else{
                charCount[s[right]]++;
                if (charCount[s[left] == 3]){
                    charCount[s[left]]--;
                    left++;
                }
                else{
                  while(charCount[s[left]] != 3) {
                    charCount[s[left]]--;
                    left++; 
                    }
                    charCount[s[left]]--;
                  left++;
                }
                
            }
        }
        return maxLength;
    }
    int totalFruit(vector<int>& fruits) {
        unordered_map<int, int> count;
        int kind = 0;
        int max_num = 0;
        int current = 0;
        int left = 0;
        for (int right = 0; right < fruits.size(); right++){
            current++;
            if (count.find(fruits[right]) == count.end()){
                kind++;
                count[fruits[right]] = 1;
            }
            else count[fruits[right]]++;
            while (kind == 3){
                count[fruits[left]]--;
                if (count[fruits[left]] == 0) {
                    count.erase(fruits[left]);
                    kind--;        
                }
                current--;
                left++;
            }
            max_num = max(max_num, current);
        }
        return max_num;
    }
    int maxSubarrayLength(vector<int>& nums, int k) {
        unordered_map<int,int> cnt;
        int ans = 1000000;
        for (int right = 0, left = 0; right < nums.size(); right++){
            if (++cnt[nums[right]] > k){
                while(cnt[nums[right] > k]){ 
                    --cnt[nums[left++]];
                }
            }
                ans = max (ans, right - left + 1);
            
        }
        return ans;
    }
       int minOperations(vector<int>& nums, int x) {
        vector<int> left;
        vector<int> right;
        int ans = 100001;
        left.push_back(0);
        right.push_back(0);
        bool r = false, l = false;;
        for (int i = 0, j = 0, k = 0; i < nums.size(); i++){
            if (nums[i] + left[j] <= x ){
                left.push_back(nums[i] + left[j]);
                if (nums[i] + left[j] == x) l = true;
                j++;
            }
            else l = true;
            if (nums[nums.size() - i - 1] + right[k] <= x){
                right.push_back(nums[nums.size() - i - 1] + right[k]);
                if (nums[nums.size() - i - 1] + right[k] == x) r = true;
                k++;
            }
            else r = true;
            if (l && r) break; 
        }
        for (int i = 0; i < left.size(); i++){
            for (int j = 0; j < right.size(); j++){
                if (left[i] + right[j] == x){
                    ans = min(ans, i + j);
                }
            }
        }
        if (ans != 0 && r && l) return ans;
        return -1;
    }
    string shortestBeautifulSubstring(string &s, int k) {
        int len = s.size();
        int min_len = len + 1;
        int num = 0;
        string cur;
        string ans = "";
        for (int right = 0, left = 0; right < len; right++){
            if (s[right] == '1') num++;
            if (num == k){
                cur = s.substr(left, right - left + 1);
                if (cur.size() < min_len) min_len =cur.size();
                else if (cur.size() == min_len) {
                    if (ans == "") ans = cur;
                    else ans = min(cur, ans);
                }
            }
            while(num >= k){
                if (s[left] != '1') left++;
                else{
                    if (num > k) {
                        left++;
                        
                    }
                    else{
                        cur = s.substr(left, right - left + 1);
                if (cur.size() < min_len) min_len =cur.size();
                else if (cur.size() == min_len) {
                    if (ans == "") ans = cur;
                    else ans = min(cur, ans);
                } 
                    }num--;
               
}
                
        }
    }
    return ans;
    }
};
int main() {
    Solution sol;
    string str ="100011001";
    string result = sol.shortestBeautifulSubstring(str, 3) ;
    cout << "result" << result << endl;
}