#include "unistd.h"
#include "stdio.h"

int main(){
    int fd[2];
    string name_of_file;
    string s;
    cout << "Print input name of file: ";
    cin >> name_of_file;
    if(pipe(fd) == -1){
        cout << "Error during creating pipe\n";
        return 1;
    }
    int id = fork();
    switch(id){
        case -1:
            cout << "Error during creating fork\n";
            return 2;
        case 0:
            printf("[%d] It's child\n", getpid());
        default:
            printf("[%d] It's parent. Child id: %d\n", getpid(), id);
    }   
}
