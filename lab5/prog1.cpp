#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

extern "C" float Square(float A, float B);
extern "C" void translation(long n, char* c);

int main(){
    int n;
    while(cin >> n) {
        switch(n) {
            case 1: {
                int a, b;
                cin >> a >> b;
                if(a <= 0 || b <= 0){
                    cout << "Please input correct data!\n";
                    break;
                }
                cout << Square(a, b) << '\n';
                break;
            } case 2: {
                int z;
                cin >> z;
                if(z < 0){
                    cout << "Please input correct data!\n";
                    break;
                }
                int size = 0;
                if(z == 0){
                    size = 1; 
                } else{
                    size = ceil(log2(z));
                    if(int(log2(z)) == (log2(z)) || z == 1){
                        ++size;
                    }
                }
                char c[size];
                translation(z, c);
                for(int i = 0; i < size; i++){
                    cout << c[i];
                }
                cout  << '\n';
                break;
            } default: {
                cout << "Please input correct key!\n";
                break;
            }
        }
    }
}
