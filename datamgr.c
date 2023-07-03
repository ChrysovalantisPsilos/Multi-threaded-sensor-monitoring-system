#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include "sbuffer.h"
#include "config.h"
#include "connmgr.h"
#include "sensor_db.h"

#define NO_ERROR 0
#define MEM_ALLOC_ERROR -1 //memory alocation error
#define INVALID_ERROR -2 //sensor not found error

typedef struct sensor{
	uint16_t sensorID;
	uint16_t roomID;
	double running_avg;
	time_t timestamp;
}sensor_t;

dplist_t* list;

void* element_copy(void*);
void element_free(void**);
int element_compare(void*,void*);


int size=0;
dplist_t* list;
sensor_data_t* Data;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t* buffer, int count, char* filename, sbuffer_t* shadowbuffer){
	uint16_t sensorIDdummy, roomIDdummy;
	time_t timedummy=0;
	double tempdummy=0, dummyArray15[count], dummyArray21[count], dummyArray37[count], dummyArray49[count],dummyArray112[count],dummyArray129[count],dummyArray132[count],dummyArray142[count];
	list = dpl_create(element_copy,element_free,element_compare);
	if(fp_sensor_map !=NULL){
		sensor_t* sensor = malloc(sizeof(sensor_t*));
		while(!feof(fp_sensor_map)){
			fscanf(fp_sensor_map, "%hu %hu\n", &roomIDdummy,&sensorIDdummy);
			sensor->sensorID = sensorIDdummy;
			sensor->roomID = roomIDdummy;
		}
		free(sensor);
	}else{
		fprintf(stderr,"Error while opening file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(buffer!=NULL){
		int count15=0, count21=0, count37=0, count49=0, count112=0, count129=0, count132=0, count142=0;
		Data = malloc(sizeof(sensor_data_t));
		FILE* f=fopen(filename,"a");
		sensor_t* dummy15 = malloc(sizeof(sensor_t*));
		sensor_t* dummy21 = malloc(sizeof(sensor_t*));
		sensor_t* dummy37 = malloc(sizeof(sensor_t*));
		sensor_t* dummy49 = malloc(sizeof(sensor_t*));
		sensor_t* dummy112 = malloc(sizeof(sensor_t*));
		sensor_t* dummy129 = malloc(sizeof(sensor_t*));
		sensor_t* dummy132 = malloc(sizeof(sensor_t*));
		sensor_t* dummy142 = malloc(sizeof(sensor_t*));
		double temp15,temp21, temp37, temp49, temp112, temp129, temp132, temp142;
		double sum15,sum21, sum37, sum49, sum112, sum129, sum132, sum142;
		for(int i=0; i<count; i++){
			sbuffer_getData(buffer,Data);
			sbuffer_insert(shadowbuffer,Data);
			sensorIDdummy = Data->id;
			tempdummy = Data -> value;
			timedummy = Data->ts;
			
			if(sensorIDdummy==15){
				dummy15->sensorID=sensorIDdummy;
				dummy15->roomID = 1;
				dummy15->timestamp = timedummy;
				dummyArray15[count15] = tempdummy;
				count15++;
			}
			 if(sensorIDdummy==21){
				dummy21->sensorID = sensorIDdummy;
				dummy21->roomID = 2;
				dummy21->timestamp = timedummy;
				dummyArray21[count21] = tempdummy;
				count21++;
			}

			 if(sensorIDdummy==37){
				dummy37->sensorID = sensorIDdummy;
				dummy37->roomID = 3;
				dummy37->timestamp = timedummy;
				dummyArray37[count37] = tempdummy;
				count37++;
			}

			 if(sensorIDdummy==49){
				dummy49->sensorID = sensorIDdummy;
				dummy49->roomID = 4;
				dummy49->timestamp = timedummy;
				dummyArray49[count49] = tempdummy;
				count49++;
			}

			 if(sensorIDdummy==112){
				dummy112->sensorID = sensorIDdummy;
				dummy112->roomID = 11;
				dummy112->timestamp = timedummy;
				dummyArray112[count112] = tempdummy;
				count112++;
			}

			 if(sensorIDdummy==129){
				dummy129->sensorID = sensorIDdummy;
				dummy129->roomID = 12;
				dummy129->timestamp = timedummy;
				dummyArray129[count129] = tempdummy;
				count129++;
			}

			 if(sensorIDdummy==132){
				dummy132->sensorID = sensorIDdummy;
				dummy132->roomID = 13;
				dummy132->timestamp = timedummy;
				dummyArray132[count132] = tempdummy;
				count132++;
			}

			 if(sensorIDdummy==142){
				dummy142->sensorID = sensorIDdummy;
				dummy142->roomID = 14;
				dummy142->timestamp = timedummy;
				dummyArray142[count142] = tempdummy;
				count142++;
			}
		
			if(sensorIDdummy!=15 && sensorIDdummy !=21 && sensorIDdummy!=37 && sensorIDdummy!=49 &&sensorIDdummy!=112 &&sensorIDdummy!=129 &&sensorIDdummy!=132 &&sensorIDdummy!=142){
				fprintf(f,"At timestamp %d, Received sensor data with invalid sensor node id %hu\n",getTimeStamp(),sensorIDdummy);
			}

		}
			if(count15>0){
				for(int i=count15-RUN_AVG_LENGTH; i<count15;i++){
					sum15+=dummyArray15[i];
				}
				temp15 = sum15/RUN_AVG_LENGTH;
				dummy15->running_avg = temp15;
				dpl_insert_at_index(list,dummy15,100,false);

				if(dummy15->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(),dummy15->sensorID, dummy15->running_avg);
				}
				
				if(dummy15->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy15->sensorID, dummy15->running_avg);
				}
			}

			if(count21>0){
				for(int i=count21-RUN_AVG_LENGTH; i<count21;i++){
					sum21+=dummyArray21[i];
				}
				temp21 = sum21/RUN_AVG_LENGTH;
				dummy21->running_avg = temp21;
				dpl_insert_at_index(list,dummy21,100,false);
				if(dummy21->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(),dummy21->sensorID, dummy21->running_avg);
				}
							
				if(dummy21->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy21->sensorID, dummy21->running_avg);
				}
			}

			if(count37>0){
				for(int i=count37-RUN_AVG_LENGTH; i<count37;i++){
					sum37+=dummyArray37[i];
				}
				temp37 = sum37/RUN_AVG_LENGTH;
				dummy37->running_avg = temp37;
				dpl_insert_at_index(list,dummy37,100,false);

				if(dummy37->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy37->sensorID, dummy37->running_avg);
				}
									
				if(dummy37->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy37->sensorID, dummy37->running_avg);
				}
			}

			if(count49>0){
				for(int i=count49-RUN_AVG_LENGTH; i<count49;i++){
					sum49+=dummyArray49[i];
				}
				temp49 = sum49/RUN_AVG_LENGTH;
				dummy49->running_avg = temp49;
				dpl_insert_at_index(list,dummy49,100,false);
				if(dummy49->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy49->sensorID, dummy49->running_avg);
				}
									
				if(dummy49->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy49->sensorID, dummy49->running_avg);
				}
				
			}
			
			if(count112>0){
				for(int i=count112-RUN_AVG_LENGTH; i<count112;i++){
					sum112+=dummyArray112[i];
				}
				temp112 = sum112/RUN_AVG_LENGTH;
				dummy112->running_avg = temp112;
				dpl_insert_at_index(list,dummy112,100,false);
				if(dummy112->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy112->sensorID, dummy112->running_avg);
				}
												
				if(dummy112->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy112->sensorID, dummy112->running_avg);
				}			
			}

			if(count129>0){
				for(int i=count129-RUN_AVG_LENGTH; i<count129;i++){
					sum129+=dummyArray129[i];
				}
				temp129 = sum129/RUN_AVG_LENGTH;
				dummy129->running_avg = temp129;
				dpl_insert_at_index(list,dummy129,100,false);
				if(dummy129->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy129->sensorID, dummy129->running_avg);
				}
														
				if(dummy129->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy129->sensorID, dummy129->running_avg);
				}
			}

			if(count132>0){
				for(int i=count132-RUN_AVG_LENGTH; i<count132;i++){
					sum132+=dummyArray132[i];
				}
				temp132 = sum132/RUN_AVG_LENGTH;
				dummy132->running_avg = temp132;
				dpl_insert_at_index(list,dummy132,100,false);
				if(dummy132->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy132->sensorID, dummy132->running_avg);
				}
																	
				if(dummy132->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy132->sensorID, dummy132->running_avg);
				}
			}

			if(count142>0){
				for(int i=count142-RUN_AVG_LENGTH; i<count142;i++){
					sum142+=dummyArray142[i];
				}
				temp142 = sum142/RUN_AVG_LENGTH;
				dummy142->running_avg = temp142;
				dpl_insert_at_index(list,dummy142,100,false);
				if(dummy142->running_avg < SET_MIN_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too cold %g\n",getTimeStamp(), dummy142->sensorID, dummy142->running_avg);
				}
																	
				if(dummy142->running_avg > SET_MAX_TEMP){
					fprintf(f,"At timestamp %d, Sensor node %hu reports that it's too hot %g\n",getTimeStamp(), dummy142->sensorID, dummy142->running_avg);
				}
			}
	free(dummy15);
	free(dummy21);
	free(dummy37);
	free(dummy49);
	free(dummy112);
	free(dummy129);
	free(dummy132);
	free(dummy142);
	free(Data);
	fclose(f);
	}else{
		fprintf(stderr,"Error reading buffer: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
}


void datamgr_free(){
	dpl_free(&list,true);
}

//taken from the file dplist_test from milestone1 and adjusted approprietly. Here, the element is the sensor!
void* element_copy(void* element){
	sensor_t* copy = malloc(sizeof(sensor_t));
    *copy = *(sensor_t*)element;
    return (void*)copy;
}

void element_free(void** element){
	sensor_t* dummyelement = *element;
    free(dummyelement);
}

int element_compare(void* x, void* y){
	return ((((sensor_t*)x)->sensorID < ((sensor_t*)y)->sensorID) ? -1 : (((sensor_t*)x)->sensorID == ((sensor_t*)y)->sensorID) ? 0 : 1);
}




