#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char censor(char c);
char* map(char *array, int array_length, char (*f) (char));
char encrypt(char c);
char decrypt(char c);
char dprt(char c);
char cprt(char c);
char my_get(char c);
char quit(char c);

struct fun_desc {
  char *name;
  char (*fun)(char);
};

struct fun_desc menu[] = { {"Censor", censor}, { "Encrypt", encrypt }, { "Decrypt", decrypt }, {"Print dec", dprt }, { "Print string", cprt }, { "Get string", my_get}, {"Quit", quit }, { NULL, NULL } };

char censor(char c) {
  if(c == '!')
    return '*';
  else
    return c;
}
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for ( int i = 0; i < array_length; i++)
  {
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}

char encrypt(char c) {
	if ('A' <= c && c <= 'z') return c+2;
	else return c;
}

char decrypt(char c) {
	if ('A' <= c && c <= 'z') return c-2;
	else return c;
}

char dprt(char c) {
	printf("%d\n", c);
	return c;
}

char cprt(char c) {
        if ('A' <= c && c <= 'z') printf("%c\n", c);
	else printf("*\n");
	return c;
}

char my_get(char c) {
	return fgetc(stdin); 
}

char quit(char c) {
	if (c == 'q') exit(0);
	else return c;
}


 
int main(int argc, char **argv){
  char carray[5];
  carray[0] = '\0';
  printf("carray: %s\n", carray);
  
  while(1) {
	printf("Please choose a function:\n");
	int length = sizeof(menu)/sizeof(struct fun_desc);
	for (int i = 0; i < length-1; i++) {
		printf("%d) %s\n", i, menu[i].name);
	}
	printf("Option: ");
	int option = fgetc(stdin) - '0';
	fgetc(stdin);
	if (0 <= option && option <= length-1) printf("Within bounds\n");
	else { printf("not within bounds\n"); exit(0); }

	char *new = map(carray, 5, menu[option].fun);
	for (int i = 0; i < 5; i++) {
		carray[i] = new[i];
	}
	free(new);
	printf("Done.\n\n");
  }

}
