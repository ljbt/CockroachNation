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

void reproduction (Cockroach *swarm, int idx_parent_1, int idx_parent_2, int *swarmSize, const int day)
{
    swarm[idx_parent_1].last_reproduction_day = swarm[idx_parent_2].last_reproduction_day = day;
    swarm[idx_parent_1].time_for_reproduction = swarm[idx_parent_2].time_for_reproduction = false;
    swarm[idx_parent_1].mode = swarm[idx_parent_2].mode = Walking;
}