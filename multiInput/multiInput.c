#define  _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define NAME_SIZE 16

// Reverse linked list node of Boolean variable and Boolean value pairs
typedef struct var {
    char name[NAME_SIZE];
    bool val;
    struct var* prev;
} var_t;


// PROCESS A SINGLE ROW OF CIRCUIT FILE DESCRIBING A GATE TO FIND OUTPUT NAME AND VALUE
void findOutVarNameVal (
    var_t* varList,
    FILE* circuit_fp,
    char* outName,
    bool* outVal
) {

    char gate[NAME_SIZE];
    size_t input_size;

    fscanf(circuit_fp, "%s", gate);

    // printf("%s\n",gateLine);

    if ( strcmp(gate, "NOT") == 0) {
      // printf("%s \n", gate);
        // It is a NOT gate
        // printf("NOT %s %s\n", inName0, outName);
        char name[NAME_SIZE];
        fscanf(circuit_fp, "%s", name);
        bool inVal=false;
        // Traverse varList to look for existing value
        var_t* curr = varList;
        while (curr) {
            // https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
            if ( strcmp(curr->name, name)==0 ) {
                inVal = curr->val;
                break;
            }
            curr = curr->prev;
        }
        fscanf(circuit_fp, "%s", outName);
        *outVal = !inVal;
        // printf("%d ", *outVal);
        // printf("%s\t %d\n",outName, *outVal);

    }
    else {

      // printf("y \n");
    fscanf(circuit_fp, "%ld", &input_size);
    bool input_values[input_size];

    for ( size_t i=0; i<input_size; i++ ){

        char input_buff[NAME_SIZE];
        assert ( fscanf(circuit_fp, "%s", input_buff)==1 );

        bool inVal=false;
        var_t* curr = varList;
        while (curr) {
            // https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
              if ( strcmp(curr->name, input_buff)==0 ) {
                inVal = curr->val;
                break;
            }
            curr = curr->prev;
        }
        input_values[i] = inVal;
        // printf("%s", input_buff);
        // printf("%s\t %d\n",input_buff, inVal);
    }

     assert( fscanf(circuit_fp, "%s", outName) == 1 );

    if(strcmp(gate, "AND") == 0 || strcmp(gate, "NAND") == 0){

      bool x = input_values[0];
      for( size_t i = 1; i < input_size; i++){

          x = x && input_values[i];

      }
      (*outVal) = x;
      if(strcmp(gate, "NAND") == 0){
        (*outVal) = !(*outVal);
      }

    }
    else if(strcmp(gate, "OR") == 0 || strcmp(gate, "NOR") == 0){

      bool x = input_values[0];
      for( size_t i = 1; i < input_size; i++){

        x = x || input_values[i];


      }

      (*outVal) = x;
      // printf("....................%d\n", *outVal);

      if(strcmp(gate, "NOR") == 0){

        (*outVal) = !(*outVal);

      }

    }





    }
}


// PRINT A SINGLE ROW OF THE TRUTH TABLE GIVEN INITIAL ASSIGNMENTS TO CIRCUIT INPUTS
void printTruthTableRow (
    FILE* circuit_fp,
    size_t circuitInputBits // Vector of Boolean assignments for input bits
) {

    // Record input variable names
    size_t circuitInputCount;
    assert ( fscanf(circuit_fp, "INPUTVAR %ld ", &circuitInputCount)==1 );
    var_t* inputs = calloc( circuitInputCount, sizeof(var_t) ); // We use an array of var_t here
    for ( size_t i=0; i<circuitInputCount; i++ )
        assert ( fscanf(circuit_fp, "%s", inputs[i].name)==1 );
    assert ( fscanf(circuit_fp, "\n")==0 );

    // Record output variable names
    size_t circuitOutputCount;
    assert ( fscanf(circuit_fp, "OUTPUTVAR %ld ", &circuitOutputCount)==1 );
    var_t* outputs = calloc( circuitOutputCount, sizeof(var_t) ); // We use an array of var_t here
    for ( size_t i=0; i<circuitOutputCount; i++ )
        assert ( fscanf(circuit_fp, "%s", outputs[i].name)==1 );
    assert ( fscanf(circuit_fp, "\n")==0 );

    // Reversed linked list of variable structs
    var_t* varList = NULL;
    // Initialize the list with initial assignments to input variables
    for ( size_t i=0; i<circuitInputCount; i++ ) {
        var_t* temp = calloc( 1, sizeof(var_t) );
        // https://www.tutorialspoint.com/c_standard_library/c_function_strcpy.htm
        strcpy(temp->name, inputs[i].name);
        temp->val = 0b1 & circuitInputBits>>i; // Each bit in input bits
        temp->prev = varList;
        varList = temp;
    }

    while( !feof(circuit_fp) ){

      char outName[NAME_SIZE];
      bool outVal;
      findOutVarNameVal( varList, circuit_fp, outName, &outVal );
      assert ( fscanf(circuit_fp, "\n")==0 );

    // Record this gate's outputs
      var_t* temp = calloc( 1, sizeof(var_t) );
    // https://www.tutorialspoint.com/c_standard_library/c_function_strcpy.htm

    // printf("output:_%s_%d\n", outName, outVal);
      strcpy(temp->name, outName);
      temp->val = outVal;
      temp->prev = varList;
      varList = temp;
      // printf("z ");

  }

    /* ... */
// printf("%s %d ", outputs[0].name, outputs[0].val);

    // Print the truth table
    for ( size_t i=0; i<circuitInputCount; i++ ) {
        var_t* curr = varList;
        while (curr) {
            if ( strcmp(curr->name, inputs[i].name)==0 ) {
                printf ( "%d ", curr->val );
                break;
            }
            curr = curr->prev;
        }
    }
    for ( size_t i=0; i<circuitOutputCount; i++ ) {
        var_t* curr = varList;
        while (curr) {
            if ( strcmp(curr->name, outputs[i].name)==0 ) {
                printf ( "%d ", curr->val );
                break;
            }
            curr = curr->prev;
        }
    }
    printf("\n");


    while (varList) {
        // printf ( "\n%s %d", varList->name, varList->val );
        var_t* temp = varList;
        varList = varList->prev;
        free(temp);
    }
    free(outputs);
    free(inputs);
}


int main(int argc, char* argv[]) {

    if (argc!=2) {
        printf("Usage: ./basicGates <circuit_file>\n");
        exit(EXIT_FAILURE);
    }

    // Open circuit file
    FILE* circuit_fp = fopen(argv[1], "r");
    if (!circuit_fp) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // Peak at the input count so we know how many rows in the truth table
    size_t circuitInputCount;
    assert ( fscanf(circuit_fp, "INPUTVAR %ld ", &circuitInputCount)==1 );

    // CURRENTLY SET UP TO PRINT JUST TWO ROWS OF THE TRUTH TABLE

    // Rewind circuit file for each truth table line
    for(size_t i = 0; i < (int) pow(2,circuitInputCount); i++){
      rewind(circuit_fp);
      printTruthTableRow (circuit_fp, 0b0 | i);
    }

    fclose(circuit_fp);
    exit(EXIT_SUCCESS);
}
