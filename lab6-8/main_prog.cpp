#include "zmq.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <signal.h>
#include <sstream>
#include <set>
#include <algorithm>

// g++ main_prog.cpp -lzmq -o main_prog -w

using namespace std;

int main(){
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REP);
    string adr = "tcp://127.0.0.1:300";
    string command;
    int child_id = 0;
    while(1){
        cout << "Please, enter command\n";
        cin >> command;
        if(command == "create"){
            if(child_id == 0){
                int id;
                cin >> id;
                int id_tmp = id - 1;
                while(1){
                    try {
                        main_socket.bind(adr + to_string(++id_tmp));
                        break;
                    } catch(...) {

                    }
                }
                string new_adr = adr + to_string(id_tmp);
                char* adr_ = new char[new_adr.size() + 1];
                memcpy(adr_, new_adr.c_str(), new_adr.size() + 1);
                char* id_ = new char[to_string(id).size() + 1];
                memcpy(id_, to_string(id).c_str(), to_string(id).size() + 1);
                char* args[] = {"./child_node", adr_, id_, NULL};
                int id2 = fork();
                if (id2 == -1) {
                    std::cout << "Unable to create first worker node\n";
                    id = 0;
                    exit(1);
                } else if(id2 == 0){
                    execv("./child_node", args);
                } else {
                    child_id = id;
                }
                zmq::message_t message;
                main_socket.recv(&message);
                string recieved_message(static_cast<char*>(message.data()), message.size());
                cout << recieved_message << "\n";
                delete [] adr_;
                delete [] id_;
            } else {
                int id;
                cin >> id;
                string message_string = command + " " + to_string(id);
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);

                // catch message from new node
                main_socket.recv(&message);
                string recieved_message(static_cast<char*>(message.data()), message.size());
                cout << recieved_message << "\n";
            }
        } else if(command == "exec"){
            int id, value;
            string name;
            cin >> id >> name;
            string s;
            char q;
            while((q = getchar()) != '\n'){
                s += q;
            }
            if(s == ""){
                string message_string = command + " " + to_string(id) + " " + name;
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);
                // return value from map
                main_socket.recv(&message);
                string recieved_message(static_cast<char*>(message.data()), message.size());
                cout << recieved_message << "\n";
            } else {
                value = stoi(s);
                string message_string = command + " " + to_string(id) + " " + name + " " + to_string(value);
                zmq::message_t message(message_string.size());
                memcpy(message.data(), message_string.c_str(), message_string.size());
                main_socket.send(message);
                // add new element to map
                main_socket.recv(&message);
                string recieved_message(static_cast<char*>(message.data()), message.size());
                cout << recieved_message << "\n";
            }
        } else if(command == "ping"){
            int id;
            cin >> id;
            string message_string = command + " " + to_string(id);
            zmq::message_t message(message_string.size());
            memcpy(message.data(), message_string.c_str(), message_string.size());
            main_socket.send(message);
            // receive answer from child
            main_socket.recv(&message);
            string recieved_message(static_cast<char*>(message.data()), message.size());
            cout << recieved_message << "\n";
        } else if(command == "kill"){
            int id;
            cin >> id;
            if(child_id == 0){
                cout << "Error: there isn't nodes\n";
            } else if(child_id == id){
                string kill_message = command + " " + to_string(id);
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                main_socket.recv(message);
                string received_message(static_cast<char*>(message.data()), message.size());
                cout << received_message << "\n";
                cout << "Tree deleted successfully\n";
                /*main_socket.close();
                context.close();*/
                return 0;
            } else {
                string kill_message = command + " " + to_string(id);
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                main_socket.recv(&message);
                string received_message(static_cast<char*>(message.data()), message.size());
                cout << received_message << "\n";
            }
        } else if(command == "exit"){
            if(child_id){
                string kill_message = "DIE";
                zmq::message_t message(kill_message.size());
                memcpy(message.data(), kill_message.c_str(), kill_message.size());
                main_socket.send(message);
                cout << "Tree was deleted\n";
            }
            main_socket.close();
            context.close();
            break;
        } else {
            cout << "Error: incorrect command\n";
        }
    }
}