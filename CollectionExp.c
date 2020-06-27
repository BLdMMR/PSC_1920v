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

int main() {
	system("clear");
	puts("Initializing...");
	json_t * root = http_get_json_data("https://api.le-systeme-solaire.net/rest/bodies");
	
	first = malloc(sizeof(BodyCollection));
	first->body = NULL;
	first->prev = NULL;
	first->next = NULL;
	
	json_t * bodies_obj = json_object_get(root, "bodies");
	
	//Body bodies[json_array_size(bodies_obj)];
	//int array_len = 0;
	
	for(int i = 0; i < json_array_size(bodies_obj); ++i/*, array_len++*/){
		
		json_t * body_obj = json_array_get(bodies_obj, i);
		
		Body * b = malloc(sizeof(Body));
		BodyCollection * bodyCollection = malloc(sizeof(BodyCollection));
		
		b->name = json_string_value(json_object_get(body_obj, "name")) + '\0';
		b->radius = json_real_value(json_object_get(body_obj, "meanRadius"));
		b->isPlanet = json_boolean_value(json_object_get(body_obj, "isPlanet"));
	
		json_t * mass_obj = json_object_get(body_obj, "mass");
	
		b->mass = (double)(json_real_value(json_object_get(mass_obj, "massValue"))) * pow(10.0,(double) json_integer_value(json_object_get(mass_obj, "massExponent")));
		b->gravity = json_real_value(json_object_get(body_obj, "gravity"));
		
		bodyCollection->body = b;
		putInCollection(bodyCollection);
		
		//bodies[i] = *b;
	}
	
	BodyCollection * curr = first;
	Body* bods;
	while(curr->next != NULL) {
		puts("Not here");
		memcpy(bods, &(curr->body), sizeof(Body));
		puts("Not here");
		printf("%s", bods->name);
		puts("Not here");
		curr = curr->next;
	}
	
	puts("Everything went right");
	
}
