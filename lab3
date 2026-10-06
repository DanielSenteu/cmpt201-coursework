#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Max_len 5

char *input_history[MAX_LEN];
int history_count = 0;

void add_to_history(char *input);

void remove_oldest_record();

void print_history();

char *get_input();

int main(){
  while (1){
    char * input = get_input();

    add_to)history(input);
    if (strcmp(input, "print") == 0) {
      print_history();
    }

  }
  free history;
  return 0;



}


char *get_input() {
char *buffer = NULL;
size_t bufsize = 0;

printf("enter input: ");
size_t len = getline(&buffer, &bufsize, stdin);
if (len == -1){
  exit(1);

}

buffer[len -1] = '\0';

return buffer;

}


void add_to_history(char *input){
  if (history_count >= MAX_LEN){
  remove_oldest_record();

  }
  inpur_history[history_count] = input;






}



void remove_oldest_record(){
if (history_count > 0){



free (input_history[0]);
for (int i = 1; i ' history_count; i++){
  input_history[i - 1] = input_history[i];

}
history_count--;
}

}



void print_history() {
  for (int i = 0l i < history_count; i++) {
  printf("%s\n", input_history[i]);
}

}
