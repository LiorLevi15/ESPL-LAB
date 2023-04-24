#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

char censor(char c);
char* map(char *array, int array_length, char (*f) (char));
char encrypt(char c);
char decrypt(char c);
char dprt(char c);
char cprt(char c);
char my_get(char c);

void toggle_Debug_Mode();
void examine_ELF_File();
void print_Section_Names();
void print_Symbols();
void quit();

void open_File();
char* dataType(Elf32_Ehdr* header);
int isELFfile(Elf32_Ehdr* header);
Elf32_Shdr* getTable(char* _name);

int debug = 0;
int currFD = -1;
struct stat fd_stat;
void* map_start;
Elf32_Ehdr *header;




struct fun_desc {
  char *name;
  void (*fun)();
};

struct fun_desc menu[] = { {"Toggle Debug Mode", toggle_Debug_Mode}, { "Examine ELF File", examine_ELF_File }, { "Print Section Names", print_Section_Names }, {"Print Symbols", print_Symbols }, {"Quit", quit }, { NULL, NULL } };


void toggle_Debug_Mode() {
	if(debug) {
		debug = 0;
		printf("Debug flag now off\n");
	}
	else {
		debug = 1;
		printf("Debug flag now on\n");
	}
}

void examine_ELF_File() {
	printf("Enter File Name:");
	open_File();
	if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, currFD, 0)) == MAP_FAILED ) {
      perror("error on maping file");
	  currFD = -1;
      exit(-4);
	}
	header = (Elf32_Ehdr *) map_start;
	if(isELFfile(header)){
        printf("Magic:\t\t\t\t %X %X %X\n", header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2]);
        printf("Data:\t\t\t\t %s\n",dataType(header));
        printf("Enty point address:\t\t 0x%x\n",header->e_entry);
        printf("Start of section headers:\t %d (bytes into file)\n",header->e_shoff);
        printf("Number of section headers:\t %d\n",  header->e_shnum);
        printf("Size of section headers:\t %d (bytes)\n",header->e_shentsize);
        printf("Start of program headers:\t %d (bytes into file)\n",header->e_phoff);
        printf("Number of program headers:\t %d\n",header->e_phnum);
        printf("Size of program headers:\t %d (bytes)\n",header->e_phentsize);
   	}
    else {
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(currFD); 
        currFD=-1;
        //currentFilenameOpen=NULL;
    }
}

void open_File() {
	
	char buffer[1024];
	char fileName[1024];
    fgets(buffer,1024,stdin);
    strncpy(fileName,buffer,strlen(buffer)-1);
	int fd = open(fileName, O_RDWR);// O_RDWR?
	if(fd < 0) {
		perror("error on opening file\n");
		exit(-1);
	}
	if(fstat(fd, &fd_stat) != 0) {
		perror("error on quiring file stats");
		exit(-1);
	}
	if(currFD != -1) close(currFD); 
	currFD = fd;
}

char* dataType(Elf32_Ehdr* header){
    switch (header->e_ident[5]){
    case ELFDATANONE:
        return "invalid data encoding";
        break;
    case ELFDATA2LSB:
        return "2's complement, little endian";
        break;
    case ELFDATA2MSB:
        return "2's complement, big endian";
        break;
    default:
        return "NO DATA";
        break;
    }
}

int isELFfile(Elf32_Ehdr* header){
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){
        return 1;
    }
    return 0;
}

void print_Section_Names(){
    if(currFD == -1){
        printf("no File!");
        return;
    }
    int i,currNameIndex;
    int sectionsNum = header->e_shnum;
    Elf32_Shdr * sections = (Elf32_Shdr*)(map_start + header->e_shoff);
    char* stringTable = (char *)(map_start + sections[header->e_shstrndx].sh_offset);
    if (debug) printf("DEBUG---> e_shstrndx: %d \t Section name Offset: %x\n",header->e_shstrndx,header->e_shoff);
    printf("There are %d section headers, starting at offset %#x:\n\n",header->e_shnum,header->e_shoff);
    printf("Section Headers:\n  [Nr] Name\t\tAddr\t\tOff\tSize\t\tType\n");
    for (i=0 ; i<sectionsNum; i++){
        printf("[%d] ",i);
        char* name = stringTable + sections[i].sh_name;
        printf("%s\t\t",name);
        printf("%08x\t",sections[i].sh_addr);
        printf("%06x\t",sections[i].sh_offset);
        printf("%06x\t\t",sections[i].sh_size);
        printf("%010d\n",sections[i].sh_type);
    }        
}

void print_Symbols() {
    if(currFD!=-1) {
        Elf32_Shdr *symbol_table_entry = getTable(".symtab");
        Elf32_Shdr *strtab = getTable(".strtab");       
        Elf32_Shdr *shstrtab = getTable(".shstrtab");   
        if (symbol_table_entry != NULL){
            int num_of_entries = symbol_table_entry->sh_size / sizeof(Elf32_Sym);
            if(debug){
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\t\tsize\n");
            }
            else{
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\n");
            }
            for (size_t i = 0; i < num_of_entries; i++) {
                Elf32_Sym *entry = map_start + symbol_table_entry->sh_offset + (i * sizeof(Elf32_Sym));
                char *section_name;
                if (entry->st_shndx == 0xFFF1) { section_name = "ABS";}
                else if (entry->st_shndx == 0x0) { section_name = "UND";}
                else {
                    Elf32_Shdr *section_entry = map_start + header->e_shoff + (entry->st_shndx * header->e_shentsize);
                    section_name = map_start + shstrtab->sh_offset + section_entry->sh_name;
                }
                char *symbol_name = map_start + strtab->sh_offset + entry->st_name;
                if (debug){
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t%-20.30s\t\t%06d\n",
                    i, entry->st_value, entry->st_shndx, section_name,symbol_name,entry->st_size);
                }
                else {
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t%-20.30s\n",
                        i, entry->st_value, entry->st_shndx, section_name,symbol_name);
                }
            }
        }
        else{
            perror("Can't find symbol table in the file");
        }
    }
    else{perror("No file is currently open\n");}

}

Elf32_Shdr* getTable(char* _name){
    Elf32_Shdr* string_table_entry = map_start+ header->e_shoff+(header->e_shstrndx*header->e_shentsize);
    for (size_t i = 0; i < header->e_shnum; i++){
        Elf32_Shdr* entry = map_start+header->e_shoff+(i* header->e_shentsize);
        char* name = map_start + string_table_entry->sh_offset + entry->sh_name;
        if(strcmp(_name, name)==0){
            return entry;
        }
    }
    return NULL;
}

void quit() {
	if (currFD != -1) {
		close(currFD);
		munmap(map_start, fd_stat.st_size); 
	}
	exit(0); 
}



int main(int argc, char **argv){

  while(1) {
	printf("Please choose a function:\n");
	int length = sizeof(menu)/sizeof(struct fun_desc);
	for (int i = 0; i < length-1; i++) {
		printf("%d-%s\n", i, menu[i].name);
	}
	printf("Option: ");
	int option = fgetc(stdin) - '0';
	fgetc(stdin);
	if (0 <= option && option <= length-1) printf("Within bounds\n");
	else { printf("not within bounds\n"); exit(0); }
	menu[option].fun();
	printf("Done.\n\n");
  }

}
















// char censor(char c) {
//   if(c == '!')
//     return '*';
//   else
//     return c;
// }
 
// char* map(char *array, int array_length, char (*f) (char)){
//   char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
//   for ( int i = 0; i < array_length; i++)
//   {
//     mapped_array[i] = f(array[i]);
//   }
//   return mapped_array;
// }

// char encrypt(char c) {
// 	if ('A' <= c && c <= 'z') return c+2;
// 	else return c;
// }

// char decrypt(char c) {
// 	if ('A' <= c && c <= 'z') return c-2;
// 	else return c;
// }

// char dprt(char c) {
// 	printf("%d\n", c);
// 	return c;
// }

// char cprt(char c) {
//         if ('A' <= c && c <= 'z') printf("%c\n", c);
// 	else printf("*\n");
// 	return c;
// }

// char my_get(char c) {
// 	return fgetc(stdin); 
// }


 

