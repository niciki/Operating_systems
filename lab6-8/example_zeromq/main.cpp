#include <iostream>
#include <unistd.h>
#include "zmq.hpp"
#include <unistd.h>

// g++ main.cpp -lzmq

using namespace std;

int main(){
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REQ);
    string adr = "tcp://127.0.0.1:300003";
    main_socket.bind("tcp://127.0.0.1:300003");
    cout << "Starting\n";
    char* args[] = {"./child_node", "tcp://127.0.0.1:300003", NULL};
    int id = fork();
    if(id == 0){
        execv("./child", args);
    } else {
        for(;;){
            string message_string = "hello";
            zmq::message_t message(message_string.size());
            memcpy(message.data(), message_string.c_str(), message_string.size());
            printf("Sending: hello - %ld\n", strlen(message_string.c_str()));
            main_socket.send(message);
            sleep(2);
            main_socket.recv(&message);
            string recieved_message(static_cast<char*>(message.data()), message.size());
            cout << recieved_message << "\n";
        }
    }
}
