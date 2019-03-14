#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>

#include "GfxLib.h"		// To do simple graphics
#include "ESLib.h"		// For valeurAleatoire()

#include "definitions.h"


void time_for_reproduction_or_not(Cockroach *insect, const int nb_min_days_without_sex, const int day)
{
    if( insect->growth == Adult  && day - insect->last_reproduction_day >= nb_min_days_without_sex )
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

/*     Gender opposite_gender;
    if(swarm[num_insect].gender == Male) opposite_gender = Female;
    else opposite_gender == Male; */

    bool oppos_gender;

    for(int i = 0; i < swarmSize; i++)
    { 
        // conditions for beeing a good partner: not himself, opposite gender, in Reproducting mode
        if(i != num_insect)
        {
            oppos_gender = opposite_gender(swarm[i].gender,swarm[num_insect].gender);
            if (oppos_gender)
            {
//                printf("diff gender between %s %d and %s %d\n", getGenderName(swarm[num_insect].gender),swarm[num_insect].id, getGenderName(swarm[i].gender), swarm[i].id);
//                printf("time for reproduction for %d?: %d\n", swarm[i].id, swarm[i].time_for_reproduction);
            }           
            if (oppos_gender && swarm[i].time_for_reproduction)
            {
                deltaX = swarm[i].x-swarm[num_insect].x;
                deltaY = swarm[i].y-swarm[num_insect].y;
                hypotenuse = hypot(deltaX, deltaY);
               
                if (*little_hypotenuse == -1 || hypotenuse < *little_hypotenuse) {
                    *little_hypotenuse = hypotenuse;
                    *indexClosePartner = i;
                }
//                printf("dist = %f and little hypot = %f\n", hypotenuse, *little_hypotenuse);
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

Cockroach* createLarva(Cockroach *swarm, int idx_parent_1, int idx_parent_2, int *swarmSize)
{
    (*swarmSize)++;
    printf("new size = %d\n", *swarmSize);
    Cockroach *new_swarm;
    new_swarm = (Cockroach *)realloc(swarm, sizeof(Cockroach) * (*swarmSize));

    // now init new larva values
	new_swarm[(*swarmSize)-1] = (Cockroach){
			.id = find_new_id(new_swarm,(*swarmSize)-1),
			.x = new_swarm[idx_parent_1].x,
			.y = new_swarm[idx_parent_1].y,
			.speedRho = 1.,
			.speedTheta = valeurAleatoire()*2.*M_PI,	// GroupSpeedTheta
			.gender = alea_gender(), // male or female
			.growth = Larva, 
			.adult_date = rand_a_b(1,6), //time from larva to adult (between 2 and 6 days here)
			.time_for_reproduction = false,
			.last_reproduction_day = 0, // no reproduction yet
			.mode = Walking,
			.food_attraction = 0, 
			.light_sensitivity = 100,
			.capacity_to_survive = 100 // max de la jauge
		};

    printf("new id = %d\n", new_swarm[(*swarmSize)-1].id);
    return new_swarm;
}


Cockroach * reproduction (Cockroach *swarm, int idx_parent_1, int idx_parent_2, int *swarmSize, const int day)
{
	printf("%d and %d have babies !\n", swarm[idx_parent_1].id, swarm[idx_parent_2].id);
    swarm[idx_parent_1].last_reproduction_day = swarm[idx_parent_2].last_reproduction_day = day;
    swarm[idx_parent_1].time_for_reproduction = swarm[idx_parent_2].time_for_reproduction = false;
    swarm[idx_parent_1].mode = swarm[idx_parent_2].mode = Walking;

    int nb_larva = rand_a_b(10,20);

    Cockroach *new_swarm;
    new_swarm = createLarva(swarm,idx_parent_1,idx_parent_2,swarmSize);

//    displaySwarmDetails(swarm,*swarmSize);
    return new_swarm;
 }