#include <vector>
using namespace std;
class Solution {
public:
    int trap(vector<int>& height) {
        //只蓄水平层
        int ans = 0;
        int real = 0, fake0 = 0, fake1 = 0;
        for (int i = 0; i < height.size() - 2; i++){
            fake0 = 0;
            fake1 = 0;
            real = 0;
            int real_fake = 0;
            for (int j = i + 1; j < height.size(); j++){
                //将可能的水算到fake中去
                if (height[j] < height[i]){
                    fake0 = height[i] - height[j];
                    real_fake += fake0;
                }
                else{
                    break;
                }
                
                if(j + 1 < height.size()){
                if (height[j + 1] < height[i]){
                    fake1 = height[i] - height[j + 1];

                    if (fake0 > fake1) real += fake0 -fake1;
                }
                else{
                    real = real_fake;
                    i = j + 1;
                }
            }

                ans += real; 
            }
        }
        return ans;
    }
};
int main(){
    Solution s;
    vector<int> height = {4,2,0,3,2,5};
    s.trap(height);
}