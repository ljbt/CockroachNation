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

POINT positionFoodArea(void)
{
    POINT foodPoint;
    foodPoint.x = rand_a_b(0,WindowWidth);
    foodPoint.y = rand_a_b(0,WindowHeight);
    foodPoint.rayon = rand_a_b(10, 50);
    return foodPoint;
}

//create a number (passed in parameter) of food areas positioned randomly
POINT* positionsFoodAreas (int nb_foodPoints)
{
    POINT *foodPoints = (POINT*)malloc(sizeof(POINT)*nb_foodPoints);

    for(int i=0; i<nb_foodPoints; ++i){
        foodPoints[i] = positionFoodArea();
    }

    return foodPoints;
}

//computes the distances between cockroach position and food areas, and stores the smallest one and its index in pointers
void indexCloseFood(Cockroach insect, int nb_foodPoints, POINT *foodPoints,int *indexCloseFood,double *little_hypotenuse)
{
    double deltaX;
	double deltaY;
	double hypotenuse;

    for(int i = 0; i < nb_foodPoints; i++)
    {
        deltaX = insect.x-foodPoints[i].x;
        deltaY = insect.y-foodPoints[i].y;
	    hypotenuse = hypot(deltaX, deltaY);

        if (i == 0 || hypotenuse < *little_hypotenuse) {
            *little_hypotenuse = hypotenuse;
            *indexCloseFood = i;
        }
    }

}

