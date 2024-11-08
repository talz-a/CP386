#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define MAX 5

sem_t count;
pthread_mutex_t mutex;

int buffer[MAX];
int fill = 0;
int use = 0;
int start_consuming = 0;

void *producer(void *arg) {
  for (int i = 0; i < 10; i++) {
    pthread_mutex_lock(&mutex);

    while ((fill + 1) % MAX == use) {
      pthread_mutex_unlock(&mutex);
      pthread_mutex_lock(&mutex);
    }

    buffer[fill] = i;
    fill = (fill + 1) % MAX;

    if (i == 2) {
      start_consuming = 1;
    }

    printf("Produced %d\n", i);
    sem_post(&count);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void *consumer(void *arg) {
  pthread_mutex_lock(&mutex);

  while (!start_consuming) {
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&mutex);
  }
  pthread_mutex_unlock(&mutex);

  for (int i = 0; i < 10; i++) {
    sem_wait(&count);
    pthread_mutex_lock(&mutex);

    int val = buffer[use];
    use = (use + 1) % MAX;

    printf("Consumed %d\n", val);
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main() {
  pthread_t prod_thread, cons_thread;

  sem_init(&count, 0, 0);
  pthread_mutex_init(&mutex, NULL);

  pthread_create(&prod_thread, NULL, producer, NULL);
  pthread_create(&cons_thread, NULL, consumer, NULL);

  pthread_join(prod_thread, NULL);
  pthread_join(cons_thread, NULL);

  sem_destroy(&count);
  pthread_mutex_destroy(&mutex);

  return 0;
}
