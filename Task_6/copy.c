#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/mman.h> 
#include <fcntl.h> 

#define SIZEBUFFER 2048

void copy_mmap(char* fd_from, char* fd_to);
void copy_read_write(char* fd_from, char* fd_to);


int main(int totArg, char *argv[])
{
	if (totArg >= 2)
	{
		if (strcmp(argv[1], "-m") == 0 && totArg == 4) // ./copy.out -m file copyfile
		{
			copy_mmap(argv[2], argv[3]);
			return 0;
		}

		if (strcmp(argv[1], "-m") != 0 && strcmp(argv[1], "-h") != 0 && totArg == 3) // ./copy.out fil copyfile
		{
			copy_read_write(argv[1], argv[2]);
			return 0;
		}
	}

	//Wrong call of the program or ./copy.out -h
	printf("Description of possible case:\n");
	printf("\t./copy.out [-m] <source> <destination>\n");
	printf("\t./copy.out <source> <destination>\n");
	printf("\t./copy.out [-h]\n");
	printf("\t\t-m : copy the file with memcpy()\n");
	printf("\t\tjust with files' name: copy the file with read() and write()\n");
	printf("\t\t-h : display this help\n");

	return 0;
}


void copy_mmap(char* fd_from, char* fd_to)
{
	int source, destination;
    char *sourcePtr, *destinationPtr;
    size_t sourceFileSize = 0;

    source = open(fd_from, O_RDONLY); //Open source file in read only mod

    sourceFileSize = lseek(source, 0, SEEK_END); //Compute the size of the source file

    sourcePtr = mmap(NULL, sourceFileSize, PROT_READ, MAP_PRIVATE, source, 0); //Copy source file in the virtual memory -- we can read the file in the memory

    destination = open(fd_to, O_CREAT | O_RDWR, 0666); //Create a destination file in read/write mod + add rights 0666 

    ftruncate(destination, sourceFileSize); //Set the size of the destination file equal to the size of the source file

    destinationPtr = mmap(NULL, sourceFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, destination, 0); ///Copy destination file in the virtual memory -- we can read and write the file in the memory

    memcpy(destinationPtr, sourcePtr, sourceFileSize); //Copy sourceFileSize bytes from sourcePtr memory to destinationPtr memory

    // Delete allocated memory of sourcePtr and destinationPtr
    munmap(sourcePtr, sourceFileSize);
    munmap(destinationPtr, sourceFileSize);

    printf("\"%s\" was successfully copied in \"%s\". \n", fd_from, fd_to);

    close(source);
    close(destination);
}


void copy_read_write(char* fd_from, char* fd_to)
{
    char buffer[SIZEBUFFER];
    int sizefile, source, destination; 

    source = open(fd_from, O_RDONLY); //Open source file in read only mod
    
    destination = open(fd_to, O_CREAT | O_WRONLY, 0666); //Create a destination file in write only mod  + add rights 0666 
    
    sizefile = read(source, buffer, SIZEBUFFER); //Read the source file until SIZEBUFFER bytes in the buffer

    write(destination, buffer, sizefile); //Write in the destination file the buffer until sizefile bytes

    printf("\"%s\" was successfully copied in \"%s\". \n", fd_from, fd_to);

    close(source);
    close(destination);
}