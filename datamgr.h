/**
 * \author Chrysovalantis Psilos
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include "config.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
	#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
	#error SET_MIN_TEMP not set
#endif

/**
 * This method holds the core functionality of the datamgr. It takes the data coming from the buffer and prints to the gateway.log file
 * whether its too hot/cold in a room or if the sensor node id is invalid.
 * \param 'fp_sensor_map', file pointer to the map file.
 * \param 'buffer', the buffer from which the data is retrieved
 * \param 'filename', the name of the file where the log will be temporarily stored.
 * \param 'shadowbuffer', a buffer that is connected in series with the first buffer, forming a large buffer.
 */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t *buffer, int count, char* filename, sbuffer_t* shadowbuffer);

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id);

/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id);

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors();

#endif  //DATAMGR_H_
