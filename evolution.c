#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "ESLib.h"		// For valeurAleatoire()

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
        printf("%d is now a %s adult, find the one !\n", insect->id, getGenderName(insect->gender));
        insect->time_for_reproduction = true;
    }
}

 int crossover(int gene1, int gene2)
{
	const int mask = valeurAleatoire()*65536;
	return (gene1 & mask) | (gene2 & ~mask);
}

int mutation(int gene, int maxValue)
{
	const int bitNumber = valeurAleatoire()*32;
	return ((gene ^ (1<<bitNumber))&1023)%maxValue;
}

void genetic_evolution (Cockroach *swarm, const int idx, const int idx_parent_1, const int idx_parent_2)
{
    // crossover between each parent gene
    swarm[idx].life = crossover(swarm[idx_parent_1].life,swarm[idx_parent_2].life);
    swarm[idx].light_sensitivity = crossover(swarm[idx_parent_1].light_sensitivity,swarm[idx_parent_2].light_sensitivity);
    swarm[idx].food_attraction = crossover(swarm[idx_parent_1].food_attraction,swarm[idx_parent_2].food_attraction);

    // mutation of each gene
    int lifeMaxValue = swarm[idx].life + 150 ;
    swarm[idx].life = mutation(swarm[idx].life,lifeMaxValue);
    int lightMaxValue = swarm[idx].light_sensitivity + 150;
    swarm[idx].light_sensitivity = mutation(swarm[idx].light_sensitivity,lightMaxValue);
    int foodMaxValue = swarm[idx].food_attraction + 150;
    swarm[idx].food_attraction = mutation(swarm[idx].food_attraction,foodMaxValue);
}
