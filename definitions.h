
// constants definitions -----------------------------------
#ifndef M_PI
#define M_PI 3.141592654
#endif

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
	double food_attraction;
	double light_sensitivity;
	double life; //jauge bouffe
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
void displayCockroachDetails(Cockroach insect);
void displaySwarmDetails (Cockroach *swarm, const int swarmSize);
Gender alea_gender();
const char* getGenderName (Gender gender);
bool opposite_gender (Gender g1, Gender g2);
void init_cockroach (Cockroach *swarm, const int idx);
Cockroach *initializeSwarm(int swarmSize) ;
void displaySwarm(const Cockroach *swarm, int swarmSize) ;
void updateSwarm(Cockroach **swarm, int *swarmSize, int lightAbscissa, int lightOrdinate, int nb_foodPoints, POINT *foodPoints, const int day);

