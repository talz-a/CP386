#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_WAITING_CUSTOMERS 5
#define NUM_CUSTOMERS 10

// Mutex and condition variables for synchronization
pthread_mutex_t mutex;
pthread_cond_t barber_cond;
pthread_cond_t customer_cond;

// Variables to track the state
int waiting_customers = 0;        // Number of customers waiting
int customers_remaining = NUM_CUSTOMERS; // Number of customers left to process
int barber_sleeping = 0;          // Barber's sleeping status

// Circular queue to represent waiting room chairs
int waiting_room[MAX_WAITING_CUSTOMERS];
int front = 0, rear = 0;

void *barber(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // If there are no customers, barber goes to sleep
        while (waiting_customers == 0) {
            if (customers_remaining == 0) {
                // No more customers will arrive
                printf("Barber is closing shop.\n");
                pthread_mutex_unlock(&mutex);
                return NULL;
            }

            barber_sleeping = 1;
            printf("Barber is sleeping.\n");
            pthread_cond_wait(&barber_cond, &mutex);
            barber_sleeping = 0;
        }

        // Serve the next customer
        int customer_id = waiting_room[front];
        front = (front + 1) % MAX_WAITING_CUSTOMERS;
        waiting_customers--;

        printf("Barber is cutting hair of customer %d.\n", customer_id);

        // Signal the customer that the barber is ready
        pthread_cond_signal(&customer_cond);

        pthread_mutex_unlock(&mutex);

        // Simulate hair cutting
        sleep(2);

        printf("Barber finished cutting hair of customer %d.\n", customer_id);

        // Customer will decrement customers_remaining after haircut
    }
    return NULL;
}

void *customer(void *arg) {
    int customer_id = *(int *)arg;
    free(arg); // Free the allocated memory immediately

    pthread_mutex_lock(&mutex);

    if (waiting_customers < MAX_WAITING_CUSTOMERS) {
        // Add customer to the waiting room
        waiting_room[rear] = customer_id;
        rear = (rear + 1) % MAX_WAITING_CUSTOMERS;
        waiting_customers++;

        printf("Customer %d is waiting. Waiting customers: %d\n", customer_id, waiting_customers);

        // Wake up the barber if he is sleeping
        if (barber_sleeping) {
            printf("Customer %d wakes up the barber.\n", customer_id);
            pthread_cond_signal(&barber_cond);
        }

        // Wait until the barber is ready
        pthread_cond_wait(&customer_cond, &mutex);

        printf("Customer %d is getting a haircut.\n", customer_id);

        pthread_mutex_unlock(&mutex);

        // Simulate hair cutting (already simulated in barber thread)
        sleep(2);

        // After haircut is done
        pthread_mutex_lock(&mutex);
        customers_remaining--;
        // Signal the barber in case he's waiting to check for remaining customers
        pthread_cond_signal(&barber_cond);
        pthread_mutex_unlock(&mutex);

    } else {
        // No chairs available; customer leaves
        printf("No available chairs. Customer %d leaves.\n", customer_id);

        customers_remaining--;

        // Signal the barber in case he's waiting to check for remaining customers
        pthread_cond_signal(&barber_cond);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t barber_thread;
    pthread_t customer_threads[NUM_CUSTOMERS];

    // Initialize mutex and condition variables
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&barber_cond, NULL);
    pthread_cond_init(&customer_cond, NULL);

    // Seed the random number generator
    srand(time(NULL));

    // Create barber thread
    pthread_create(&barber_thread, NULL, barber, NULL);

    // Create customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        // Simulate random arrival times
        sleep(rand() % 3);

        int *customer_id = malloc(sizeof(int));
        *customer_id = i + 1;
        pthread_create(&customer_threads[i], NULL, customer, customer_id);
    }

    // Wait for all customer threads to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    // Wait for the barber thread to finish
    pthread_join(barber_thread, NULL);

    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&barber_cond);
    pthread_cond_destroy(&customer_cond);

    printf("All customers have been served or left. Barber shop is closing.\n");
    return 0;
}
