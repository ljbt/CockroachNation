
// constants definitions -----------------------------------

// Default width and height
#define WindowWidth 600
#define WindowHeight 600



// types definitions ------------------------------------------

typedef enum Gender {Male = 0,Female = 1} Gender;
typedef enum Development_stage {Larva,Adult} Development_stage;
typedef enum Mode {Eating, Walking} Mode;

typedef struct {
	int id; // number to identify a cockroach
	float x;
	float y;
	float speedRho;
	float speedTheta;
	Gender gender; // male or female, for reproduction	
	Development_stage growth;
	int adult_date;
	bool time_for_reproduction;
	int last_reproduction_day;
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


// simulation.c functions ------------------------------

void circle(float xCenter, float yCenter, float radius);
int rand_a_b(int a, int b);
void affichePoint(POINT p);
const char* getGenderName (Gender gender);
bool opposite_gender (Gender g1, Gender g2);