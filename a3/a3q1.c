#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX 5 // Buffer size

sem_t count; // Semaphore to keep track of the number of items in the buffer
pthread_mutex_t mutex; // Mutex lock for buffer access

int buffer[MAX];
int fill = 0; // Index for producer to put data
int use = 0;  // Index for consumer to get data
int start_consuming =
    0; // Flag indicating when the consumer should start (0 = false, 1 = true)

void *producer(void *arg) {
  for (int i = 0; i < 10; i++) {
    pthread_mutex_lock(&mutex);

    // Wait if buffer is full
    while ((fill + 1) % MAX == use) {
      pthread_mutex_unlock(&mutex);
      // Busy wait
      pthread_mutex_lock(&mutex);
    }

    // Put item into buffer
    buffer[fill] = i;
    fill = (fill + 1) % MAX;

    // Set start_consuming to true after producing 3 items
    if (i == 2) { // After producing the third item (indices 0,1,2)
      start_consuming = 1;
    }

    printf("Produced %d\n", i);

    sem_post(&count); // Signal that a new item is available

    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *consumer(void *arg) {
  // Wait until at least 3 items have been produced
  pthread_mutex_lock(&mutex);
  while (!start_consuming) {
    pthread_mutex_unlock(&mutex);
    // Busy wait
    pthread_mutex_lock(&mutex);
  }
  pthread_mutex_unlock(&mutex);

  for (int i = 0; i < 10; i++) {
    sem_wait(&count); // Wait for an item to be available

    pthread_mutex_lock(&mutex);

    // Get item from buffer
    int val = buffer[use];
    use = (use + 1) % MAX;

    printf("Consumed %d\n", val);

    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main() {
  pthread_t prod_thread, cons_thread;

  // Initialize semaphore and mutex
  sem_init(&count, 0, 0); // Buffer initially empty
  pthread_mutex_init(&mutex, NULL);

  // Create producer and consumer threads
  pthread_create(&prod_thread, NULL, producer, NULL);
  pthread_create(&cons_thread, NULL, consumer, NULL);

  // Wait for threads to finish
  pthread_join(prod_thread, NULL);
  pthread_join(cons_thread, NULL);

  // Destroy semaphore and mutex
  sem_destroy(&count);
  pthread_mutex_destroy(&mutex);

  return 0;
}

