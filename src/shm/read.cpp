/*
* read from the shm every 1 second
*/
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>

int main() {

  int shm_id;
  int *share;

  shm_id = shmget(1234, getpagesize(), IPC_CREAT | 0660);

  if (shm_id == -1) {
    perror("shmget()");
  }

  share = (int *)shmat(shm_id, 0, 0);

  while (1) {
    sleep(1);
    printf("%d\n", *share);
  }

  return 0;
}