#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, long x, long y) {
        int temp = a[x];
        a[x] = a[y];
        a[y] = temp;
}

void quicksort_parallel(int *a, long lo, long hi) {
        long i, div;

        if (lo < hi) {
                int x = a[lo];
                div = lo;

                for (i = lo+1; i < hi; i++) {
                        if (a[i] <= x) {
                                div++;
                                swap(a, div, i);
                        }
                }

                swap(a, lo, div);
                quicksort_parallel(a, lo, div);
                quicksort_parallel(a, div+1, hi);
        }
}

int main(int argc, char *argv[]) {
	int i;
	double start, end;
	int nArray = atoi(argv[1]);
	int rank, numtasks;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int elmt_per_proses = nArray / numtasks;

	int *data = (int*) malloc(nArray * sizeof(int));

	if (rank == 0){
		if (data != NULL) {
	        for (i = 0; i < nArray; i++) {
	        	data[i] = rand();
	        }
		}

		start = MPI_Wtime();
	}


	if (numtasks > 1) {
		int *part_data = (int*) malloc(elmt_per_proses * sizeof(int));

	  	if (part_data != NULL) {
	  		//Distributed data to every part of data
			MPI_Scatter(data, elmt_per_proses, MPI_INT, part_data, elmt_per_proses, MPI_INT, 0, MPI_COMM_WORLD);
				
			quicksort_parallel(part_data, 0, elmt_per_proses);
			
			//Gather the data form part data to the real data
		    MPI_Gather(part_data, elmt_per_proses, MPI_INT, data, elmt_per_proses, MPI_INT, 0, MPI_COMM_WORLD);
				
		    free(part_data);
		}
	}

	if (rank == 0){
		quicksort_parallel(data, 0, nArray);
		
	    end = MPI_Wtime();
		
		free(data);

	    printf ("Time : %f second\n", end - start);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();

	return 0;
}
