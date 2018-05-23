#include <iostream>
#include <string>
using namespace std;

int count = 0;
struct node_t {
    int value;
    node_t * next;
};

void add(node_t * & head, int value) {
    node_t * node = new node_t;
    node->value = value;
    node->next = nullptr;
    if( head == nullptr) {
        head = node;
    }
    else {
        node_t * last = head;
        while(last->next) {
            last = last->next;
        }
        last->next = node;
    }
    count++;
}

void away(node_t * & list) {
    node_t * first = list;
    list = list->next;
    delete first;
    count--;
}

void reverse(node_t * list) {
    node_t * begin = list;
    node_t * last = begin;
    for (int j = 0; j < count / 2 ; j++) {
        for (int i = j; i < count - 1 - j; i++) {
            last = last->next;
        }
        int saved = begin->value;
        begin->value = last->value;
        last->value = saved;
        begin = begin->next;
        last = begin;
    }
}

void print(node_t * first, int count) {
    node_t *print = first;
    for (int i = 0; i < count; i++) {
        cout << "+---+    ";
    }
    cout << endl;
    for (int i = 0; i < count; i++) {
        if (i != 0) {
            cout << "--->";
        }
        cout << "| " << print->value << " |";
        print = print->next;
    }
    cout << endl;
    for (int i = 0; i < count; i++) {
        cout << "+---+    ";
    }
    cout << endl;
}
void delet(node_t * head) {
    while(head) {
        delete head;
        head = head->next;
    }
}
