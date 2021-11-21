#include <iostream>
#include <vector>
#include <cmath>
#include <dlfcn.h>
#include <cstring>

using namespace std;

int main(){
    float (*Square)(float, float);
    void (*translation)(long, char*);
    string name_1("libd1.so");
    string name_2("libd2.so");
    char* name1 = &name_1[0];
    char* name2 = &name_2[0];
    void* handle = dlopen(name2, RTLD_LAZY);
    if(!handle){
        cout << "Error during uploading dynamic library\n";
        return 1;
    }
    Square = (float(*)(float, float))dlsym(handle, "Square");
    char* error = dlerror();
    if(error){
        cout << error << '\n';
        return 2;
    }
    translation = (void(*)(long, char*))dlsym(handle, "translation");
    error = dlerror();
    if(error){
        cout << error << '\n';
        return 2;
    }
    bool f = true;
    int n;
    while(cin >> n) {
        switch(n) {
            case 0:
                if(f){
                    dlclose(handle);
                    char* error = dlerror();   
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    handle = dlopen(name1, RTLD_LAZY);
                    if(!handle){
                        cout << "Error during uploading dynamic library\n";
                        return 1;
                    }
                    Square = (float(*)(float, float))dlsym(handle, "Square");
                    error = dlerror();
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    translation = (void(*)(long, char*))dlsym(handle, "translation");
                    error = dlerror();
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    f = false;
                } else {
                    dlclose(handle);
                    char* error = dlerror();   
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    handle = dlopen(name2, RTLD_LAZY);
                    if(!handle){
                        cout << "Error during uploading dynamic library\n";
                        return 1;
                    }
                    Square = (float(*)(float, float))dlsym(handle, "Square");
                    char* error = dlerror();
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    translation = (void(*)(long, char*))dlsym(handle, "translation");
                    error = dlerror();
                    if(error){
                        cout << error << '\n';
                        return 2;
                    }
                    f = true;
                }
                break;
            case 1:
                int a, b;
                cin >> a >> b;
                cout << Square(a, b) << '\n';
                break;
            case 2: {
                int z;
                cin >> z;
                int size;
                if(z == 0){
                    size = 1; 
                } else{
                    size = ceil(log2(z)/log2(3));
                    if(int(log2(z)/log2(3)) == (log2(z)/log2(3)) || z == 1){
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
            }
            default:
                cout << "Error, please enter correct number\n";
        }
    }
}