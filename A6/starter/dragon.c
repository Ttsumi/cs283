#include <stdio.h>
#include <stdlib.h>

size_t get_file_size(){
  FILE *file = fopen("dragon.txt", "r");
  if (!file){
      return (size_t) -1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fclose(file);
  return size;
}

// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){
  // TODO implement
  char *dragon_buff;
  size_t dragon_size = get_file_size();
  if (dragon_size == (size_t) -1){
      perror("Print Dragon");
  }
  dragon_buff = malloc(dragon_size);
  FILE *file = fopen("dragon.txt", "r");
  size_t len = fread(dragon_buff, 1, dragon_size - 1, file);
  fclose(file);
  if (len == 0){
      perror("Read Dragon");
  }
  printf("%s\n", dragon_buff);
  free(dragon_buff);
}
