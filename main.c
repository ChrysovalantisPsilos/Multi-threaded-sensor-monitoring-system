#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include "sensor_db.h"
#include "sbuffer.h"
#include "datamgr.h"
#include "connmgr.h"

/*These are used for the pipe*/
#define SIZE 255 
#define READ_END 0
#define WRITE_END 1

/*Function prototypes:*/
/**
 * This function implements the multithreaded server. It waits for incoming data and then saves it 
 * to the shared buffer. The number of nodes that it can handle depends on whether the -D MAX_CONN 
 * was set or not. If it is not set, then it handle up to 4 clients at once.
 * \param 'port' the port that the server will listen to.
 * \return NULL.
 */
void* runnerPopulateBuffer(void* port);

/**
 * This function holds the core functionality of the server. It retrieves the values from the shared buffer,
 * and outputs to the logfile whether its cold/hot in a room. If a sensor with uknown id is passed, an error
 * message is sent instead.
 * \return NULL
 */
void* runnerDataManager();

/**
 * This function has to read sensor measurements from the shared buffer, and add these to a file called data.csv. 
 * A new data.csv file should be created when the server starts and it should not be deleted when it stops.
 * \return NULL
 */
void* runnerstorageManager();

/*End of function prototypes*/

/*Global variables*/
int fd[2];
sbuffer_t* sbuffer,*shadowbuffer;
pthread_mutex_t thread_mutex;
/*End of global variables*/

int main(int argc, char *argv[]){
	//Used to save the port given as a commandline argument
	int* dummyport= malloc(sizeof(int));
	//Thread stuff
	pthread_t threads[3];
	//Mutex(es) used
	pthread_mutex_init(&thread_mutex,NULL);
	//Parent-child
	pid_t pid;

	if(argc!=2){
		print_help();
		free(dummyport);
		exit(EXIT_FAILURE);
	}else{
	 	*dummyport = atoi(argv[1]);
	 	if(pipe(fd)==-1){ //Error handling in case the pipe cannot be created.
	 		fprintf(stderr,"Failed to create the pipe: %s\n",strerror(errno)); 
	 		free(dummyport);
	 		exit(EXIT_FAILURE);
		}
		pid=fork(); //fork the child
		if(pid<0){ //eError handling in case fork is not possible
			fprintf(stderr,"Failed to fork the child: %s\n",strerror(errno));
			free(dummyport);
			exit(EXIT_FAILURE);
		}

		if(pid>0){ //Parent process. We want everything to run in here and then send the logs to the child process
			sbuffer_init(&sbuffer); //init the shared buffer
			sbuffer_init(&shadowbuffer);
			if(pthread_create(&threads[0],NULL,runnerPopulateBuffer,(void*)dummyport)!=0){ //0. Populates the buffer
				fprintf(stderr,"Error creating the populate buffer thread: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			if(pthread_join(threads[0],NULL)!=0){ //Wait for the thread to finish
				fprintf(stderr,"Error waiting for thread[0]: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			if(pthread_create(&threads[1],NULL,runnerDataManager,NULL)!=0){ //1. Takes the data from the buffer and stores a message to the gateway.log file
				fprintf(stderr,"Error creating the data manager thread: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			if(pthread_join(threads[1],NULL)!=0){ //Wait for the thread to finish
				fprintf(stderr,"Error waiting for thread[1]: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			
			if(pthread_create(&threads[2],NULL,runnerstorageManager,NULL)!=0){ //2. Takes the data from the buffer and stores them to a file called data.csv
				fprintf(stderr,"Error creating the storage manager thread: %s\n",strerror(errno));
				exit(EXIT_FAILURE);
			}
			if(pthread_join(threads[2],NULL)!=0){ //Wait for the thread to finish.
				fprintf(stderr,"Error waiting for thread[2]: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			close(fd[READ_END]); //The next 8 lines are used for communication with the child process
			FILE* temp = fopen("BIRD.txt","r");
			char buffer[SIZE];
			size_t bytes_read;
			while((bytes_read=fread(buffer,1, SIZE,temp))>0){
				write(fd[WRITE_END],buffer,bytes_read);
			}
			fclose(temp);
			close(fd[WRITE_END]);
			wait(NULL); //wait for child to finish
		}

		if(pid==0){ //Child process. It is used to store the log messages to the gateway.log file
			close(fd[WRITE_END]); //the next 8 lines are used to store the log messages to the gateway.log file
			char rmsg[SIZE];
			ssize_t bytes_read;
			FILE* logfile = fopen("gateway.log","w");
			while((bytes_read = read(fd[READ_END],rmsg,SIZE))>0){
				fwrite(rmsg,1,bytes_read,logfile);
			}
			close(fd[READ_END]);
			remove("BIRD.txt");
			fclose(logfile);
			exit(EXIT_SUCCESS);
			return 0;
		}
	}
	printf("Clean up phase\n");
	free(dummyport);
	sbuffer_free(&sbuffer);
	sbuffer_free(&shadowbuffer);
	pthread_mutex_destroy(&thread_mutex);
	return 0;
}

void* runnerPopulateBuffer(void* port){
	int* server_port = (int*)port;
	connectionManager(*server_port, sbuffer,"BIRD.txt");
	return NULL;	
}

void* runnerDataManager(){
	FILE* fp_sensor_map = fopen("room_sensor.map","r");
	if(fp_sensor_map!=NULL){
		datamgr_parse_sensor_files(fp_sensor_map,sbuffer,inserted_data_counter,"BIRD.txt",shadowbuffer); //inserted_data_counter is an external variable from connmgr.h
		fclose(fp_sensor_map);
		datamgr_free();
		return NULL;
	}else{
		fprintf(stderr,"Error occured while trying to open the file: %s\n",strerror(errno));
		return NULL;
	}
}

void* runnerstorageManager(){
	LogicForStorageManager("data.csv",shadowbuffer,inserted_data_counter);
	return NULL;
}






