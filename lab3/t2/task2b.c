
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct virus {
    unsigned short SigSize;
    unsigned char* sig;
    char virusName[16];
} virus;

typedef struct link link;
 
struct link {
    link *nextVirus;
    virus *vir;
};

struct fun_desc {
  char *name;
  link* (*fun)(link*);
};


link* fixFile(link* null);
void kill_virus(char *fileName, int signatureOffset, int signatureSize);
link* detect(link *virus_list);
void detect_virus(char *buffer, unsigned int size, link *virus_list);
link* quit(link* null);
link* loadSignatures(link* virus_list);
link* printSignatures(link* virus_list);
void list_print(link *virus_list, FILE* output);
link* list_append(link* virus_list, link* to_add);
void printHex(unsigned char* buffer,unsigned short length, FILE* output);
int readVirus(virus* vir, FILE* input);
void printVirus(virus* vir, FILE* output);


struct fun_desc menu[] = { {"Load signatures", loadSignatures}, { "Print signatures", printSignatures}, {"Detect viruses", detect}, {"Fix file", fixFile}, {"Quit", quit }, { NULL, NULL } };



void printHex(unsigned char* buffer,unsigned short length, FILE* output) {
	size_t i;
	for(i=0; i < length; i++) {
		fprintf(output, "%02hhX ", buffer[i]);
	}
}

int readVirus(virus* vir, FILE* input) {

	char SigSize[2]; 
	fread(SigSize, 1, 2, input); 
	short size = SigSize[0] | (SigSize[1] << 8);
	if (size == 0) return 0;
	vir->SigSize = size;
	//add size allocations to vir->sig
	fread(vir->sig, 1, vir->SigSize, input);
	return fread(vir->virusName, 1, 16, input);
}

void printVirus(virus* vir, FILE* output) {
	// VIRUS 5 31 32 33 34 35
	fprintf(output, "virus name: %s\n", vir->virusName);
	fprintf(output, "virus signature length: %d\n", vir->SigSize);
        fprintf(output, "virus signature:\n");
	printHex(vir->sig, vir->SigSize, output);
	fprintf(output,"\n");
}

void list_print(link *virus_list, FILE* output) {
	link *curr = virus_list;
	while(curr != NULL) {
		printVirus(curr->vir, output);
		printf("\n");
		curr = curr->nextVirus;
	}
}

link* list_append(link* virus_list, link* to_add) {
	//append first
/*
	to_add->nextVirus = virus_list; // link->list
	return to_add;
*/
	//append last
	link *curr = virus_list;
	while(curr->nextVirus != NULL) {
		curr = curr->nextVirus;
	}
	curr->nextVirus = to_add;
	return virus_list;

}

void list_free(link *virus_list) {
	if(virus_list != NULL) {
		list_free(virus_list->nextVirus);
		free(virus_list->vir->sig);
		free(virus_list->vir);
		free(virus_list);
	}
}

link* loadSignatures(link* virus_list) {
	printf("enter signature file: ");
	char *file_name = malloc(300);
	scanf("%[^\n]%*c", file_name); 
	FILE *signatures = fopen(file_name, "r"); 
	link *dummy = malloc(sizeof(link)); 
	dummy->nextVirus = NULL;
	link *curr = malloc(sizeof(link));
	curr->vir = malloc(sizeof(virus)); 
	curr->vir->sig = malloc(300); 

	while(readVirus(curr->vir, signatures) > 0) { 
		dummy = list_append(dummy, curr);
		curr = malloc(sizeof(link)); 
		curr->vir = malloc(sizeof(virus));
		curr->vir->sig = malloc(300);	
	}
	list_free(curr);
	free(file_name);
	link *ret = dummy->nextVirus;
	free(dummy);
	fclose(signatures);
	return ret;
}

link* printSignatures(link* virus_list) {
	if (virus_list != NULL) {
		list_print(virus_list, stdout);
	}
	return virus_list;
}

link* quit(link* virus_list) {
	list_free(virus_list);
	exit(0);
	return virus_list;
}

link* detect(link *virus_list) {
	printf("enter file path: ");
	char *file_path = malloc(300);
	scanf("%[^\n]%*c", file_path); 
	FILE *file = fopen(file_path, "r");
	char *buffer = malloc(10000);
	size_t size = fread(buffer, 1, 10000, file);
	detect_virus(buffer, size, virus_list);
	fclose(file);
	free(buffer);
	free(file_path);
	return virus_list;
}
	
void detect_virus(char *buffer, unsigned int size, link *virus_list) {
	link *curr = virus_list;
	while(curr != NULL) {
		for(int i=0; i < size; i++) {
			if (0 == memcmp(buffer+i,curr->vir->sig, curr->vir->SigSize)) {
				printf("Located virus at byte number %d\nvirus name: %s\nvirus signature length: %d\n", i, curr->vir->virusName, curr->vir->SigSize);
				return; //unseen assignment
			}
		}
		curr = curr->nextVirus;
	}
}

link* fixFile(link* null) {
	printf("enter starting byte location: ");
	char *startingByte = malloc(300);
	scanf("%[^\n]%*c", startingByte); 
	int signatureOffset = atoi(startingByte);
	printf("enter size of virus signature: ");
	char *sigSize = malloc(300);
	scanf("%[^\n]%*c", sigSize); 
	int signatureSize = atoi(sigSize);
	printf("enter file path: ");
	char *file_path = malloc(300);
	scanf("%[^\n]%*c", file_path);
	kill_virus(file_path, signatureOffset, signatureSize);
	free(sigSize);
	free(startingByte);
	return null;
}
	
	
void kill_virus(char *fileName, int signatureOffset, int signatureSize) {
	FILE *file = fopen(fileName, "r+");
	fseek(file, signatureOffset, SEEK_SET);
	char *nopArr = malloc(signatureSize);
	for (int i=0; i < signatureSize; i++) {
		nopArr[i] = 0x90;
	}
	fwrite(nopArr, 1, signatureSize, file);
	free(nopArr);
	free(fileName);
	fclose(file);
}
	

int main(int argc, char **argv) {
	link *virus_list = NULL;

	while(1) {
		printf("Please choose a function:\n");
		int length = sizeof(menu)/sizeof(struct fun_desc);
		for (int i = 0; i < length-1; i++) {
			printf("%d) %s\n", i+1, menu[i].name);
		}
		printf("Option: ");
		int option = fgetc(stdin) - '0' -1;
		fgetc(stdin);
		if (0 <= option && option <= length-1) printf("Within bounds\n");
		else { printf("not within bounds\n"); exit(0); }
		virus_list = menu[option].fun(virus_list);
		printf("\n");
	}

/*
	FILE *signatures = fopen("signatures", "r");
	virus *vir = malloc(sizeof(virus));
	vir->sig = malloc(300);
	while(readVirus(vir, signatures) > 0) {
		printVirus(vir, stdout);
	}
*/
	return 0;
}


