#include <stdlib.h>		// For exit()
#include <stdio.h>		// For printf()
#include <math.h>		// For sin() and cos()
#include <strings.h>	// For bzero()
#include <stdbool.h>
#include "GfxLib.h"		// To do simple graphics
#include "ESLib.h"		// For valeurAleatoire()

#ifndef M_PI
#define M_PI 3.141592654
#endif

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

// Default width and height
#define WindowWidth 600
#define WindowHeight 600
#define NumberOfCockroachs 300
const double PowerForWeights = 3;	// To decrease or increase the influence of the distance
const double Horizon = 300.;		// Units to look for neighbors
const double WeightOfNeighbors = .1;
const double Bubble = 5.;			// A minimal distance between cockroachs
const double WeightOfEscape = .8;
const double WeightOfMimic = .3;
const double MimicHorizon = 30.;		// Units to look for neighbors to mimic
const double PredatorBubble = 40.;		// A minimal distance with the predator
const double WeightOfPredatorEscape = .8;
const double MinDistanceFromBoxEdges = 10.;// A minimal distance with the edges of the box

enum {RandomWalker, SimpleCockroach} mode = SimpleCockroach /*RandomWalker*/;

typedef struct {
	float x;
	float y;
	float speedRho;
	float speedTheta;
} Cockroach;

Cockroach *initializeSwarm(const int swarmSize) {
	Cockroach *swarm = (Cockroach*)calloc(swarmSize, sizeof(Cockroach));
	const double GroupSpeedTheta = 0.;//valeurAleatoire()*2.*M_PI;
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
		};
	}
	return swarm;
}

void displaySwarm(const Cockroach *swarm, const int swarmSize) {
	epaisseurDeTrait(2);
	couleurCourante(0, 0, 0);
	for (int i = 0; i < swarmSize; ++i)
		point(swarm[i].x, swarm[i].y);//, 1);
}

void updateSwarm(Cockroach *swarm, const int swarmSize, int predatorAbscissa, int predatorOrdinate) {
	for (int i = 0; i < swarmSize; ++i) {	// All the individuals
		switch (mode) {
			case RandomWalker:
				swarm[i].speedTheta = valeurAleatoire()*2.*M_PI;
				break;
			case SimpleCockroach:
				{
					// Rule 1: avoid being alone
					double sumX = -0.;
					double sumY = -0.;
					int neighbors = 0;
					for (int n = 0; n < swarmSize; ++n) {	// All the neighbors
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
						//swarm[i].speedTheta = atan2(sumY/normalization, sumX/normalization);
					}
				}
				{
					// Rule 3: mimic the neighbors
					double sumX = -0.;
					double sumY = -0.;
					int models = 0;
					for (int n = 0; n < swarmSize; ++n) {	// All the neighbors
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
						swarm[i].speedTheta = atan2(sumY, sumX);
					}
				}
				{
					// Rule 2: don't move too close
					double closerDistance = INFINITY;
					int closerIndex = i;
					for (int n = 0; n < swarmSize; ++n) {	// All the neighbors
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
					// Rule 4: avoid the predator
					if (predatorAbscissa >= 0 && predatorOrdinate >= 0) {
						const double deltaX = predatorAbscissa-swarm[i].x;
						const double deltaY = predatorOrdinate-swarm[i].y;
						const double hypotenuse = hypot(deltaX, deltaY);
						if (hypotenuse < PredatorBubble) {
							const double sumX = -(predatorAbscissa-swarm[i].x)/hypotenuse*WeightOfPredatorEscape+(1.-WeightOfPredatorEscape)*cos(swarm[i].speedTheta)*swarm[i].speedRho;
							const double sumY = -(predatorOrdinate-swarm[i].y)/hypotenuse*WeightOfPredatorEscape+(1.-WeightOfPredatorEscape)*sin(swarm[i].speedTheta)*swarm[i].speedRho;
							//swarm[i].speedTheta = atan2(sumY, sumX);
						}
					}
				}
				{
					// Rule 5: stay in the box
					bool tooClose = false;
					double sumX = -0.;
					double sumY = -0.;
					const double deltaX = WindowWidth-swarm[i].x;
					const double deltaY = WindowHeight-swarm[i].y;
					//const double hypotenuse = hypot(deltaX, deltaY);
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
	for (int i = 0; i < swarmSize; ++i) {
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
	static bool displayPredator = false;
	static int predatorAbscissa = -1;
	static int predatorOrdinate = -1;
	
	switch (event) {
		case Initialisation:
			demandeTemporisation(20);
			cockroach = initializeSwarm(NumberOfCockroachs);
			break;

		case Temporisation:
			updateSwarm(cockroach, NumberOfCockroachs, predatorAbscissa, predatorOrdinate);
			rafraichisFenetre();
			break;

		case Affichage:
			effaceFenetre (255, 255, 255);
			couleurCourante(255, 235, 0);
			if (displayPredator)
				circle(abscisseSouris(), ordonneeSouris(), PredatorBubble);
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
				displayPredator = true;
			else if (etatBoutonSouris() == GaucheRelache)
				displayPredator = false;
		case Souris:
			if (displayPredator) {
				predatorAbscissa = abscisseSouris();
				predatorOrdinate = ordonneeSouris();   
			}
			else {
				predatorAbscissa = -1;
				predatorOrdinate = -1;
			}
			break;

		case ClavierSpecial: 
		case Inactivite:
		case Redimensionnement: 
			break;
	}
}
