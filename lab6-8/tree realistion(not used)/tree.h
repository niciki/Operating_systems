#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <map>
using namespace std;

struct node {
    node(int cur): id(cur), left(nullptr), right(nullptr){};
    int id;
    map<string, int> info;
    node* left;
    node* right;
};

struct tree {
    tree(){
        node* tmp = nullptr;
        root = tmp;
    }
    tree(int cur){
        node* tmp = new node(cur);
        root = tmp;
    }
    bool is_in_tree(int cur){
        node* tmp = root;
        while(tmp != nullptr){
            if(tmp->id > cur){
                tmp = tmp->left; 
            } else if(tmp->id == cur){
                return true;
            } else {
                tmp = tmp->right;
            }
        }
        return false;
    }
    void add(int cur){
        if(root == nullptr){
            root = new node(cur);
        } else {
            node* tmp = root;
            while(tmp != nullptr){
                if(tmp->id > cur){
                    if(tmp->left == nullptr){
                        node* n_ = new node(cur);
                        tmp->left = n_;
                        break;    
                    } else {
                        tmp = tmp->left;
                    }
                } else {
                    if(tmp->right == nullptr){
                        node* n_ = new node(cur);
                        tmp->right = n_;
                        break; 
                    } else {
                        tmp = tmp->right;
                    }
                }
            }
        }
    }
    vector<node*> get_node_list(node* root){
        if(root != nullptr){
            vector<node*> v, tmp1, tmp2;
            v.push_back(root);
            tmp1 = get_node_list(root->left);
            tmp2 = get_node_list(root->right);
            v.insert(v.end(), tmp1.begin(), tmp1.end());
            v.insert(v.end(), tmp2.begin(), tmp2.end());
            return v;
        }
        return vector<node*>();
    }
    void print_(node* root, int i){
        if(root != nullptr && i == 0){
            cout << root->id << ": " ;
            print_(root->left, i + 1);
            print_(root->right, i + 1);
        } else if(root != nullptr){
            cout << root->id << ", " ;
            print_(root->left, i + 1);
            print_(root->right, i + 1);
        }
    }
    void print(){
        vector<node*> list_nodes = get_node_list(root);
        for(const auto& i: list_nodes){
            print_(i, 0);
            cout << "\n";
        }
    }
    void del_(node* n){
        if(n != nullptr){
            del_(n->left);
            del_(n->right);
            delete n;
        }
    }
    void del(int id){
        node* tmp = root;
        while(tmp != nullptr && tmp->id != id){
            if(tmp->id > id){
                if(tmp->left->id != id){
                    tmp = tmp->left;
                } else {
                    node* tmp_2;
                    tmp_2 = tmp;
                    tmp = tmp->left;
                    tmp_2->left = nullptr;
                }
            } else if(tmp->id < id){
                if(tmp->right->id != id){
                    tmp = tmp->right;
                } else {
                    node* tmp_2;
                    tmp_2 = tmp;
                    tmp = tmp->right;
                    tmp_2->right = nullptr;
                }
            }
        }
        if(root == nullptr){
            cout << "Error:id: Not found\n";
        } else {
            del_(tmp);
        }
    }
    node* root = nullptr;
};

#endif
