// Default width and height
#define WindowWidth 600
#define WindowHeight 600

typedef struct {
	float x;
	float y;
	float speedRho;
	float speedTheta;
	enum {Eating, Walking} mode;
} Cockroach;


typedef struct POINT POINT;
struct POINT{
	int x, y;
	double rayon;
};
