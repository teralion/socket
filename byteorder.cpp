#include <cstdlib>
#include <cstdio>

int main(int argc, char **argv) {
  union {
    short s;
    char c[sizeof(short)];
  } order;

  order.s = 0x0102;
  if (sizeof(short) == 2) {
    if (order.c[0] == 1 && order.c[1] == 2) {
      printf("big-endian\n");
    } else if (order.c[0] == 2 && order.c[1] == 1) {
      printf("little-endian\n");
    } else {
      printf("unknown\n");
    }
  } else {
    printf("sizeof(short) = %ld\n", sizeof(short));
  }
  exit(0);
}
