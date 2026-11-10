#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUTS 100
#define MAX_LINE 100

typedef struct {
  int line_number;
  int value;
} Input;

typedef struct {
  int line_number;
  int doubled_value;
} IntermediateInput;

typedef struct {
  int doubled_value;
  int line_numbers[MAX_INPUTS];
  int count;
} Output;

void map(Input *input, IntermediateInput *intermediate_input) {
  intermediate_input->line_number = input->line_number;
  intermediate_input->doubled_value = input->value * 2;
}

void groupByKey(IntermediateInput *input, Output *output, int *result_count) {
  int found = 0;

  for (int i = 0; i < *result_count; i++) {
    if (output[i].doubled_value == input->doubled_value) {
      // already have this value, add the line number to it
      if (output[i].count < MAX_INPUTS) {
        output[i].line_numbers[output[i].count] = input->line_number;
        output[i].count++;
      }
      found = 1;
      break;
    }
  }

  if (found == 0) {
    // new value, make a new entry at the end
    if (*result_count < MAX_INPUTS) {
      output[*result_count].doubled_value = input->doubled_value;
      output[*result_count].line_numbers[0] = input->line_number;
      output[*result_count].count = 1;
      *result_count = *result_count + 1;
    }
  }
}

void reduce(Output output) {
  printf("(%d, [", output.doubled_value);
  for (int i = 0; i < output.count; i++) {
    printf("%d", output.line_numbers[i]);
    if (i < output.count - 1) {
      printf(", ");
    }
  }
  printf("])\n");
}

int main(void) {
  Input inputs[MAX_INPUTS];
  IntermediateInput intermediate[MAX_INPUTS];
  Output outputs[MAX_INPUTS];
  int input_count = 0;
  int result_count = 0;
  char line[MAX_LINE];

  printf("Enter values (one per line). Type 'end' to finish:\n");
  while (input_count < MAX_INPUTS && fgets(line, sizeof(line), stdin) != NULL) {
    line[strcspn(line, "\n")] = '\0';
    if (strcmp(line, "end") == 0) {
      break;
    }
    inputs[input_count].line_number = input_count + 1;
    inputs[input_count].value = atoi(line);
    input_count++;
  }

  for (int i = 0; i < input_count; i++) {
    map(&inputs[i], &intermediate[i]);
  }

  for (int i = 0; i < input_count; i++) {
    groupByKey(&intermediate[i], outputs, &result_count);
  }

  for (int i = 0; i < result_count; i++) {
    reduce(outputs[i]);
  }

  return 0;
}
