#include <stdio.h>
#include <stdlib.h>      /* rand() and srand() functions        */
#include <time.h>        /* nanosleep(), etc.                   */
#include <thread>

#include "monitor.hpp"

using namespace std;

Semaphore sem_print(1);  // mutex to access print console

MonitorQueue AB("AB");
MonitorQueue AC("AC");
MonitorQueue BD("BD");
MonitorQueue CD("CD");

Semaphore sem_buss_BD(0);  // semaphore for number of businessman in the queue
Semaphore sem_buss_CD(0);  // semaphore for number of businessman in the queue


// Producer loop for given queue.
void producer(MonitorQueue &queue, Semaphore &sem_print) {
    int petent;
    for (int i = 0; i < 10; i++) {
        // Generate petent.
        petent = rand() % 2; 

        // Add petent to queue.
        queue.add_petent(petent, &sem_print, true, false);

        sleep(1);
    }
}

// Consumer loop for the given queue.
void consumer(MonitorQueue &queue_1, MonitorQueue &queue_2, Semaphore &sem_print) {
    int petent;
    int num_buss_BD;
    int num_buss_CD;

    for (int i = 0; i < 20; i++) {
        num_buss_BD = sem_buss_BD.get_value();  // current value of bus in the queue
        num_buss_CD = sem_buss_CD.get_value();  // current value of bus in the queue
        if (num_buss_BD != 0 || num_buss_CD != 0) {
            if (num_buss_BD > num_buss_CD) {
                // If there are more businessmen in the BD, remove petent from it.
                petent = queue_1.remove_petent(&sem_print, true);
                sem_buss_BD.p();  // decrease number of businessmen in the queue.
            } else {
                // If there are more businessmen in the CD, remove petent from it.
                petent = queue_2.remove_petent(&sem_print, true);
                sem_buss_CD.p();  // decrease number of businessmen in the queue.
            }
        } else {
            if (queue_1.is_queue_empty() == false)
                // If queue_1 is not empty, remove petent.
                petent = queue_1.remove_petent(&sem_print, true);
            if (queue_2.is_queue_empty() == false)
                // If queue_w is not empty, remove petent.
                petent = queue_2.remove_petent(&sem_print, true);
        }
        sleep(1);
    }
}


// Consumer-producer loop for given queues.
// Remove item from the queue_cons and add it to queue_prod.
void consumer_producer(MonitorQueue &queue_cons, MonitorQueue &queue_prod, string name_prod, Semaphore &sem_print) {
    int petent;
    for (int i = 0; i < 10; i++) {
        // Remove petent from queue_cons.
        petent = queue_cons.remove_petent(&sem_print,true);

        // Add removed petent to the queue_prod.
        queue_prod.add_petent(petent, &sem_print, true, true);

        // If added petent was businessman raise value of semaphore.
        if (petent == 1 && name_prod == "BD") {
            sem_buss_BD.v();
        } else if (petent == 1 && name_prod == "CD"){
            sem_buss_CD.v();
        }
        
        sleep(1);
    }
}


int main() {
    srand(time(NULL));
    
    thread t1(producer, ref(AB), ref(sem_print));
    thread t2(producer, ref(AC), ref(sem_print));
    sleep(11);
    thread t3(consumer_producer, ref(AB), ref(BD), "BD", ref(sem_print));
    sleep(3);
    thread t4(consumer_producer, ref(AC), ref(CD), "CD", ref(sem_print));
    sleep(3);
    thread t5(consumer, ref(BD), ref(CD), ref(sem_print));

    // Wait for every thread to finish it's functions.
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}
