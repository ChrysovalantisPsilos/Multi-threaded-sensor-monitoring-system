#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <stdbool.h>
#include "sbuffer.h"

#define ERROR_HANDLER(condition,... )    do {                       \
                      if (condition) {                              \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE);                         \
                      }                                             \
                    } while(0)

#define SENSOR_DB_SUCCESS 0 
#define SENSOR_DB_FAILURE -1 

/**
 * This function can open a given file in write mode or append mode.  
 * \param 'filename' the name of the file we want to open.
 * \param 'appen' a boolean indicating whether we should append the file or overwrite it.
 * If append--> true file will be appended. Else, it will be overwritten.
 * \return a file pointer which points to the file we have just opened. 
 */
FILE * open_db(char*filename,bool append);

/**
 * This function defines the core functionality of the storage manager thread. 
 * It takes sensor data in the form <SensorID><Value><Timestamp> and 
 * inserts them to a file called data.csv. 
 * \param 'filename' the name of the file where the data should be saved.
 * \param 'buffer' a buffer from which the data is read.
 * \param 'count' an int count which is used to loop through the buffer.
 * In this case, we use the inserted_data_counter provided from connmgr.h
 */
void LogicForStorageManager(char* filename, sbuffer_t* buffer, int count);


/**
 * This function is used to insert sensor data to a specified file.
 * \param 'file_pointer' a pointer to the file in which the data should be inserted.
 * \param 'id' the id of the sensor.
 * \param 'value' the value of the sensor.
 * \param 'ts' the timestamp of the sensor.
 * \return SENSOR_DB_SUCCESS if successful.
 * \return SENSOR_DB_FAILURE if unsuccessful. 
 */
int insert_sensor(FILE* file_pointer, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/**
 * This is a simple function used to close a given file.
 * \param file_pointer a pointer to the file we want to close.
 * \return SENSOR_DB_SUCCESS if successful.
 * \return SENSOR_DB_FAILURE if unsuccessful.
 */
int close_db(FILE* file_pointer); 


/**
 * A very simple function used to get the timestamp.
 * \return the current timestamp in integer form.
 */
int getTimeStamp();



#endif //_SENSOR_DB_H_
