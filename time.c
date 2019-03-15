#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>
#include "GfxLib.h"		// To do simple graphics

#include "definitions.h"
#include "time.h"

#define MAX_ITERATION 1e3   // for day iteration

void computes_day(int *day, int *num_iteration)
{
    if (*num_iteration > MAX_ITERATION ) {
        (*day)++;
        printf("\nDAY %d\n",*day);
        (*num_iteration) = 0;
    }
}

void display_day(const int day)
{
    char chaine [10];
    sprintf(chaine,"Day %d",day);
    couleurCourante(200,100,10);
    afficheChaine(chaine , 20 , WindowWidth/2 - 30 , 10);
}