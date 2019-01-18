#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include "./monitor.h"
#include <iostream>
#include <list>
#include <string>

using namespace std;


class MonitorQueue : Monitor {
 public:
    explicit MonitorQueue(string name_in) {
        count = 0;
        size = 10;
        name = name_in;
    }

    // Add petent to the buffer. When cp equals to false the petent is added
    // to the back of the queue. When cp equals to true this function add
    // petents of value 1 to the front of the queue and rest of the
    // petents to the back.
    void add_petent(int petent, Semaphore *sem_print, bool print_c = false, bool cp = false) {
        enter();  // enter critical section

        // If buffer is full, put current thread to sleep on full variable.
        if (count == size)
            wait(full);

        if (cp == true) {
            // If consumer-producer thread.
            if (petent == 1)
                buffer.push_front(petent);
            else
                buffer.push_back(petent);
        } else {
            buffer.push_back(petent);
        }

        
        count = count + 1;  // increase number of petents in the queue

        if (print_c == true) {
            sem_print->p();  // down mutex print
            cout << "Enqueued to   " << name << ": " << petent;
            print_content();  // print content of the queue
            sem_print->v();  // up mutex print
        }

        // Wake up any thread sleeping on empty variable,
        // because there is a new petent in the buffer.
        signal(empty);

        leave();  // leave critical section
    }

    // Remove petent from the front of the queue and return it.
    int remove_petent(Semaphore *sem_print, bool print_c = false) {
        enter();  // enter critical section

        // If buffer is empty, put current thread to sleep on empty variable.
        if (count == 0)
            wait(empty);

        int petent = buffer.front();  // get the petent from the front of the queue
        buffer.pop_front();  // remove petent from the queue
        count = count - 1;  // decrease number of petents in the queue

        if (print_c == true) {
            sem_print->p();  // down mutex print
            cout << "Dequeued from " << name << ": " << petent;
            print_content();  //print content of the queue
            sem_print->v();  // up mutex print
        }

        // Wake up any thread sleeping on full variable,
        // because we removed one petent from the buffer.
        signal(full);

        leave();  // leave critical section
        return petent;
    }

    // If queue is empty return true, otherwise false.
    bool is_queue_empty() {
        if (count == 0)
            return true;
        else
            return false;
    }

 private:
    list<int> buffer;      // shared memory buffer
    int count;             // actual number of items in the buffer
    int size;              // size of the buffer
    string name;           // name of the buffer
    Condition empty, full;

    // Print content of the buffer.
    void print_content() {
        // Print formatted content of the buffer.
        cout << " | ";
        cout << name << ": ";
        for (int n : buffer) {
            cout << n << " ";
        }
        cout << '\n';
    }
};

#endif  // MONITOR_HPP_
