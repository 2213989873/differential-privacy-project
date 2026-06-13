#include<iostream>
using namespace std;
class adder {
    public:
    int operator()(int a, int b) {
        return a + b;
    }
};
int main() {
    std::cout << "Hello, World!" << std::endl;
    adder a;
    cout << a(2, 3) << endl;
    
    return 0;
}
