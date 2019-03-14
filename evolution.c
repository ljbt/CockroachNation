#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "definitions.h"
#include "evolution.h"


// function that kills a cockroach, so the array of swarm is reduced by one
Cockroach* adios(Cockroach *swarm, int *swarmSize, int idx_death)
{
	int i;
    for(i = idx_death; i < *swarmSize-1; i++) // swarsize-1 because we do not want to update the last element of the array
    {
        swarm[i] = swarm[i+1];
    } 
    (*swarmSize)--;
    //realloc is done to avoid unused cockroach in memory and to avoid error when using realloc elsewhere (for instance in reproduction)
    Cockroach *new_swarm;
	new_swarm = (Cockroach *)realloc(swarm, sizeof(*new_swarm) * (*swarmSize));
	if (new_swarm == NULL) {
		perror("realloc");
		exit(1);
	}
	return new_swarm;
}

void cockroach_evolution(Cockroach *insect,int num_insect, int day)
{
    if(insect->adult_date == day && insect->growth == Larva) // larva to adult
    {   
        insect->growth = Adult;
        printf("%d is now a %s adult, let's fuck !\n", insect->id, getGenderName(insect->gender));
        insect->time_for_reproduction = true;
    }
}
