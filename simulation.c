#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include <time.h>
#include "GfxLib.h"		// To do simple graphics
#include "ESLib.h"		// For valeurAleatoire()

#include "definitions.h"
#include "positionFood.h"
#include "evolution.h"

#ifndef M_PI
#define M_PI 3.141592654
#endif

int NumberOfCockroachs = 30;
const double PowerForWeights = 3;	// To decrease or increase the influence of the distance
const double Horizon = 100.;		// Units to look for neighbors
const double WeightOfNeighbors = .1;
const double Bubble = 5.;			// A minimal distance between cockroachs
const double WeightOfEscape = .8;
const double WeightOfMimic = .1;
const double MimicHorizon = 30.;		// Units to look for neighbors to mimic
const double lightBubble = 50.;		// A minimal distance with the light
const double foodBubble = 50.;		// food area distance
const int nb_max_foodPoints = 4;
const double WeightOfFoodApproach = .3;
const double WeightOflightEscape = .8;
const double MinDistanceFromBoxEdges = 1.;// A minimal distance with the edges of the box

const double Speed_of_digestion = .1; //when the cockroach walks he digests
const double Speed_of_exctinction = .1; // when the cockroach has to survive he looses life

const double EatValue = 5.0;
const double SpeedOfDeath = 0.1;
const double ValorisationLight = 50.0;
const double MaxLife = 100.0;
const double SeuilSurvie = 10.0;
const double ProbaPredateurEating = 2.0; // Chance sur 1000
const double ProbaPredateurWalking = 1.0; // Chance sur 1000

//enum {RandomWalker, SimpleCockroach} mode = SimpleCockroach /*RandomWalker*/;

//Eat if < 50, avoid light if > 50. 
double valorisation(Cockroach cockroach)
{
  	if (cockroach.capacity_to_survive < SeuilSurvie)
      	return 0.0;
	if(cockroach.capacity_to_eat > cockroach.capacity_to_avoid_light)
		return 0.0;
	else 
		return 100.0;
}

// To draw a circle
void circle(float xCenter, float yCenter, float radius)
{
	const int Steps = 16; // Number of sectors to draw a circle
	const double AngularSteps = 2.*M_PI/Steps;
	int index;

	for (index = 0; index < Steps; ++index) // For each sector
	{
		const double angle = 2.*M_PI*index/Steps; // Compute the starting angle
		triangle(xCenter, yCenter,
			xCenter+radius*cos(angle), yCenter+radius*sin(angle),
					xCenter+radius*cos(angle+AngularSteps), yCenter+radius*sin(angle+AngularSteps));
	}
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


Cockroach *initializeSwarm(int swarmSize) 
{
	Cockroach *swarm = (Cockroach*)calloc(swarmSize, sizeof(Cockroach));
	//const double GroupSpeedTheta = 0.;//valeurAleatoire()*2.*M_PI;
	double groupBaseEat = rand_a_b(10, 90); // valeur entre 10 et 90
	double groupBaseLight = rand_a_b(10, 90); // valeur entre 10 et 90
	for (int i = 0; i < swarmSize; ++i) {
	const double theta = valeurAleatoire()*2.*M_PI;
	const double rho = sqrt(valeurAleatoire()*pow(fmin(WindowWidth/2., WindowHeight/2.), 2));
	const double x = WindowWidth*.5+cos(theta)*rho;
	const double y = WindowHeight*.5+sin(theta)*rho;
	swarm[i] = (Cockroach){
			.x = x,
			.y = y,
			.speedRho = 1.,
			.speedTheta = valeurAleatoire()*2.*M_PI,	// GroupSpeedTheta
			swarm[i].mode = Walking,
			.capacity_to_eat = groupBaseEat + (rand_a_b(0, 20) - 10), // entre 0 et 100 car de base entre 0 et 90 et on ajoute ou enleve 10
			.capacity_to_avoid_light = groupBaseLight + (rand_a_b(0, 20) - 10),
			.capacity_to_survive = MaxLife // max de la jauge
		};
	}
	return swarm;
}

void displaySwarm(const Cockroach *swarm, int swarmSize) 
{
	epaisseurDeTrait(2);
	couleurCourante(0, 0, 0);
	for (int i = 0; i < swarmSize; ++i)
		point(swarm[i].x, swarm[i].y);//, 1);
}

void updateSwarm(Cockroach *swarm, int *swarmSize, int lightAbscissa, int lightOrdinate, int nb_foodPoints, POINT *foodPoints) 
{
	for (int i = 0; i < *swarmSize; ++i) {	// All the individuals
		switch (swarm[i].mode) 
		{
			case Eating:
					swarm[i].speedTheta = valeurAleatoire()*2.*M_PI;
				{
					// Rule 7: eat from food area
					int i_close_food;
					double hypothenuse_close_food;
					indexCloseFood(swarm[i], nb_foodPoints, foodPoints, &i_close_food, &hypothenuse_close_food);
					if (hypothenuse_close_food < foodPoints[i_close_food].rayon) 
					{
						foodPoints[i_close_food].rayon -= 0.1;
						swarm[i].capacity_to_survive += EatValue;
						if (swarm[i].capacity_to_survive > MaxLife)
						{
						swarm[i].capacity_to_survive = MaxLife;
						}
					}
					else swarm[i].mode = Walking;
				}
				{
					// Rule 6: walk when detect light if valorisation > ValorisationLight
					if (lightAbscissa >= 0 && lightOrdinate >= 0) {
						const double deltaX = lightAbscissa-swarm[i].x;
						const double deltaY = lightOrdinate-swarm[i].y;
						const double hypotenuse = hypot(deltaX, deltaY);
						if (hypotenuse < lightBubble)
						{
							if (valorisation(swarm[i]) > ValorisationLight)
							{
								swarm[i].mode = Walking;
							}
							else
							{
								if (rand_a_b(0, 1000) <= ProbaPredateurEating)
								{
									printf("hey number %d died because of PREDATOR while eating\n", i);
									adios(swarm, swarmSize, i); //death of little cockroach
									if(*swarmSize == 0)
									{
										printf("Everybody died... :(\n");
										exit(0);
									}
								}
							}
						}
					}
				}
				break;
			case Walking:
				{
					/*Rule of beeing alive: if he walks he looses food energy, 
					if hs no more food energy he has to survive, 
					then if he hasnt energy to survive he dies */
					/*if(swarm[i].capacity_to_eat > 0)
						swarm[i].capacity_to_eat -= Speed_of_digestion;
					else if(swarm[i].capacity_to_eat <= 0 && swarm[i].capacity_to_survive > 0)
						swarm[i].capacity_to_survive -= Speed_of_exctinction;
					else if (swarm[i].capacity_to_eat <= 0 && swarm[i].capacity_to_survive <= 0)*/
                  	swarm[i].capacity_to_survive -= SpeedOfDeath;
                  	if(swarm[i].capacity_to_survive <= 0)
					{
						printf("hey number %d died bro\n", i);
						adios(swarm, swarmSize, i); //death of little cockroach
						printf("%d cockroach left\n", *swarmSize);
					}
					if(*swarmSize == 0)
					{
						printf("Everybody died... :(\n");
						exit(0);
					}	

					/// Rule 1: avoid being alone
					double sumX = -0.;
					double sumY = -0.;
					int neighbors = 0;
					for (int n = 0; n < *swarmSize; ++n) {	// All the neighbors
						if (i != n) {
							const double deltaX = swarm[n].x-swarm[i].x;
							const double deltaY = swarm[n].y-swarm[i].y;
							const double hypotenuse = hypot(deltaX, deltaY);
							if (hypotenuse < Horizon) {
								const double weight = pow(hypotenuse, -1./PowerForWeights);
								sumX += weight*deltaX;
								sumY += weight*deltaY;
								++neighbors;
							}
						}
					}
					if (neighbors != 0) {
						double normalization = hypot(sumX, sumY);
						sumX = sumX/normalization*WeightOfNeighbors+(1.-WeightOfNeighbors)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
						sumY = sumY/normalization*WeightOfNeighbors+(1.-WeightOfNeighbors)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
						normalization = hypot(sumX, sumY);
						swarm[i].speedTheta = atan2(sumY/normalization, sumX/normalization);
					}
				}
				{
					// Rule 3: mimic the neighbors
					double sumX = -0.;
					double sumY = -0.;
					int models = 0;
					for (int n = 0; n < *swarmSize; ++n) {	// All the neighbors
						if (i != n) {
							const double deltaX = swarm[n].x-swarm[i].x;
							const double deltaY = swarm[n].y-swarm[i].y;
							const double hypotenuse = hypot(deltaX, deltaY);
							if (hypotenuse < MimicHorizon) {
								const double weight = pow(hypotenuse, -1./PowerForWeights);
								sumX += weight*cos(swarm[n].speedTheta)*swarm[n].speedRho;
								sumY += weight*sin(swarm[n].speedTheta)*swarm[n].speedRho;
								++models;
							}
						}
					}
					if (models != 0) {
						const double normalization = hypot(sumX, sumY);
						sumX = sumX/normalization*WeightOfMimic+(1.-WeightOfMimic)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
						sumY = sumY/normalization*WeightOfMimic+(1.-WeightOfMimic)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
						//swarm[i].speedTheta = atan2(sumY, sumX);
					}
				}

				{
					// Rule 2: don't move too close
					double closerDistance = INFINITY;
					int closerIndex = i;
					for (int n = 0; n < *swarmSize; ++n) {	// All the neighbors
						if (i != n) {
							const double deltaX = swarm[n].x-swarm[i].x;
							const double deltaY = swarm[n].y-swarm[i].y;
							const double hypotenuse = hypot(deltaX, deltaY);
							if (hypotenuse < Bubble && hypotenuse < closerDistance) {
								closerDistance = hypotenuse;
								closerIndex = n;
							}
						}
					}
					if (closerIndex != i) {
						const double sumX = -(swarm[closerIndex].x-swarm[i].x)/closerDistance*WeightOfEscape+(1.-WeightOfEscape)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
						const double sumY = -(swarm[closerIndex].y-swarm[i].y)/closerDistance*WeightOfEscape+(1.-WeightOfEscape)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
						swarm[i].speedTheta = atan2(sumY, sumX);
					}
				}
				{
					// Rule 5: go to food areas to eat
					int i_close_food;
					double hypothenuse_close_food;
					indexCloseFood(swarm[i], nb_foodPoints, foodPoints, &i_close_food, &hypothenuse_close_food);
						if (hypothenuse_close_food > foodPoints[i_close_food].rayon) {
							swarm[i].mode = Walking;
							const double sumX = (foodPoints[i_close_food].x-swarm[i].x)/hypothenuse_close_food*WeightOfFoodApproach+(1.-WeightOfFoodApproach)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
							const double sumY = (foodPoints[i_close_food].y-swarm[i].y)/hypothenuse_close_food*WeightOfFoodApproach+(1.-WeightOfFoodApproach)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
							swarm[i].speedTheta = atan2(sumY, sumX);
						}
						else swarm[i].mode = Eating;
				}
				{
					// Rule 4: avoid the light
					if (lightAbscissa >= 0 && lightOrdinate >= 0) 
					{
						const double deltaX = lightAbscissa-swarm[i].x;
						const double deltaY = lightOrdinate-swarm[i].y;
						const double hypotenuse = hypot(deltaX, deltaY);
						if (hypotenuse < lightBubble) 
						{
							if(valorisation(swarm[i]) > ValorisationLight)
							{
								const double sumX = -(lightAbscissa-swarm[i].x)/hypotenuse*WeightOflightEscape+(1.-WeightOflightEscape)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
								const double sumY = -(lightOrdinate-swarm[i].y)/hypotenuse*WeightOflightEscape+(1.-WeightOflightEscape)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
								swarm[i].speedTheta = atan2(sumY, sumX);
							}
							if (rand_a_b(0, 1000) <= ProbaPredateurWalking)
							{
								printf("hey number %d died because of PREDATOR while walking\n", i);
								adios(swarm, swarmSize, i); //death of little cockroach
								if(*swarmSize == 0)
								{
									printf("Everybody died... :(\n");
									exit(0);
								}
							}
						}
					}
				}
				{
					// Final Rule: stay in the box
					bool tooClose = false;
					double sumX = -0.;
					double sumY = -0.;
					const double deltaX = WindowWidth-swarm[i].x;
					const double deltaY = WindowHeight-swarm[i].y;
					if (deltaX < MinDistanceFromBoxEdges) 
					{
						tooClose = true;
						sumX = (MinDistanceFromBoxEdges-swarm[i].x)/swarm[i].x*WeightOfEscape+(1.-WeightOfEscape)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
					}
					else if (deltaX > WindowWidth-MinDistanceFromBoxEdges) 
					{
						tooClose = true;
						sumX = (( WindowWidth-MinDistanceFromBoxEdges)-swarm[i].x)/swarm[i].x*WeightOfEscape+(1.-WeightOfEscape)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
					}
					if (deltaY < MinDistanceFromBoxEdges) 
					{
						tooClose = true;
						sumY = (MinDistanceFromBoxEdges-swarm[i].y)/swarm[i].y*WeightOfEscape+(1.-WeightOfEscape)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
					}
					else if (deltaY > WindowHeight-MinDistanceFromBoxEdges) 
					{
						tooClose = true;
						sumY = ((WindowHeight-MinDistanceFromBoxEdges)-swarm[i].y)/swarm[i].y*WeightOfEscape+(1.-WeightOfEscape)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
					}
					if(tooClose)
						swarm[i].speedTheta = atan2(sumY, sumX);
				}
				break;
		}
	}
	// Update position
	for (int i = 0; i < *swarmSize; ++i) {
		swarm[i].x += swarm[i].speedRho*cos(swarm[i].speedTheta);
		swarm[i].y += swarm[i].speedRho*sin(swarm[i].speedTheta);
	}
}

int main(int argc, char *argv[]) {
	initialiseGfx(argc, argv);

	prepareFenetreGraphique(argv[argc-1], WindowWidth, WindowHeight);
	lanceBoucleEvenements();

	return 0;
}


void gestionEvenement(EvenementGfx event) {
	static Cockroach *cockroach = NULL;
	static bool displaylight = false;
	static int lightAbscissa = -1;
	static int lightOrdinate = -1;

	static int nb_foodPoints;
	static POINT *foodPoints = NULL; 
	
	switch (event) {
		case Initialisation:
			srand(time(NULL)); //seed initialization for random values
			demandeTemporisation(20);
			cockroach = initializeSwarm(NumberOfCockroachs);
			nb_foodPoints = rand_a_b(1,nb_max_foodPoints+1);
			foodPoints = positionsFoodAreas(nb_foodPoints);
			break;

		case Temporisation:
			updateSwarm(cockroach, &NumberOfCockroachs, lightAbscissa, lightOrdinate, nb_foodPoints, foodPoints);
			rafraichisFenetre();
			break;

		case Affichage:
			effaceFenetre (255, 255, 255);
			if (displaylight)
			{
				couleurCourante(255, 235, 0);
				circle(abscisseSouris(), ordonneeSouris(), lightBubble);
			}
			create_and_displayFood(foodPoints,nb_foodPoints);
			displaySwarm(cockroach, NumberOfCockroachs);
			break;

		case Clavier:
			switch (caractereClavier()) {
				case 'Q':
				case 'q':
					exit(0);
					break;
				case 'R':
				case 'r':
					free(cockroach);
					cockroach = initializeSwarm(NumberOfCockroachs);
					break;
			}
			break;

		case BoutonSouris:
			if (etatBoutonSouris() == GaucheAppuye)
				displaylight = true;
			else if (etatBoutonSouris() == GaucheRelache)
				displaylight = false;
		case Souris:
			if (displaylight) {
				lightAbscissa = abscisseSouris();
				lightOrdinate = ordonneeSouris();   
			}
			else {
				lightAbscissa = -1;
				lightOrdinate = -1;
			}
			break;

		case ClavierSpecial: 
		case Inactivite:
		case Redimensionnement: 
			break;
	}
}
