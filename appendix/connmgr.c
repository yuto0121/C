/**
 * \author Yusuf Hussein
 */


#include <pthread.h>
#include "config.h"
#include "lib/tcpsock.h"
#include "sbuffer.h"
#include "connmgr.h"


void *listen_to_client(void *args) {
    params_t *params = args;
    tcpsock_t *client = params->param1;
    sbuffer_t *buf = params->param2;
    free(params);

    sensor_data_t data;
    int bytes, result;

    bool first = true;
    sensor_id_t id = 0;

    do {

        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive_timeout(client, (void *) &data.id, &bytes, TIMEOUT);
        if(result != TCP_NO_ERROR) break;

        // log connection for first packet
        if(first && (bytes == sizeof(data.id))) {
            id = data.id;
            char con_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
            sprintf(con_msg, "Sensor node %hu has opened a new connection.", id);
            write_to_log_process(con_msg);
            first = false;
        }

        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive_timeout(client, (void *) &data.value, &bytes, TIMEOUT);
        if(result != TCP_NO_ERROR) break;


        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive_timeout(client, (void *) &data.ts, &bytes, TIMEOUT);
        if(result != TCP_NO_ERROR) break;


        if (bytes == sizeof(data.ts)) {
            char ins_buf_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
            sprintf(ins_buf_msg, "Sensor node %hu Value: %f inserted to buffer.", data.id, data.value);
            write_to_log_process(ins_buf_msg);
            sbuffer_insert(buf, &data);
        }

    } while (true);

    if(id == 0) {
        write_to_log_process("Client failed to send data (tcpsock failed).");
        exit(EXIT_FAILURE);
    }

    char dc_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
    if(result == TCP_TIMED_OUT)
        sprintf(dc_msg, "Sensor node %hu has timed out. Connection terminated.", id);
    else
        sprintf(dc_msg, "Sensor node %hu has closed the connection.", id);
    write_to_log_process(dc_msg);

    tcp_close(&client);
    write_to_log_process("A connection was terminated successfully.");

    return NULL;
}

void *await_clients(void *args) {
    params_t *params_in = args;
    int MAX_CONN = *(int *)params_in->param1;
    int PORT = *(int *)params_in->param2;
    sbuffer_t *buf = params_in->param3;

    // list of threads
    pthread_t threads[MAX_CONN];

    // server code
    write_to_log_process("Server booting up.");
    tcpsock_t *server;

    // boot server and log failure
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR){
        write_to_log_process("Server failed to boot.");
        exit(EXIT_FAILURE);
    }
    else {
        // wait for client connections
        for (int i = 0; i < MAX_CONN; i++) {
            tcpsock_t *client;
            if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) {
                write_to_log_process("Failed to connect to any clients.");
                exit(EXIT_FAILURE);
            } else {
                // debug
                char clt_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
                sprintf(clt_msg, "Client connecting...");
                write_to_log_process(clt_msg);

                // start client threads
                params_t *params = malloc(sizeof(params_t));
                params->param1 = client;
                params->param2 = buf;
                pthread_create(&threads[i], NULL, &listen_to_client, params);
            }

        }
        for (int i = 0; i < MAX_CONN; i++) {
            // await threads
            pthread_join(threads[i], NULL);
        }
    }
    // insert termination entry
    sensor_data_t terminator = {0, 0, 0};
    sbuffer_insert(buf, &terminator);

    if (tcp_close(&server) != TCP_NO_ERROR) {
        write_to_log_process("Server failed to shut down.");
        exit(EXIT_FAILURE);
    }
    write_to_log_process("Server shutdown.");
    return NULL;
}










