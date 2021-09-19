#include <iostream>
#include <fstream>
#include "unistd.h"

int main(){
    int fd1[2];
    int fd2[2];
    std::string name_of_file;
    std::string s;
    std::cout << "Print input name of file: ";
    std::cin >> name_of_file;
    std::ofstream f;
    f.open(name_of_file);
    if(pipe(fd1) == -1){
        std::cout << "Error during creating pipe1\n";
        return 1;
    }
    if(pipe(fd2) == -1){
        std::cout << "Error during creating pipe2\n";
        return 4;
    }
    int id = fork();
    int size;
    //std::cout << id << "\n";
    switch(id){
        case -1:
            std::cout << "Error during creating fork\n";
            return 2;
            break;
        case 0:
            printf("[%d] It's child\n", getpid());
            close(fd1[1]);
            close(fd2[0]);
            while(read(fd1[0], &size, sizeof(int))){
                s.resize(size);
                read(fd1[0], s.data(), size * sizeof(char));
                if(65 <= int(s[0]) and int(s[0]) <= 90){
                    std::cout << s << "\n";
                    f << s << "\n";
                } else {
                    int size = s.length();
                    write(fd2[1], &size, sizeof(int));
                    write(fd2[1], s.data(), sizeof(char) * size);
                }
            }
            close(fd1[0]);
            close(fd2[1]);
            break;
        default:
            printf("[%d] It's parent. Child id: %d\n", getpid(), id);
            close(fd1[0]);
            close(fd2[1]);
            while(std::cin >> s && s != "quit"){
                size = s.length();
                write(fd1[1], &size, sizeof(int));
                write(fd1[1], s.data(), (size * sizeof(char)));
            }
            close(fd1[1]);
            while(read(fd2[0], &size, sizeof(int))){
                s.resize(size);
                read(fd2[0], s.data(), (size * sizeof(char)));
                std::cout << "Error in string " << s << "\n";
            }
            close(fd2[0]);
            f.close();
            return 0;
    }   
}
