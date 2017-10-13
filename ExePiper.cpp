#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define NUM_PIPES 2

#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1
 
int pipesA[NUM_PIPES][2];
int pipesB[NUM_PIPES][2];

#define READ_FD  0
#define WRITE_FD 1
 
#define PARENT_READ_FD_A  ( pipesA[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD_A ( pipesA[PARENT_WRITE_PIPE][WRITE_FD] )
 
#define CHILD_READ_FD_A   ( pipesA[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD_A  ( pipesA[PARENT_READ_PIPE][WRITE_FD]  )

#define PARENT_READ_FD_B  ( pipesB[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD_B ( pipesB[PARENT_WRITE_PIPE][WRITE_FD] )
 
#define CHILD_READ_FD_B   ( pipesB[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD_B  ( pipesB[PARENT_READ_PIPE][WRITE_FD]  )

void closeFDs() {
	close(CHILD_READ_FD_A);
    close(CHILD_WRITE_FD_A);
    close(PARENT_READ_FD_A);
    close(PARENT_WRITE_FD_A);
    close(CHILD_READ_FD_B);
    close(CHILD_WRITE_FD_B);
    close(PARENT_READ_FD_B);
    close(PARENT_WRITE_FD_B);
}

// size of relevant char in buffer. Includes the null terminator
int sizeofBuffer(char buffer[100]) {
	int size = 0;
	while (buffer[size]) {
		size++;
	}
	return size+1;
}

// assumed input to child process through inputBuffer and output through outputBuffer
void getChildResponse(
	int parentWrite, 
	int parentRead, 
	char* inputBuffer, 
	int inputBytes, 
	char* outputBuffer, 
	int* outputBytes
	) {
	write(parentWrite, inputBuffer, inputBytes);

	*outputBytes = read(parentRead, outputBuffer, sizeof(outputBuffer)-1);
}

int main(int args, char **argv) {

	
	pipe(pipesA[PARENT_READ_PIPE]);
    pipe(pipesA[PARENT_WRITE_PIPE]);
    pipe(pipesB[PARENT_READ_PIPE]);
    pipe(pipesB[PARENT_WRITE_PIPE]);

	int pidA = fork();
	int pidB = fork();

	if (pidA < 0 || pidB < 0) {
		cout << "Fork failed" << endl;
	}

    if(!pidA) { // child process A
    	char A[] = {'A', '\0'}; // placeholder as using with test program
        char *argvA[]={argv[1], A, 0};
 
        dup2(CHILD_READ_FD_A, STDIN_FILENO);
        dup2(CHILD_WRITE_FD_A, STDOUT_FILENO);
 
        /* Close fds not required by child. Also, we don't
           want the exec'ed program to know these existed */
        closeFDs();
          
        execv(argvA[0], argvA);

    } else if (!pidB) { // child process B
    	char B[] = {'B', '\0'};
        char *argvB[]={argv[2], B, 0};
 
        dup2(CHILD_READ_FD_B, STDIN_FILENO);
        dup2(CHILD_WRITE_FD_B, STDOUT_FILENO);

        closeFDs();
          
        execv(argvB[0], argvB);

    } else {
        char inputBuffer[100];
        char outputBuffer[100];
        int count;
 
        /* close fds not required by parent */
        close(CHILD_READ_FD_A);
        close(CHILD_WRITE_FD_A);
        close(CHILD_READ_FD_B);
        close(CHILD_WRITE_FD_B);
 		
 		// loop for communication basically starts here. code below is placeholder mostly for testing
 		for (int i = 0; i < 5; i++) {
 			outputBuffer[0] = i+'0';
 			outputBuffer[1] = '\n';
 			outputBuffer[2] = 0;
	        
	        // Write to child’s stdin
	        write(PARENT_WRITE_FD_A, outputBuffer, sizeofBuffer(outputBuffer));

	        // Read from child’s stdout
	        count = read(PARENT_READ_FD_A, inputBuffer, sizeof(inputBuffer)-1);
	        if (count >= 0) {
	            inputBuffer[count] = 0;
	            printf("%s", inputBuffer);
	        } else {
	            printf("IO Error\n");
	        }

	        write(PARENT_WRITE_FD_B, outputBuffer, sizeofBuffer(outputBuffer));
	  
	        // Read from child’s stdout
	        count = read(PARENT_READ_FD_B, inputBuffer, sizeof(inputBuffer)-1);
	        if (count >= 0) {
	            inputBuffer[count] = 0;
	            printf("%s", inputBuffer);
	        } else {
	            printf("IO Error\n");
	        }
	    }
    }
}


