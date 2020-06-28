#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include <curl/curl.h>
#include <math.h>

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

json_t * http_get_json_data(const char* url)
{
	
	mem_t buffer;
	buffer.memory = malloc(1);
	buffer.size = 0;
	
	CURL *curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
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

void putInCollection(BodyCollection * bc, BodyCollection * src) 
{
	BodyCollection * curr = src;
	while(curr->next != NULL) {
		curr = curr->next;
	}
	curr->next = bc;
	bc->prev = curr;
	bc->next = NULL;
}	


void build_data_structure(json_t * root) 
{
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
		
		b->arroundPlanet = json_string_value(json_object_get(json_object_get(body_obj, "aroundPlanet"), "planet"));
		bodyCollection->body = b;
		putInCollection(bodyCollection, first);
		
	}
}

void showDetails(char* bodyName) {
	BodyCollection * curr = first;
	Body* bods;
	while(curr->next != NULL) {
	 	curr = curr->next;
		bods = curr->body;
		if((strcmp(bodyName, bods->name)) == 0){
			printf("Name: %s\nRadius: %lf\nMass: %f\nGravity: %lf\nIs Planet:%d\nArround Planet: %s\n\n", bods->name, bods->radius, bods->mass, bods->gravity, bods->isPlanet, bods->arroundPlanet);
			return;
		}
		

	}
	printf("Unable to find body with the name '%s'\n\n\n", bodyName);
	
}

void getDetailsAboutABody() {
	system("clear");
	puts("Insert Name Of Body");
	char* name = malloc(sizeof(char*));
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
	BodyCollection * currList = first->next;
	BodyCollection * currElem;
	BodyCollection * ret = malloc(sizeof(BodyCollection));
	ret->body = NULL;
	ret->next = NULL;
	ret->prev = NULL;
	Body* bods;
	do {
		bods = currList->body;
		if(bods->isPlanet == 1) {
			currElem = malloc(sizeof(BodyCollection));
			currElem->body = currList->body;
			putInCollection(currElem, ret);
		}
		currList = currList->next;
	} while(currList->next != NULL);
	return ret;
}

BodyCollection *solar_get_satellites(){
	BodyCollection * currList = first->next;
	BodyCollection * currElem;
	BodyCollection * ret = malloc(sizeof(BodyCollection));
	ret->body = NULL;
	ret->next = NULL;
	ret->prev = NULL;
	Body* bods;
	do {
		bods = currList->body;
		if(bods->arroundPlanet != NULL) {
			currElem = malloc(sizeof(BodyCollection));
			currElem->body = currList->body;
			putInCollection(currElem, ret);
		}
		currList = currList->next;
	} while(currList->next != NULL);
	return ret;
}


void listPlanets() {
	BodyCollection * coll = solar_get_planets();
	coll = coll->next;
	Body * body;
	while((coll->next) != NULL) {
		body = coll->body;
		printf("%s\n", body->name);
		coll = coll->next;
	}
}

void getSatellites(){
	BodyCollection * coll = solar_get_satellites();
	coll = coll->next;
	Body * body;
	while((coll->next) != NULL) {
		body = coll->body;
		printf("%s\n", body->name);
		coll = coll->next;
	}
}
