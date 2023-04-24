#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
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
  /* TODO: Test your code */

  //char arr1[] = {'H','e','y','!'};
  //char* arr2 = map(arr1, 4, censor);
  //printf("%s\n", arr2); 
  //free(arr2);
  //printf("%c\n", encrypt('a'));

  int base_len = 5;

  char arr1[base_len];
  char* arr2 = map(arr1, base_len, my_get);
  char* arr3 = map(arr2, base_len, encrypt);
  char* arr4 = map(arr3, base_len, dprt);
  char* arr5 = map(arr4, base_len, decrypt);
  char* arr6 = map(arr5, base_len, cprt);
  //char* arr7 = map(arr2, base_len, quit);
  free(arr2);
  free(arr3);
  free(arr4);
  free(arr5);
  free(arr6);
  char c = quit(fgetc(stdin));
  char t = quit(fgetc(stdin));
}
