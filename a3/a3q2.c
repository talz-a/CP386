#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_WAITING_CUSTOMERS 5
#define NUM_CUSTOMERS 10

pthread_mutex_t mutex;
pthread_cond_t barber_cond;
pthread_cond_t customer_cond;

int waiting_customers = 0;
int customers_remaining = NUM_CUSTOMERS;
int barber_sleeping = 0;

int waiting_room[MAX_WAITING_CUSTOMERS];
int front = 0, rear = 0;

void *barber(void *arg) {
  while (1) {
    pthread_mutex_lock(&mutex);

    while (waiting_customers == 0) {
      if (customers_remaining == 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
      }

      barber_sleeping = 1;
      printf("Barber is going to sleep\n");
      pthread_cond_wait(&barber_cond, &mutex);
      barber_sleeping = 0;
    }

    int customer_id = waiting_room[front];
    front = (front + 1) % MAX_WAITING_CUSTOMERS;
    waiting_customers--;

    printf("Barber is cutting hair\n");

    pthread_cond_signal(&customer_cond);
    pthread_mutex_unlock(&mutex);

    sleep(2);
  }
  return NULL;
}

void *customer(void *arg) {
  int customer_id = *(int *)arg;
  free(arg);

  pthread_mutex_lock(&mutex);

  if (waiting_customers < MAX_WAITING_CUSTOMERS) {
    waiting_room[rear] = customer_id;
    rear = (rear + 1) % MAX_WAITING_CUSTOMERS;
    waiting_customers++;

    printf("Customer %d is waiting\n", customer_id);

    if (barber_sleeping) {
      pthread_cond_signal(&barber_cond);
    }

    pthread_cond_wait(&customer_cond, &mutex);

    printf("Customer %d is getting a haircut\n", customer_id);

    pthread_mutex_unlock(&mutex);

    sleep(2);

    pthread_mutex_lock(&mutex);
    customers_remaining--;
    pthread_cond_signal(&barber_cond);
    pthread_mutex_unlock(&mutex);

  } else {
    customers_remaining--;
    pthread_cond_signal(&barber_cond);
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

int main() {
  pthread_t barber_thread;
  pthread_t customer_threads[NUM_CUSTOMERS];

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&barber_cond, NULL);
  pthread_cond_init(&customer_cond, NULL);

  srand(time(NULL));

  pthread_create(&barber_thread, NULL, barber, NULL);

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    sleep(rand() % 3);
    int *customer_id = malloc(sizeof(int));
    *customer_id = i;
    pthread_create(&customer_threads[i], NULL, customer, customer_id);
  }

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    pthread_join(customer_threads[i], NULL);
  }

  pthread_join(barber_thread, NULL);

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&barber_cond);
  pthread_cond_destroy(&customer_cond);

  return 0;
}
