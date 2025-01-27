#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  
  volatile int i, j, prime;
  
  for (i = 2; i < 5000000; i++) {
    prime = 1;
    for (j = 2; j * j <= i; j++) {
      if (i % j == 0) {
        prime = 0;
      }
      if (prime == 0){
        break;
      }
    }
  }
  exit(0);
}

