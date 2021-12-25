#include <iostream>
#include <unistd.h>
#include "zmq.hpp"

// g++ main.cpp -lzmq

using namespace std;

int main(int argc, char* argv[]){
    cout << 2;
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REP);
    main_socket.connect(argv[1]);
    cout << "Starting\n";
    for(;;){
        zmq::message_t message;
        main_socket.recv(&message);
        string recieved_message(static_cast<char*>(message.data()), message.size());
        cout << recieved_message << "\n";
        sleep(1);
        string message_string = "world";
        zmq::message_t mes(message_string.size());
        memcpy(mes.data(), message_string.c_str(), message_string.size());
        printf("Sending: world - %ld\n", strlen(message_string.c_str()));
        main_socket.send(mes);
    }
}
