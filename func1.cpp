#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// rectangle
extern "C" float Square(float A, float B) {
    return A * B;
}

// binary
extern "C" void translation(long x, char * c){
    vector<int> v;
    if(x == 0){
        c[0] = '0';
        return;
    } else {
        while(x){
            v.push_back(x % 2);
            x /= 2;
        }
    }
    for(int i = v.size() - 1; i >= 0; --i){
        c[v.size() - i - 1] = static_cast<char>(v[i] + '0');
    }
}