#ifndef solarlib_h__
#define solarlib_h__

typedef struct Body {
	const char* name;
	double radius;
	double mass;
	double gravity;
	int isPlanet;
	const char* arroundPlanet;
} Body;

typedef struct Mem {
	char * memory;
	size_t size;
}mem_t;

typedef struct BodyCollection{
	Body * body;
	struct BodyCollection *next;
	struct BodyCollection *prev;
}BodyCollection;

static size_t writeCallback(char* contents, size_t size, size_t nmemb, void *userp);

json_t * http_get_json_data(const char* url);

void putInCollection(BodyCollection * bc, BodyCollection * src);

void build_data_structure(json_t * root);

void showDetails(char* bodyName);

void getDetailsAboutABody();

int solar_insert_body(Body * body);

void insertSolarBody();

BodyCollection *solar_get_planets();

BodyCollection *solar_get_satellites();

void listPlanets();

void getSatellites();

#endif
