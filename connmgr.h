/**
 * \author Chrysovalantis Psilos
 */

#ifndef __CONNMGR_H__
#define __CONNMGR_H__

#include <stdio.h>
#include "config.h"
#include "sbuffer.h"


#ifndef TIMEOUT
	#error TIMEOUT not specified (in seconds)
#endif

#ifndef MAX_CONN
	#define MAX_CONN 3
#endif
extern int inserted_data_counter;

/**
 * This function contains the main logic of the connection manager.
 * \param 'server_port' the port where the server will listen for 
 * incoming connections. It is given as a command line argument.
 * \param 'sbuffer' the buffer where the data will be saved.
 * \param 'filename' the name of the file where the logs will be saved temporarily
 */
void connectionManager(int server_port, sbuffer_t* sbuffer, char* filename);

/**
 * This function is used by a thread to handle the connecting client (sensor).
 * Internally, the function will save the 
 * If TCP_NO_ERROR then the data are printed to the terminal screen.
 * If TCP_CONNECTION_CLOSED then the client has closed the connection and 
 * the thread exits.
 * \param 'arguments' a structure that is passed to the thread function
 * \return NULL 
 */
void* runnerForMultiThreadedServer(void*arguments);

/**
 * This function is used to provide instructions on how to use this program.
 */
void print_help(void);

#endif 
