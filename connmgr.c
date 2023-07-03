/**
 * \author Chrysovalantis Psilos
 */

 #define _GNU__SOURCE

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include "config.h"
 #include "lib/tcpsock.h"
 #include "connmgr.h"
 #include "sbuffer.h"
 #include "sensor_db.h"
 
 struct arg_struct{
 	tcpsock_t* client;
 	sbuffer_t* buffer;
 	char* nameoffile;
 }*args ;
 
 int inserted_data_counter=0;
 
 void connectionManager(int server_port,sbuffer_t* sbuffer, char* filename){
 	sensor_data_t* data = malloc(sizeof(sensor_data_t));
 	tcpsock_t *server;
 	pthread_t threads[MAX_CONN];
 	printf("Server is ready! It will handle up to %d clients\n",MAX_CONN);
 	tcp_passive_open(&server, server_port);
 	args = malloc(sizeof(struct arg_struct));
 	args->buffer = sbuffer;
 	args->nameoffile = filename;
 	for(int i=0; i<MAX_CONN; i++){
 		tcp_wait_for_connection(server,&args->client);
 		pthread_create(&threads[i],NULL,runnerForMultiThreadedServer,(void*)args);
 	}
 	for(int i=0; i<MAX_CONN; i++){
 		pthread_join(threads[i],NULL);
 	}
 	printf("Server reached maximum sensors. Now it will close\n");
 	/*Before the server closes, the end of buffer (sensor with id=0 is inserted.)*/
 	data->id =0;
 	data->value=0;
 	data->ts=0;
 	sbuffer_insert(args->buffer,data);
 	inserted_data_counter++;
 	
 	if(tcp_close(&server)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
 	free(args);
 	free(data);
 }
 
 void* runnerForMultiThreadedServer(void*arguments){
 	pthread_mutex_t mutex;
 	pthread_mutex_init(&mutex,NULL);
 	struct arg_struct *thread_args = arguments;
	tcpsock_t* clientdummy = thread_args->client;
	sbuffer_t* bufferdummy = thread_args->buffer;
	char* dummychar =  thread_args->nameoffile;
 	sensor_data_t* data, *logdata;
 	data = malloc(sizeof(sensor_data_t));
 	logdata = malloc(sizeof(sensor_data_t));
 	int bytes,dummybytes;
 	int result=0, dummyresult=0;
 	FILE* log = fopen(dummychar,"a");

 	if(dummyresult ==TCP_NO_ERROR){
 		dummybytes = sizeof(logdata->id);
 		dummyresult = tcp_receive(clientdummy,(void*)&logdata->id,&dummybytes);
 		dummybytes = sizeof(logdata->value);
 		dummyresult = tcp_receive(clientdummy,(void*)&logdata->value,&dummybytes);
 		dummybytes = sizeof(logdata->ts);
 		dummyresult = tcp_receive(clientdummy,(void*)&logdata->ts,&dummybytes);
 		fprintf(log,"At timestamp %d, Sensor %hu has opened a new connection\n",getTimeStamp(),logdata->id);
 		printf("SensorID: %hu Value: %g Timestamp: %ld\n",logdata->id,logdata->value,(long int)logdata->ts);
 		//sbuffer_insert(bufferdummy,logdata);
 		pthread_mutex_lock(&mutex);
 		sbuffer_insert(bufferdummy,logdata);
 		inserted_data_counter++;
 		pthread_mutex_unlock(&mutex);
 		fclose(log);
 		free(logdata);
 	}
 	while(result == TCP_NO_ERROR){
 		bytes = sizeof(data->id);
 		result = tcp_receive(clientdummy,(void*)&data->id,&bytes);
 		bytes = sizeof(data->value);
 		result = tcp_receive(clientdummy,(void*)&data->value,&bytes);
 		bytes = sizeof(data->ts);
 		result = tcp_receive(clientdummy,(void*)&data->ts,&bytes);
 		if((result==TCP_NO_ERROR)&&bytes){
 			printf("SensorID: %hu Value: %g Timestamp: %ld\n",data->id,data->value,(long int)data->ts);
 			pthread_mutex_lock(&mutex);
 			sbuffer_insert(bufferdummy,data);
 			inserted_data_counter++;
 			pthread_mutex_unlock(&mutex);
 			//sbuffer_insert(bufferdummy,data);
 		}
 	}
 	if(result == TCP_CONNECTION_CLOSED){
 		FILE* f =fopen(dummychar,"a");
 		fprintf(f,"At timestamp %d, Sensor %hu has closed the connection\n",getTimeStamp(),data->id);
 		fclose(f);
 		tcp_close(&clientdummy);
 	}
 	pthread_mutex_destroy(&mutex);
 	free(data);
 	return NULL;
 }

 void print_help(void){
 	printf("Use this program with 1 command line option: \n");
 	printf("\t%-15s: A TCP server port number.\n","\'Server port\'");
 }
 
 
