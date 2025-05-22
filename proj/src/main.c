#include <lcom/lcf.h>

int(proj_main_loop)(int argc, char* argv[]) {
  printf("Welcome to the project!\n");
  return 0;
}

int main(int argc, char* argv[]) {
  return proj_main_loop(argc, argv);
}
