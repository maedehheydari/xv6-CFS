#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  int a, b;
  a = 0;
  b = 1;
  for(i = 0; i < 10; i++) {
    for (int j = 0; j < 50000000; j++) {
      a+=b;
    }
    sleep(10);
  }
  exit(0);
}

