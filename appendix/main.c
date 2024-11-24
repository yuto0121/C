/**
 * \author Yusuf Hussein
 */

#include "config.h"
#include "sbuffer.h"

#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"

#include "pthread.h"

pid_t pid; // process id
bool running = false; // is logger running or not
int pPTC[2]; // parent to child pipe


pthread_mutex_t lock;
int write_to_log_process(char *msg) {
    if(!running) {
        printf("Log Error: failed to write to log process. Log process not running.\n");
        return -1;
    }

    pthread_mutex_lock(&lock);
    // only called on parent process since child process is in infinite loop
    ssize_t response_code = write(pPTC[1], msg, MSG_LENGTH); // write to pipe
    pthread_mutex_unlock(&lock);

    if(response_code == -1) {
        printf("Log Error: failed to write message '%s' to pipe.\n", msg);
        return -1;
    }

    return 0;
}


int create_log_process() {
    if(running) { // if log process is already running
        return 0;
    }

    running = true;

    // initialize pipe
    if(pipe(pPTC) == -1) {
        printf("Pipe Error: parent to child pipe creation failed.\n");
        return -1;
    }

    // create child process
    pid = fork();

    // forking failed
    if(pid < 0) {
        printf("Fork error: failed to create child process.\n");
        return -1;
    }
        // child: log process
    else if(pid == 0) {
        close(pPTC[1]); // close write end of pipe
        FILE *log_file = fopen("gateway.log", "a"); // open log file
        if(log_file == NULL) printf("File Error: failed to open log file.\n"); // error handling

        int log_counter = 0; // sequence number for log entries

        // infinite loop for child process
        while(true) {
            char msg[MSG_LENGTH];
            long rd = read(pPTC[0], msg, MSG_LENGTH); // read from pipe

            if(rd == 0) { // if nothing is read, EOF means pipe was closed, so end log process
                close(pPTC[0]); // close read end
                fclose(log_file); // close log file
                exit(0); // kill child process
            }

            // get current time formatted string needed for log entry
            time_t t = time(NULL);
            char time_str[TIME_LENGTH];
            strftime(time_str, TIME_LENGTH, "%c", localtime(&t));

            // write to log file and increment counter
            fprintf(log_file, "%d - %s - %s\n", log_counter++, time_str, msg);
            fflush(log_file); // flush the temporary buffer so data is written immediately
        }
    }
        // parent process
    else {
        pthread_mutex_init(&lock, NULL);
        close(pPTC[0]); // close read end of pipe
    }

    return 0;
}


int end_log_process() {
    if(!running) {
        printf("Log Error: failed to end log process. Log process not running.");
        return -1;
    }

    // only called by parent process since child process is in infinite loop
    int response_code = close(pPTC[1]); // close write end
    pthread_mutex_destroy(&lock);
    wait(NULL); // reap child process
    running = false; // set log process to no longer be running
    return response_code;
}

// THREADS

void *connection_manager(void *args) {
    params_t *params = args;

    // initialize connections
    int MAX_CONN = atoi(params->param1);
    int PORT = atoi(params->param2);

    params_t *params_o = malloc(sizeof(params_t));
    params_o->param1 = &MAX_CONN;
    params_o->param2 = &PORT;
    params_o->param3 = params->param3;

    pthread_t t_server;
    pthread_create(&t_server, NULL, &await_clients, params_o);

    pthread_join(t_server, NULL);

    free(params_o);

    return NULL;
}

void *data_manager(void *args) {
    sbuffer_t *buf = ((sbuffer_t *)args);
    FILE *map = fopen("room_sensor.map", "r");

    datamgr_parse_sensor_files(map, buf);

    fclose(map);
    datamgr_free();

    return NULL;
}

void *storage_manager(void *args) {
    sbuffer_t *buf = ((sbuffer_t *)args);
    FILE * f = open_db("data.csv", false);

    sensor_data_t *data = malloc(sizeof(sensor_data_t));

    while(sbuffer_remove(buf, data, 0) != SBUFFER_NO_DATA) {
        insert_sensor(f, data->id, data->value, data->ts);
    }

    free(data);
    close_db(f);

    return NULL;
}


int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }

    // create child log process
    create_log_process();

    // initialize shared data buffer
    sbuffer_t *buf;
    sbuffer_init(&buf);

    // create 3 main threads
    pthread_t t_connmgr;
    params_t *params_connmgr = malloc(sizeof(params_t));
    params_connmgr->param1 = argv[2];
    params_connmgr->param2 = argv[1];
    params_connmgr->param3 = buf;
    pthread_create(&t_connmgr, NULL, &connection_manager, params_connmgr);

    pthread_t t_datamgr;
    pthread_create(&t_datamgr, NULL, &data_manager, buf);

    pthread_t t_storagemgr;
    pthread_create(&t_storagemgr, NULL, &storage_manager, buf);

    // collect all threads
    pthread_join(t_connmgr, NULL);
    pthread_join(t_datamgr, NULL);
    pthread_join(t_storagemgr, NULL);

    // release buffer
    sbuffer_free(&buf);

    // free connection manager parameters
    free(params_connmgr);

    // termination message
    write_to_log_process("Sensor Gateway is shutting down.");

    // terminate log process
    end_log_process();

    // confirm successful termination
    printf("Yusuf TM Sensor Gateway terminated successfully.\n");

    return 0;
}