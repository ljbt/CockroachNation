#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "definitions.h"


// function that kills a cockroach, so the array of swarm is reduced by one
void adios(Cockroach *swarm, int *swarmSize, int idx_death)
{
    for(int i = idx_death; i < *swarmSize; i++)
    {
        swarm[i] = swarm[i+1];
    }
    (*swarmSize)--;
}

