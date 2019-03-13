// Default width and height
#define WindowWidth 600
#define WindowHeight 600

typedef enum Gender {Male,Female} Gender;
typedef enum Development_stage {Larva,Adult} Development_stage;
typedef enum Mode {Eating, Walking} Mode;

typedef struct {
	float x;
	float y;
	float speedRho;
	float speedTheta;
	Gender gender; // male or female, for reproduction	
	Development_stage growth;
	Mode mode; // a cockroach can walk or eat, that's it
	double capacity_to_eat;
	double capacity_to_avoid_light;
	double capacity_to_survive;
} Cockroach;


typedef struct POINT POINT;
struct POINT{
	int x, y;
	double rayon;
};
