#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		//

#include "definitions.h"
#include "positionFood.h"

//valeur entiere aleatoire dans [a;b[
int rand_a_b(int a, int b){

    return rand()%(b-a)+a ;
}

void affichePoint(POINT p)
{
	printf("x = %d , y = %d\n", p.x, p.y);
}

//create 
POINT* positionsFoodAreas (int nb_foodPoints)
{
    POINT *foodPoints = (POINT*)malloc(sizeof(POINT)*nb_foodPoints);

    for(int i=0; i<nb_foodPoints; ++i){
        foodPoints[i].x = rand_a_b(0,WindowWidth);
        foodPoints[i].y = rand_a_b(0,WindowHeight);
    }

    return foodPoints;
}

