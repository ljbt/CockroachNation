#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "definitions.h"
#include "evolution.h"


// function that kills a cockroach, so the array of swarm is reduced by one
void adios(Cockroach *swarm, int *swarmSize, int idx_death)
{
    for(int i = idx_death; i < *swarmSize; i++)
    {
        swarm[i] = swarm[i+1];
    }
    (*swarmSize)--;
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