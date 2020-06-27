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
} Body;

typedef struct Mem {
	char * memory;
	size_t size;
}mem_t;

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
	
	//printf("Contents: %s", buffer.memory);
	
	json_error_t error;
	json_t * root = json_loads(buffer.memory, 0, &error);
	
	/*if(!json_is_object(root)) {
		fprintf(stderr, "Error: line: %d Column: %d \n Message: %s\n\n", error.line, error.column, error.text);
	}
	
	json_t * englishName_obj = json_object_get(root, "englishName");
	const char* englishName = json_string_value(englishName_obj);
	printf("English Name: %s\n\n", englishName);
	
	json_t * moons_obj = json_object_get(root, "moons");
	json_t * thisMoon_obj = json_array_get(moons_obj, 0);
	json_t * thisMoonName_obj = json_object_get(thisMoon_obj,"moon");
	const char* moon = json_string_value(thisMoonName_obj);
	printf("Moons : %s\n\n", moon);
	
	json_t * mass_obj = json_object_get(root, "mass");
	json_t * massValue_obj = json_object_get(mass_obj, "massValue");
	double massValue = json_real_value(massValue_obj);
	json_t * massExp_obj = json_object_get(mass_obj, "massExponent");
	json_int_t massExp = json_integer_value(massExp_obj);
	printf("Mass : %f x 10^%lld\n\n", massValue, massExp);
	
	*/
	
	
	return root;
}

void print_info(Body* body) {
	printf("Name: %s\nRadius: %lf\nMass: %f\nGravity: %lf\n\n", body->name, body->radius, body->mass, body->gravity);
}

int showDetails(char* body_name, Body bodies[], int array_len) {
		
	for(int i = 0; i < array_len; ++i) {
		if (strncmp(body_name, bodies[i].name, 15) == 0){
			print_info(&bodies[i]);
			return 0;
		}
	}
	return 1;
}

void getDetailsAboutABody(Body bodies [], int array_len) {
	system("clear");
		
	puts("Insert Name of body:\n");
	char bodyName[256];
	gets(bodyName);
	puts("its here");
	int status = showDetails(bodyName, bodies, array_len);
	if (status == 1) printf("Could not find body with name %s\n\n", bodyName);
}

int main() {
	
	system("clear");
	puts("Initializing...");
	json_t * root = http_get_json_data("https://api.le-systeme-solaire.net/rest/bodies");
	
	
	json_t * bodies_obj = json_object_get(root, "bodies");
	Body bodies[json_array_size(bodies_obj)];
	int array_len = 0;
	for(int i = 0; i < json_array_size(bodies_obj); ++i, array_len++){
		json_t * body_obj = json_array_get(bodies_obj, i);
		Body * b = malloc(sizeof(Body));
		
		b->name = json_string_value(json_object_get(body_obj, "name")) + '\0';
		
		b->radius = json_real_value(json_object_get(body_obj, "meanRadius"));
		
		json_t * mass_obj = json_object_get(body_obj, "mass");
		b->mass = (double)(json_real_value(json_object_get(mass_obj, "massValue"))) * pow(10.0,(double) json_integer_value(json_object_get(mass_obj, "massExponent")));
		
		b->gravity = json_real_value(json_object_get(body_obj, "gravity"));
		
		bodies[i] = *b;
		free(b);
	}
	
	for(;;){
		system("clear");
		puts("1. Get Detailed Information About a Body\n2. Exit");
		char c = getchar();
		getchar();
		if (c == '1') getDetailsAboutABody(bodies, array_len); 
		if (c == '2') exit(0);
		if (c != '1') continue;
				
		puts("Press Enter to continue...");
		getchar();
		
	}
	
}

