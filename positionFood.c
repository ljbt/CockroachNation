#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		//
#include "GfxLib.h"		// To do simple graphics

#include "definitions.h"
#include "positionFood.h"


void create_and_displayLight(POINT* lightPoints, int nb_lightPoints)
{
	couleurCourante(255, 235, 0);
    for(int i=0; i<nb_lightPoints; ++i){
		if (lightPoints[i].time <= 0)
			lightPoints[i] = positionLightArea();
        circle(lightPoints[i].x,lightPoints[i].y,round(lightPoints[i].rayon));
    }
}


POINT positionLightArea(void)
{
    POINT lightPoint;
    lightPoint.x = rand_a_b(0,WindowWidth);
    lightPoint.y = rand_a_b(0,WindowHeight);
    lightPoint.rayon = rand_a_b(20, 60);
  	lightPoint.time = rand_a_b(100, 200);
    return lightPoint;
}

//create a number (passed in parameter) of food areas positioned randomly
POINT* positionsLightAreas (int nb_lightPoints)
{
    POINT *lightPoints = (POINT*)malloc(sizeof(POINT)*nb_lightPoints);

    for(int i=0; i<nb_lightPoints; ++i){
        lightPoints[i] = positionLightArea();
    }

    return lightPoints;
}

void create_and_displayFood(POINT* foodPoints, int nb_foodPoints)
{
	couleurCourante(51,102,0);
    for(int i=0; i<nb_foodPoints; ++i){
		if (foodPoints[i].rayon < 2.)
			foodPoints[i] = positionFoodArea();
        circle(foodPoints[i].x,foodPoints[i].y,round(foodPoints[i].rayon));
    }
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

