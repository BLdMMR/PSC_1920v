#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#define MAX_LEN_FILE 0xff


typedef struct Mem {
	char* memory;
	size_t size;
} mem_t;

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

int http_get(const char* url, const char* filename)
{
	CURL *curl =  curl_easy_init();
	if(curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		FILE *file = fopen(filename, "w");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
	}
	else{ 
		curl_easy_cleanup(curl);
		return 1;
	}
	curl_easy_cleanup(curl);
	return 0;
	
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
	//printf("%s\n\n\n", buffer.memory);
	
	json_error_t error;
	json_t * root = json_loads(buffer.memory, 0, &error);
	
	if(!json_is_object(root)) {
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
	
	
	
	
	return root;
}

int test_1() {
	int status = http_get("https://img.ibxk.com.br/2017/01/26/26115632211129.jpg?w=1040", "index.jpg");
	
	return status;
}

int test_2() {
	json_t * status = http_get_json_data("https://api.le-systeme-solaire.net/rest/bodies/terre");
	if (status != NULL) {
		return 0;
	}
	return 1;
}

int main() {
	int status = test_1();
	int status2 = test_2();
	if(!status) puts("Test 1 succeeded: Get request complete.");
	else puts("Test 1 failed: Get request not complete.");
	if(!status2) puts("Test 2 succeeded: Get request complete.");
	else puts("Test 2 failed: Get request not complete.");

	return 0;
}

