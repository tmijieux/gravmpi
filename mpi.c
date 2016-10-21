#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define BUFSIZE 2048

static void handle_mpi_error(int code)
{
  if (code != MPI_SUCCESS) {
    int len;
    char error_string[BUFSIZE];
    MPI_Error_string(code, error_string, &len);
    fprintf(stderr, "%3d: %s\n", my_rank, error_string);
  }
  exit(EXIT_FAILURE);
}

void data_struct(void)
{
  int count=5;
  int blen[count];
  int array_of_blocklengths[count];
  MPI_Aint offsets[count];
  MPI_Datatype newtype;
  for(int i = 0; i<count;++i){
    blen[i] = 1; 
    oldtypes[0] = MPI_DOU;
  }
  offsets[0] = G_STRUCT_OFFSET(struct star, mass); 
  offsets[1] = G_STRUCT_OFFSET(struct star, x); 
  offsets[2] = G_STRUCT_OFFSET(struct star, y); 
  offsets[3] = G_STRUCT_OFFSET(struct star, vx); 
  offsets[4] = G_STRUCT_OFFSET(struct star, vy); 


  int err;
  err = MPI_Type_create_struct(1, blen, offsets, oldtypes, &newtype);
  handle_mpi_error(err);
  err = MPI_Type_commit(&newtype);
  handle_mpi_error(err);
}
 

