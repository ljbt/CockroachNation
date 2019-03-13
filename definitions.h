// Default width and height
#define WindowWidth 600
#define WindowHeight 600

typedef enum Gender {Male,Female} Gender;

typedef struct {
	float x;
	float y;
	float speedRho;
	float speedTheta;
	Gender gender; // male or female, for reproduction	
	enum {Eating, Walking} mode; // a cockroach can walk or eat, that's it
	double capacity_to_eat;
	double capacity_to_avoid_light;
	double capacity_to_survive;
} Cockroach;


typedef struct POINT POINT;
struct POINT{
	int x, y;
	double rayon;
};
