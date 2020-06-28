#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include <curl/curl.h>
#include <math.h>
#include "solarlib.c"

BodyCollection * first;

int main() {
	system("clear");
	puts("Initializing...");
	json_t * root = http_get_json_data("https://api.le-systeme-solaire.net/rest/bodies");
	
	build_data_structure(root);
		
	for(;;){
		system("clear");
		
		puts("1. Insert Solar Body\n2. Get Detailed Information About a Body\n3. List Bodies That Are Planets\n4. Get Satelites\n\n0. Exit");
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
			case '4':
				getSatellites();
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
