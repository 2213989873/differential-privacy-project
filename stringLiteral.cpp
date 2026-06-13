#include <iostream>
using namespace std;
int main() {
    const char* str = "Hello World";  // C++ 字符串字面量
    std::cout << str << std::endl;
    std::cout << *(str + 1) << std::endl;  // 访问第一个字符 'H'
    cout << &str << endl;  // 打印指针变量的地址
    const char* str1 = str - 1;
    cout << &str1 << endl;

    return 0;
}