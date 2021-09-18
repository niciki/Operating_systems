#include "unistd.h"
#include "stdio.h"

int main(){
    int fd1[2];
    int fd2[2];
    string name_of_file;
    string s;
    if(pipe(fd1) == -1){
        std::cout << "Error during creating pipe1\n";
        return 1;
    }
    if(pipe(fd2) == -1){
        std::cout << "Error during creating pipe2\n";
        return 4;
    }
    int id = fork();
    switch(id){
        case -1:
            std::cout << "Error during creating fork\n";
            return 2;
        case 0:
            printf("[%d] It's child\n", getpid());
            close(fd1[1]);
            close(fd2[0]);
            read(fd1[0], &s, sizeof(string));
            if(65 <= int(s[0]) and int(s[0]) <= 90){
                fprintf(f, "%s", s);
            } else {
                write(fd2[1], &s, sizeof(string))
            }
            close(fd1[0]);
            close(fd2[1]);
        default:
            std::cout << "Print input name of file: ";
            std::cin >> name_of_file;
            FILE *f;
            if ((f = fopen(name_of_file, "w"))==NULL) {
                printf("Cannot open file.\n");
                return 3;
            }
            printf("[%d] It's parent. Child id: %d\n", getpid(), id);
            close(fd1[0]);
            close(fd2[1]);
            std::cin >> s;
            while(s){
                write(fd1[1], &s, sizeof(string));
                std::cin >> s;
            }
            read(fd2[0], &s, sizeof(string));
            std::cout << "Error in string " << s << "\n";
		    close(fd1[1]);
    }   
}
