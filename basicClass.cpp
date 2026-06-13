#include<iostream>
#include<string>
using namespace std;
class man {
    friend int bim(const man& m);
public:
    man(string name, float a, float b, int c) : 
    name(name),
    height(a), 
    weight(b),
    age(c) {}
private:
    string name;
    int age;
    float  height, weight;
public:
    void display() {
        cout << "This is a basic class." << endl;
    }
};
int bim(const man& m) {
    return m.weight / ((m.height * m.height) / 10000);
}
int main() {
    man kobe("Kobe Bryant", 198, 96, 41);
    int bmi = bim(kobe);
    cout << "BMI: " << bmi << endl;
}