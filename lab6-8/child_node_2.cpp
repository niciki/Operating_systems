#include "zmq.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <map>

// g++ child_node_2.cpp -lzmq -o child_node -w

using namespace std;

void send_message(string message_string, zmq::socket_t& socket){
    zmq::message_t message_back(message_string.size());
    memcpy(message_back.data(), message_string.c_str(), message_string.size());
    if(!socket.send(message_back)){
        cout << "Error: can't send message from node with pid " << getpid() << "\n";
    }
}


int main(int argc, char * argv[]){
    string adr = argv[1];
    zmq::context_t context(1);
    zmq::socket_t main_socket(context, ZMQ_REQ);
    main_socket.connect(argv[1]);
    send_message("OK: " + to_string(getpid()), main_socket);
    int id = stoi(argv[2]); // id of this node
    map<string, int> m;
    int left_id = 0;
    int right_id = 0;
    zmq::context_t context_l(1);
    zmq::context_t context_r(1);
    zmq::socket_t left_socket(context_l, ZMQ_REP);
    string adr_left = "tcp://127.0.0.1:300";
    zmq::socket_t right_socket(context_r, ZMQ_REP);
    string adr_right = "tcp://127.0.0.1:300";
    while(1){
        zmq::message_t message_main;
        main_socket.recv(&message_main);
        string recieved_message(static_cast<char*>(message_main.data()), message_main.size());
        string command;
        for(int i = 0; i < recieved_message.size(); ++i){
            if(recieved_message[i] != ' '){
                command += recieved_message[i];
            } else {
                break;
            }
        }
        if(command == "exec"){
            int id_proc; // id of node for adding
            string id_proc_, value_;
            string key;
            int value;
            for(int i = 5; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    id_proc_ += recieved_message[i];
                } else {
                    break;
                }
            }
            id_proc = stoi(id_proc_);
            if(id_proc == id){ // id == proc_id
                for(int i = 6 + id_proc_.size(); i < recieved_message.size(); ++i){
                    if(recieved_message[i] != ' '){
                        key += recieved_message[i];
                    } else {
                        break;
                    }
                }
                for(int i = 7 + id_proc_.size() + key.size(); i < recieved_message.size(); ++i){
                    if(recieved_message[i] != ' ' || recieved_message[i] != '\n'){
                        value_ += recieved_message[i];
                    } else {
                        break;
                    }
                }
                if(value_ == ""){
                    if(m.count(key)){
                        int value_map = m[key];
                        send_message("OK:" + id_proc_ + ":" + to_string(m[key]), main_socket);
                    } else {
                        cout << key;
                        send_message("OK:" + id_proc_ + ": \'" + key + "\' not found", main_socket);
                    }
                } else {
                    m[key] = stoi(value_);
                    send_message("OK:" + id_proc_, main_socket);
                }
            } else {
                if(id > id_proc){
                    if(left_id == 0){ // if node not exists
                        string message_string = "Error:id: Not found";
                        send_message("Error:id: Not found", main_socket);
                    } else {
                        zmq::message_t message(recieved_message.size());
                        memcpy(message.data(), recieved_message.c_str(), recieved_message.size());
                        if(!left_socket.send(message)){
                            cout << "Error: can't send message to left node from node with pid: " << getpid() << "\n";
                        }
                        // catch and send to parent
                        if(!left_socket.recv(&message)){
                            cout << "Error: can't receive message from left node in node with pid: " << getpid() << "\n";
                        }
                        if(!main_socket.send(message)){
                            cout << "Error: can't send message to main node from node with pid: " << getpid() << "\n";
                        }
                    }
                } else {
                    if(right_id == 0){ // if node not exists
                        string message_string = "Error:id: Not found";
                        zmq::message_t message(message_string.size());
                        memcpy(message.data(), message_string.c_str(), message_string.size());
                        if(!main_socket.send(message)){
                            cout << "Error: can't send message to main node from node with pid: " << getpid() << "\n";
                        }
                    } else {
                        zmq::message_t message(recieved_message.size());
                        memcpy(message.data(), recieved_message.c_str(), recieved_message.size());
                        if(!right_socket.send(message)){
                            cout << "Error: can't send message to right node from node with pid: " << getpid() << "\n";
                        }
                        // catch and send to parent
                        if(!right_socket.recv(&message)){
                            cout << "Error: can't receive message from left node in node with pid: " << getpid() << "\n";
                        }
                        if(!main_socket.send(message)){
                            cout << "Error: can't send message to main node from node with pid: " << getpid() << "\n";
                        }
                    }
                }
            }
        } else if(command == "create"){
            ofstream o;
            o.open("text.txt", std::ios::app);
            o << "Create:" << left_id << " " << id << " " << right_id << "\n";
            o.flush();
            int id_proc; // id of node for creating
            string id_proc_;
            for(int i = 7; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    id_proc_ += recieved_message[i];
                } else {
                    break;
                }
            }
            id_proc = stoi(id_proc_);
            if(id_proc == id){
                send_message("Error: Already exists", main_socket);
            } else if(id_proc > id){
                if(right_id == 0){ // there is not right node
                    right_id = id_proc;
                    int right_id_tmp = right_id - 1;
                    while(1){
                        try {
                            right_socket.bind(adr_right + to_string(++right_id_tmp));
                            break;
                        } catch(...) {

                        }
                    }
                    adr_right += to_string(right_id_tmp);
                    char* adr_right_ = new char[adr_right.size() + 1];
                    memcpy(adr_right_, adr_right.c_str(), adr_right.size() + 1);
                    char* right_id_ = new char[to_string(right_id).size() + 1];
                    memcpy(right_id_, to_string(right_id).c_str(), to_string(right_id).size() + 1);
                    char* args[] = {"./child_node", adr_right_, right_id_, NULL};
                    int f = fork();
                    if(f == 0){
                        execv("./child_node", args);
                    } else if (f == -1){
                        cout << "Error in forking in node with pid: " << getpid() << "\n";
                    } else {
                        // catch message from new node
                        zmq::message_t message_from_node;
                        if(!right_socket.recv(&message_from_node)){
                            cout << "Error: can't receive message from right node in node with pid:" << getpid() << "\n";
                        }
                        string recieved_message_from_node(static_cast<char*>(message_from_node.data()), message_from_node.size());
                        /*ofstream op;
                        op.open("text.txt", std::ios::app);
                        op << "Received:" << recieved_message_from_node << "\n";
                        op.flush();*/
                        // send message to main node
                        if(!main_socket.send(message_from_node)){
                            cout << "Error: can't send message to main node from node with pid:" << getpid() << "\n";
                        }
                    }
                    delete [] adr_right_;
                    delete [] right_id_;
                } else { // send task to right node
                    send_message(recieved_message, right_socket);
                    // catch and send to parent
                    zmq::message_t message;
                    if(!right_socket.recv(&message)){
                        cout << "Error: can't receive message from left node in node with pid: " << getpid() << "\n";
                    }
                    if(!main_socket.send(message)){
                        cout << "Error: can't send message to main node from node with pid: " << getpid() << "\n";
                    }
                }
            } else {
                if(left_id == 0){ // there is not left node
                    left_id = id_proc;
                    int left_id_tmp = left_id - 1;
                    while(1){
                        try {
                            left_socket.bind(adr_left + to_string(++left_id_tmp));
                            break;
                        } catch(...) {

                        }
                    }
                    adr_left += to_string(left_id_tmp);
                    char* adr_left_ = new char[adr_left.size() + 1];
                    memcpy(adr_left_, adr_left.c_str(), adr_left.size() + 1);
                    char* left_id_ = new char[to_string(left_id).size() + 1];
                    memcpy(left_id_, to_string(left_id).c_str(), to_string(left_id).size() + 1);
                    char* args[] = {"./child_node", adr_left_, left_id_, NULL};
                    int f = fork();
                    if(f == 0){
                        execv("./child_node", args);
                    } else if(f == -1){
                        cout << "Error in forking in node with pid: " << getpid() << "\n";
                    } else {
                        // catch message from new node
                        zmq::message_t message_from_node;
                        if(!left_socket.recv(&message_from_node)){
                            cout << "Error: can't receive message from left node in node with pid:" << getpid() << "\n";
                        }
                        string recieved_message_from_node(static_cast<char*>(message_from_node.data()), message_from_node.size());
                        // send message to main node
                        if(!main_socket.send(message_from_node)){
                            cout << "Error: can't send message to main node from node with pid:" << getpid() << "\n";
                        }
                    }
                    delete [] adr_left_;
                    delete [] left_id_;
                } else { // send task to left node
                    send_message(recieved_message, left_socket);
                    // catch and send to parent
                    zmq::message_t message;
                    if(!left_socket.recv(&message)){
                        cout << "Error: can't receive message from left node in node with pid: " << getpid() << "\n";
                    }
                    if(!main_socket.send(message)){
                        cout << "Error: can't send message to main node from node with pid: " << getpid() << "\n";
                    }
                }
            }
        } else if(command == "ping") {
            /*ofstream o;
            o.open("text.txt", std::ios::app);
            o << "Ping: " << left_id << " " << id << " " << right_id << "\n";
            o.flush();*/
            int id_proc; // id of node for creating
            string id_proc_;
            for(int i = 5; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    id_proc_ += recieved_message[i];
                } else {
                    break;
                }
            }
            id_proc = stoi(id_proc_);
            if(id_proc == id){
                send_message("OK: 1", main_socket);
            } else if(id_proc < id) {
                if(left_id == 0){
                    send_message("OK: 0", main_socket);
                } else {
                    left_socket.send(message_main);
                    zmq::message_t answ;
                    left_socket.recv(&answ);
                    main_socket.send(answ);
                }
            } else if(id_proc > id) {
                if(right_id == 0){
                    send_message("OK: 0", main_socket);
                } else {
                    right_socket.send(message_main);
                    zmq::message_t answ;
                    right_socket.recv(&answ);
                    main_socket.send(answ);
                }
            }
        } else if(command == "kill") {
            /*ofstream o;
            o.open("text.txt", std::ios::app);
            o << "Kill:" << left_id << " " << id << " " << right_id << "\n";
            o.flush();*/
            int id_proc; // id of node for killing
            string id_proc_;
            for(int i = 5; i < recieved_message.size(); ++i){
                if(recieved_message[i] != ' '){
                    id_proc_ += recieved_message[i];
                } else {
                    break;
                }
            }
            id_proc = stoi(id_proc_);
            if(id_proc > id){
                if(right_id == 0){
                    send_message("Error: there isn`t node with this id", main_socket);
                } else {
                    if(right_id == id_proc){
                        send_message("Ok: " + to_string(right_id), main_socket);
                        send_message("DIE", right_socket);
                        right_socket.unbind(adr_right);
                        adr_right = "tcp://127.0.0.1:300";
                        right_id = 0;
                    } else {
                        right_socket.send(message_main);
                        zmq::message_t message;
                        right_socket.recv(&message);
                        main_socket.send(message);
                    }
                }
            } else if(id_proc < id){
                if(left_id == 0){
                    send_message("Error: there isn`t node with this id", main_socket);
                } else {
                    if(left_id == id_proc){
                        send_message("Ok: " + to_string(left_id), main_socket);
                        send_message("DIE", left_socket);
                        left_socket.unbind(adr_left);
                        adr_left = "tcp://127.0.0.1:300";
                        left_id = 0;
                    } else {
                        left_socket.send(message_main);
                        zmq::message_t message;
                        left_socket.recv(&message);
                        main_socket.send(message);
                    }
                }
            }
        } else if (command == "DIE") {
            if (left_id){
                send_message("DIE", left_socket);
                left_socket.unbind(adr_left);
                adr_left = "tcp://127.0.0.1:300";
                left_id = 0;
            }
            if (right_id){
                send_message("DIE", right_socket);
                right_socket.unbind(adr_right);
                adr_right = "tcp://127.0.0.1:300";
                right_id = 0;
            }
            main_socket.unbind(adr);
            return 0;
        }
    }
}