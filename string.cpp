#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;
class Solution {
public:
    int maxScore(string s) {
        int n = s.size();
        int count = 0;
        for (int t = 0; t < n - 1; t++){
            int current = 0;
            for (int i = 0; i <= t; i++){
                if (s[i] == '0') current++;
            }
            for (int i = t + 1; i < n; i++){
                if (s[i] == '1') current++;
            }
            if (current > count ) count = current;
        }

        return count;
    }
     int vowelStrings(vector<string>& words, int left, int right) {
        int count = 0;
        unordered_map<char, int> aeiou = {{'a', 1}, {'e', 1},{'i', 1},{'o', 1},{'u', 1}};
        for (int i = left; i <= right; i++){        
            if (aeiou.find(words[i][0]) != aeiou.end() && aeiou.find(words[i].back()) != aeiou.end()) count++;
        }
        return count;
    }
      int maxVowels(const string& s, int k) {
        unordered_map<char, int> aeiou ={
            {'a',1},
            {'e',1},
            {'i',1},
            {'o',1},
            {'u',1}
        };
        int count = 0;
        for (int i = 0; i < k; i++){
          if (aeiou.count(s[i])) count++;
        }
        int current = count;
        for (int j = 0; j<= s.size() - k - 1; j++){
            if (aeiou.count(s[j])) current--;
            if (aeiou.count(s[j + k])) current++;
            if (current > count) count = current;

        }
        return count;
    }
     double findMaxAverage(vector<int>& nums, int k) {
        double average = 0;
        double plus = 0;
        for (int i = 0; i < k; i++) {
            plus += nums[i];
        }
        average = plus / k;
        if (nums.size() == k) return average;
        double current = average;    
        for (int i = 0; i <= nums.size() - k - 1; i++) {
            current = current - (double)nums[i]/k + (double)nums[i+k]/k;
            if (current > average) average = current;
        }
        return average;
    }
};
int main() {
    Solution s;
    string str;
    int k;
    double avg ;
    vector<int> nums = {1,12,-5,-6,50,3};
    avg = s.findMaxAverage(nums,4);
    cout << avg << endl;
    return 0;
}