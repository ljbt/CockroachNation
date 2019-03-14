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
#include "time.h"
#include "reproduction.h"

#ifndef M_PI
#define M_PI 3.141592654
#endif

int NumberOfCockroachs = 40;
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

const int nb_min_days_without_sex = 1; // number minimum of days to wait before next reproduction
const int IntimateZone = 2;
const double WeightOfPartnerApproach = .3;
const double EatValue = 5.0;
const double SpeedOfDeath = 0.1;
const double ValorisationLight = 50.0;
const double MaxLife = 100.0;
const double SeuilSurvie = 10.0;
const double ProbaPredateurEating = 2.0; // Chance sur 1000
const double ProbaPredateurWalking = 1.0; // Chance sur 1000


//Eat if < 50, avoid light if > 50. 
double valorisation(Cockroach cockroach)
{
  	if (cockroach.capacity_to_survive < SeuilSurvie)
      	return 0.0;
	if(cockroach.food_attraction > cockroach.light_sensitivity)
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

//valeur entiere aleatoire dans [a;b[
int rand_a_b(int a, int b){

    return rand()%(b-a)+a ;
}

void affichePoint(POINT p)
{
	printf("x = %d , y = %d\n", p.x, p.y);
}


/* function that return a random gender (male or female)
	for swarm initialization */
Gender alea_gender()
{
	int g = rand_a_b(0,2); // generate a 0 or a 1
	if (g == 0)
		return Male;
	else
		return Female;
}

const char* getGenderName (Gender gender)
{
	switch(gender)
	{
		case Male: return "male";
			break;
		case Female: return "female";
			break;
		default: return "unknown";
	}
/* 	couleurCourante(51,102,0);													modif Laura
    for(int i=0; i<nb_foodPoints; ++i){
		if (foodPoints[i].rayon < 2.)
			foodPoints[i] = positionFoodArea();
		circle(foodPoints[i].x,foodPoints[i].y,round(foodPoints[i].rayon));
    } */
}

bool opposite_gender (Gender g1, Gender g2)
{
	if(g1 == g2) return false;
	else return true;
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
			.id = i,
			.x = x,
			.y = y,
			.speedRho = 1.,
			.speedTheta = valeurAleatoire()*2.*M_PI,	// GroupSpeedTheta
			.gender = alea_gender(), // male or female
			.growth = Larva, // larva by default
			.adult_date = rand_a_b(1,6), //time from larva to adult (between 2 and 6 days here)
			.time_for_reproduction = false,
			.last_reproduction_day = 0, // no reproduction yet
/* 			.mode = Walking, //walking by default 									modif Raph
			.food_attraction = 0,
			.light_sensitivity = 0,
			.capacity_to_survive = rand_a_b(90,200), //random capacity to survive without food for each cockroach */
			swarm[i].mode = Walking,
			.food_attraction = groupBaseEat + (rand_a_b(0, 20) - 10), // entre 0 et 100 car de base entre 0 et 90 et on ajoute ou enleve 10
			.light_sensitivity = groupBaseLight + (rand_a_b(0, 20) - 10),
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

void updateSwarm(Cockroach *swarm, int *swarmSize, int lightAbscissa, int lightOrdinate, int nb_foodPoints, POINT *foodPoints, const int day) {

	for (int i = 0; i < *swarmSize; ++i) {	// All the individuals

		bool possiblePartner = false;
		int i_close_partner = -1;
		double hypothenuse_close_partner = -1;

		cockroach_evolution(&(swarm[i]),i,day); // larva becomes adult if it's the day
		time_for_reproduction_or_not(&(swarm[i]), nb_min_days_without_sex,day);
		
		switch (swarm[i].mode) {
			case Eating:

				/// Rule: find the one
				if( swarm[i].time_for_reproduction)
				{
//					printf("%s %d its time to find the one\n",getGenderName(swarm[i].gender), swarm[i].id);
					indexClosePartner(swarm, i, *swarmSize, &i_close_partner, &hypothenuse_close_partner);
					if(i_close_partner != -1 ) // possible partner
						possiblePartner = true;
				}

				if (possiblePartner)
				{
	//				printf("%d saw %d\n",swarm[i].id, swarm[i_close_partner].id);
					if (hypothenuse_close_partner > IntimateZone) // partner not in intimate zone change position
					{
//						printf("%d going to %d\n",swarm[i].id, swarm[i_close_partner].id);
						const double sumX = (swarm[i_close_partner].x-swarm[i].x)/hypothenuse_close_partner*WeightOfPartnerApproach+(1.-WeightOfPartnerApproach)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
						const double sumY = (swarm[i_close_partner].y-swarm[i].y)/hypothenuse_close_partner*WeightOfPartnerApproach+(1.-WeightOfPartnerApproach)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
						swarm[i].speedTheta = atan2(sumY, sumX);
					}
					else // partner in intimate zone
					{
						printf("%d and %d have babies !\n", swarm[i].id, swarm[i_close_partner].id);
						reproduction(swarm,i,i_close_partner,swarmSize,day);  // crossover and all in this function ! 
					}
					 
				}
				else // no partner so eat mode
				{
//					printf("no partner for %s %d\n",getGenderName(swarm[i].gender), swarm[i].id);
					swarm[i].speedTheta = valeurAleatoire()*2.*M_PI; // a little bit of movement

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
						// Rule 6: walk when detect light
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
				}
				break;

			case Walking:

				/// Rule: find the one
				if( swarm[i].time_for_reproduction)
				{
//					printf("%s %d its time to find the one\n",getGenderName(swarm[i].gender), swarm[i].id);
					indexClosePartner(swarm, i, *swarmSize, &i_close_partner, &hypothenuse_close_partner);
					if(i_close_partner != -1 ) // possible partner
						possiblePartner = true;
				}

				if (possiblePartner)
				{
//					printf("%d saw %d\n",swarm[i].id, swarm[i_close_partner].id);
					if (hypothenuse_close_partner > IntimateZone) // partner not in intimate zone change position
					{
//						printf("%d going to %d\n",swarm[i].id, swarm[i_close_partner].id);
						const double sumX = (swarm[i_close_partner].x-swarm[i].x)/hypothenuse_close_partner*WeightOfPartnerApproach+(1.-WeightOfPartnerApproach)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
						const double sumY = (swarm[i_close_partner].y-swarm[i].y)/hypothenuse_close_partner*WeightOfPartnerApproach+(1.-WeightOfPartnerApproach)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
						swarm[i].speedTheta = atan2(sumY, sumX);
					}
					else // partner in intimate zone
					{
						printf("%d and %d have babies !\n", swarm[i].id, swarm[i_close_partner].id);
						reproduction(swarm,i,i_close_partner,swarmSize,day);  // crossover and all in this function !
					}
					 
				}
				else // no partner so walk mode
				{
					/*Rule of beeing alive: if he walks he looses food energy, 
					if hs no more food energy he has to survive, 
					then if he hasnt energy to survive he dies */
					/*if(swarm[i].food_attraction > 0)
					swarm[i].food_attraction -= Speed_of_digestion;
					else if(swarm[i].food_attraction <= 0 && swarm[i].capacity_to_survive > 0)
					swarm[i].capacity_to_survive -= Speed_of_exctinction;
					else if (swarm[i].food_attraction <= 0 && swarm[i].capacity_to_survive <= 0)*/
					swarm[i].capacity_to_survive -= SpeedOfDeath;
					if(swarm[i].capacity_to_survive <= 0) 
					{							
						printf("RIP number %d\n", swarm[i].id);
						adios(swarm, swarmSize, i); //death of little cockroach
						printf("%d cockroaches left\n", *swarmSize);
					}
					if(*swarmSize == 0)
					{
						printf("Everybody died... :(\n");
						exit(0);	
					}
					
					{
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
						// Rule: avoid the light
						if (lightAbscissa >= 0 && lightOrdinate >= 0) {
						
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
				}
				break;
		}
		//end of modes rules

		//important rules below
		{
			// Rule: stay in the box
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

	static int day = 1, iteration = 0;
	
	switch (event) {
		case Initialisation:
			srand(time(NULL)); //seed initialization for random values
			demandeTemporisation(20);
			cockroach = initializeSwarm(NumberOfCockroachs);
			nb_foodPoints = rand_a_b(1,nb_max_foodPoints+1);
			foodPoints = positionsFoodAreas(nb_foodPoints);
			break;

		case Temporisation:
			iteration++;
			computes_day(&day,&iteration);
			updateSwarm(cockroach, &NumberOfCockroachs, lightAbscissa, lightOrdinate, nb_foodPoints, foodPoints, day);
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
			display_day(day);
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
