/**
 * \author Yusuf Hussein
 */

#ifndef CONNMGR_H
#define CONNMGR_H


/**
 * Listens for data from a sensor_node on the socket defined by the clt socket descriptor.
 * @param args a void * array with 2 elements:\n
 * 1. client: socket descriptor created by tcp_wait_for_connection(2)\n
 * 2. buf: an sbuffer_t* to the buffer where the data
 * @return NULL
 */
void *listen_to_client(void *args);

/**
 * Starts a server that waits for connections from clients and starts a listen_to_client(1) thread for each client.
 * @param args a void * array with 3 elements:\n
 * 1. MAX_CONN: an integer determining the maximum number of client connections\n
 * 2. PORT: an integer determining the port the server will listen on\n
 * 3. buf: an sbuffer_t* to the buffer where the data
 * from the clients is inserted into
 * @return NULL
 */
void *await_clients(void *args);

#endif //OSFINALPROJECT_CONNMGR_H
