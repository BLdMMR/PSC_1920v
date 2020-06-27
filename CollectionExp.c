#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include <curl/curl.h>
#include <math.h>

typedef struct Body {
	const char* name;
	double radius;
	double mass;
	double gravity;
	int isPlanet;
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

BodyCollection * first;

static size_t writeCallback(char* contents, size_t size, size_t nmemb, void *userp)
{
	size_t realSize = size * nmemb;
	struct Mem * mem = (struct Mem*) userp;
	
	char* ptr = realloc(mem->memory, mem->size + realSize + 1);
	if (ptr == NULL) return 0;
	
	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realSize);
	mem->size += realSize;
	mem->memory[mem->size] = 0;
	
	return realSize;
}

json_t * http_get_json_data(const char* url){
	
	mem_t buffer;
	buffer.memory = malloc(1);
	buffer.size = 0;
	
	CURL *curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		FILE * file = fopen("temp.txt", "w");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&buffer);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			
		}
		curl_easy_cleanup(curl);
	}
	else {
		return NULL;
	}
		
	json_error_t error;
	json_t * root = json_loads(buffer.memory, 0, &error);
		
	return root;
}

void putInCollection(BodyCollection * bc) {
	BodyCollection * curr = first;
	while(curr->next != NULL) {
		curr = curr->next;
	}
	curr->next = bc;
	bc->prev = curr;
	bc->next = NULL;
}	

void build_data_structure(json_t * root) {
	first = malloc(sizeof(BodyCollection));
	first->body = NULL;
	first->prev = NULL;
	first->next = NULL;
	
	json_t * bodies_obj = json_object_get(root, "bodies");
	
	for(int i = 0; i < json_array_size(bodies_obj); ++i){
		
		json_t * body_obj = json_array_get(bodies_obj, i);
		
		Body * b = malloc(sizeof(Body));
		BodyCollection * bodyCollection = malloc(sizeof(BodyCollection));
		
		b->name = json_string_value(json_object_get(body_obj, "englishName")) + '\0';
		b->radius = json_real_value(json_object_get(body_obj, "meanRadius"));
		b->isPlanet = json_boolean_value(json_object_get(body_obj, "isPlanet"));
	
		json_t * mass_obj = json_object_get(body_obj, "mass");
	
		b->mass = (double)(json_real_value(json_object_get(mass_obj, "massValue"))) * pow(10.0,(double) json_integer_value(json_object_get(mass_obj, "massExponent")));
		b->gravity = json_real_value(json_object_get(body_obj, "gravity"));
		
		bodyCollection->body = b;
		putInCollection(bodyCollection);
		
	}
}

void showDetails(char* bodyName) {
	BodyCollection * curr = first;
	Body* bods;
	while(curr->next != NULL) {
	 	curr = curr->next;
		bods = curr->body;
		if((strcmp(bodyName, bods->name)) == 0){
			printf("Name: %s\nRadius: %lf\nMass: %f\nGravity: %lf\n\n", bods->name, bods->radius, bods->mass, bods->gravity);
			return;
		}
		

	}
	printf("Unable to find body with the name '%s'\n\n\n", bodyName);
	
}

void getDetailsAboutABody() {
	system("clear");
	puts("Insert Name Of Body");
	char* name;
	fgets(name, 50, stdin);
	size_t len = strlen(name) - 1;
	if (*name && name[len] == '\n') name[len] = '\0';
	showDetails(name);
	
}

int solar_insert_body(Body * body) {
	BodyCollection * curr = first;
	while(curr->next != NULL) {
	 	curr = curr->next;
	 }
	BodyCollection* forThisBody = malloc(sizeof(BodyCollection));
	forThisBody->body = body;
	forThisBody->next = NULL;
	forThisBody->prev = curr;
	curr->next = forThisBody;
	return 0;
}

void insertSolarBody() {
	system("clear");
	
	puts("Insert the name of the body: \n");
	char* name = malloc(50);
	fgets(name, 50, stdin);
	size_t nameLen = strlen(name) - 1;
	if (*name && name[nameLen] == '\n') name[nameLen] = '\0';
	
	puts("Insert it's radius: \n");
	char* radius = malloc(50);
	fgets(radius, 50, stdin);
	size_t radiusLen = strlen(radius) - 1;
	if (*radius && radius[radiusLen] == '\n') radius[radiusLen] = '\0';
	
	puts("Insert it's mass: \n");
	char* mass = malloc(50);
	fgets(mass, 50, stdin);
	size_t massLen = strlen(mass) - 1;
	if (*mass && mass[massLen] == '\n') mass[massLen] = '\0';
	
	puts("Insert its gravity: \n");
	char* gravity = malloc(50);
	fgets(gravity, 50, stdin);
	size_t gravityLen = strlen(gravity) - 1;
	if (*gravity && gravity[gravityLen] == '\n') gravity[gravityLen] = '\0';
	
	puts("Is it a Planet? (y=1/n=0)\n");
	char* isP = malloc(50);
	fgets(isP, 50, stdin);
	size_t isPLen = strlen(isP) - 1;
	if (*isP && isP[isPLen] == '\n') isP[isPLen] = '\0';
	
	Body * body = malloc(sizeof(Body));
	body->name = name;
	body->radius = atof(radius);
	body->mass = atof(mass);
	body->gravity = atof(gravity);
	body->isPlanet = atoi(isP);
	
	free(name);
	free(radius);
	free(mass);
	free(gravity);
	free(isP);
	
	int status = solar_insert_body(body);
	if (!status) puts("Body inserted\n");
	
}

BodyCollection *solar_get_planets(){
	puts("1");
	BodyCollection * currList = first;
	puts("2");
	BodyCollection * currElem = malloc(sizeof(BodyCollection));
	puts("3");
	BodyCollection * ret = malloc(sizeof(BodyCollection));
	puts("4");
	ret->body = NULL;
	ret->next = currElem;
	ret->prev = NULL;
	currElem->prev = ret;
	puts("5");
	BodyCollection * copy = malloc(sizeof(BodyCollection));
	puts("6");
	Body* bods;
	puts("7");
	while(currList->next != NULL) {
		puts("8");
		bods = currList->body;
		puts("9");
		if(bods->isPlanet) {
			puts("10");
			copy->body = currList->body;
			puts("11");
			copy->prev = currElem;
			puts("12");
			copy->next = NULL;
			puts("13");
			currElem->next = copy;
			puts("14");
			currElem = currElem->next;
			puts("15");
		}
	}
	return ret;
}

void listPlanets() {
	BodyCollection * coll = solar_get_planets();
	Body * body = coll->body;
	coll = coll->next;
	while((coll->next) != NULL) {
		body = coll->body;
		printf("%s\n", body->name);
	}
}

int main() {
	system("clear");
	puts("Initializing...");
	json_t * root = http_get_json_data("https://api.le-systeme-solaire.net/rest/bodies");
	
	build_data_structure(root);
		
	for(;;){
		system("clear");
		
		puts("1. Insert Solar Body\n2. Get Detailed Information About a Body\n3. List Bodies That Are Planets\n\n0. Exit");
		char c = getchar();
		getchar();
		
		switch(c) {
			case '0': 
				exit(0);
				break;
			case '1': 
				insertSolarBody();
				break;
			case '2':
				getDetailsAboutABody(); 
				break;
			case '3':
				listPlanets();
				break;
			default: continue;
		}
				
		puts("Press Enter to continue...");
		getchar();
	}
}

/*
	 * THIS IS A TEST TO SEE IF COLLECTION HAS EVERY ELEMENT
	 * 
	BodyCollection * curr = first->next;
	Body *bods = curr->body;
	printf("%s", bods->name);
	curr = curr->next;
	bods = curr->body;
	printf("%s", bods->name);
	
	BodyCollection * curr = first;
	Body* bods;
	while(curr->next != NULL) {
	 	curr = curr->next;
		bods = curr->body;
		printf("%s\n", bods->name);
	}
	
	puts("Everything went right");
	*/
