#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include "config.h"
#include "sbuffer.h"
#include "sensor_db.h"

FILE * open_db(char* filename,bool append){
	FILE* fileread; 
	fileread = fopen(filename,"a"); 
	if(fileread!=NULL){//file exists-->open
		if(append==true){//file should not be overwritten!
			fclose(fileread); //close the file as readable
			FILE* fileappend;
			fileappend = fopen(filename,"a"); //open it in append mode
			return fileappend; //return the file
		}else{ //file should be overwritten!
			fclose(fileread); //close file as readable
			FILE* fileoverwrite;
			fileoverwrite = fopen(filename,"w");
			return fileoverwrite; //return the file
		}
	}else{
		fprintf(stderr,"Error while trying to open the file: %s\n",strerror(errno));
		return NULL;
	}
}

void LogicForStorageManager(char* filename, sbuffer_t* buffer, int count){
	sensor_data_t* data = malloc(sizeof(sensor_data_t));
	FILE * log_process = fopen("BIRD.txt","a"); //log process
	FILE* data_csv = open_db(filename,false); //save data to a file
	if(data_csv==NULL && log_process==NULL){
		fprintf(stderr,"Error opening the file(s): %s\n",strerror(errno));
		free(data);
	}else{
		fprintf(log_process,"At timestamp %d, a new data.csv file has been created.\n", getTimeStamp());
		for(int i=0; i<count; i++){
			sbuffer_remove(buffer,data);
			if(data->id!=0){
				insert_sensor(data_csv,data->id,data->value,(long int)data->ts);
				fprintf(log_process,"At timestamp %d, Data insertion from sensor %hu secceeded.\n",getTimeStamp(),data->id);

			}else{
				fprintf(log_process,"At timestamp %d, the data.csv file has been closed.\n", getTimeStamp());
				fclose(log_process);
				close_db(data_csv);
				free(data);
			}
		}
	}
}
				
int insert_sensor(FILE *file_pointer, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	if(fprintf(file_pointer,"%hu, %g, %ld\n", id,value,ts)<0){ //fprintf return a negative value when an error occurs
		fprintf(stderr,"Error writing the data to the data.csv: %s\n", strerror(errno));
		return SENSOR_DB_FAILURE; 
	}else{
		return SENSOR_DB_SUCCESS;
	}
}

int close_db(FILE* file_pointer){
	if(fclose(file_pointer)!=0){ //fclose returns non-zero value when an error occurs
		fprintf(stderr,"Error trying to close the file data.csv: %s\n",strerror(errno));
		return SENSOR_DB_FAILURE; 
	}else{
		return SENSOR_DB_SUCCESS; 
	}
}

int getTimeStamp(){
	return (int)time(NULL);
}















	



