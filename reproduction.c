#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "ESLib.h"		// For valeurAleatoire()

#include "definitions.h"
#include "evolution.h"


void time_for_reproduction_or_not(Cockroach *insect, const int nb_min_days_without_sex, const int day)
{
    if( insect->growth == Adult  && day - insect->last_reproduction_day >= nb_min_days_without_sex)
    {
        insect->time_for_reproduction = true;
    }
}

//computes the distances between cockroach position and food areas, and stores the smallest one and its index in pointers
void indexClosePartner(Cockroach *swarm,int num_insect, int swarmSize ,int *indexClosePartner,double *little_hypotenuse)
{
    double deltaX;
	double deltaY;
	double hypotenuse;
    bool oppos_gender;

    for(int i = 0; i < swarmSize; i++)
    { 
        // conditions for beeing a good partner: not himself, opposite gender, in Reproducting mode
        if(i != num_insect)
        {
            oppos_gender = opposite_gender(swarm[i].gender,swarm[num_insect].gender);        
            if (oppos_gender && swarm[i].time_for_reproduction)
            {
                deltaX = swarm[i].x-swarm[num_insect].x;
                deltaY = swarm[i].y-swarm[num_insect].y;
                hypotenuse = hypot(deltaX, deltaY);
               
                if (*little_hypotenuse == -1 || hypotenuse < *little_hypotenuse) {
                    *little_hypotenuse = hypotenuse;
                    *indexClosePartner = i;
                }
            }
        }
    }
}

int find_new_id(Cockroach *swarm, int swarmSize)
{
    int new_id = 0;
    bool new = false;
    while(!new)
    {
        for(int i = 0; i < swarmSize; i++)
        {
            if (new_id != swarm[i].id)  // see if new id is different from each id
            {
                new = true;
                if(i == swarmSize-1) // new id has been diferent from each id in the array
                    return new_id;
            }
            else
            {
                new = false;
                break;
            }
        }
        new_id++;
    }
    /*  if no return value yet, it means that for example:
        the cockroach array has 3 values and they are (surely in disorder) 0, 1 and 2 without a doubt
        so the new id is 3 */
    return swarmSize;
}

void init_larva (Cockroach *swarm, const int idx, const int idx_parent_1, const int idx_parent_2, const int birthday, const int new_id)
{
    const double theta = valeurAleatoire()*2.*M_PI;
	const double rho = sqrt(valeurAleatoire()*pow(fmin(WindowWidth/2., WindowHeight/2.), 2));
	const double x = WindowWidth*.5+cos(theta)*rho;
	const double y = WindowHeight*.5+sin(theta)*rho;

    swarm[idx] = (Cockroach)
    {
        .id = new_id,
		.x = x,
		.y = y,
		.speedRho = 1.,
		.speedTheta = valeurAleatoire()*2.*M_PI,	// GroupSpeedTheta
		.gender = alea_gender(), // male or female
		.growth = Larva, // larva by default
		.adult_date = birthday + rand_a_b(1,6), // from birthday, date to become an adult
		.time_for_reproduction = false,
		.last_reproduction_day = birthday, // no reproduction yet
		.mode = Walking,
	};
}


void createLarva(Cockroach **swarm, int idx_parent_1, int idx_parent_2, int *swarmSize, const int birthday)
{
    (*swarmSize)++;
    (*swarm) = (Cockroach *)realloc((*swarm), (*swarmSize) * sizeof(Cockroach));// realloc with swarmSize+1
    // now init new larva values
    int new_id = find_new_id(*swarm, *swarmSize);
    int idx_larva = (*swarmSize)-1;
    init_larva((*swarm),idx_larva,idx_parent_1,idx_parent_2,birthday,new_id);
    genetic_evolution(*swarm,idx_larva,idx_parent_1,idx_parent_2);
}


void reproduction (Cockroach **swarm, int idx_parent_1, int idx_parent_2, int *swarmSize, const int day)
{
	printf("%d and %d have babies !\n", (*swarm)[idx_parent_1].id, (*swarm)[idx_parent_2].id);

    (*swarm)[idx_parent_1].last_reproduction_day = (*swarm)[idx_parent_2].last_reproduction_day = day;
    (*swarm)[idx_parent_1].time_for_reproduction = (*swarm)[idx_parent_2].time_for_reproduction = false;
    (*swarm)[idx_parent_1].mode = (*swarm)[idx_parent_2].mode = Walking;

    int nb_larva = rand_a_b(2,4);
    printf("Birth of %d larva\n", nb_larva);

    printf("parent 1: life = %f, food attraction = %f, light sensitive = %f\n", 
        (*swarm)[idx_parent_1].life, (*swarm)[idx_parent_1].food_attraction,(*swarm)[idx_parent_1].light_sensitivity);
    printf("parent 2: life = %f, food attraction = %f, light sensitive = %f\n", 
        (*swarm)[idx_parent_2].life, (*swarm)[idx_parent_2].food_attraction,(*swarm)[idx_parent_2].light_sensitivity);

    for(int i = 0; i < nb_larva; i++)
    {
        createLarva(swarm,idx_parent_1,idx_parent_2,swarmSize, day);
        printf("larva: life = %f, food attraction = %f, light sensitive = %f\n", 
            (*swarm)[*swarmSize -1].life, (*swarm)[*swarmSize -1].food_attraction,(*swarm)[*swarmSize -1].light_sensitivity);
    }
 }
