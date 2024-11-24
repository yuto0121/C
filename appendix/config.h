/**
 * \author Yusuf Hussein
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef TIMEOUT
#define TIMEOUT 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 20
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 10
#endif

// global includes
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define TIME_LENGTH 50
#define MSG_LENGTH 100

/** Writes data to log file by sending the data to the log process through the pre-established pipe.
 * \param msg a string message that will be added to the log file with a timestamp and a sequence number.
 * \return returns 0 if the message was successfully sent to the log process, and a -1 if the transfer failed.
 */
int write_to_log_process(char *msg);

/** Creates child log process and set up a communication pipe to receive data from parent process.
 * \return returns 0 if the log process was created successfully, and -1 otherwise if an error occurred.
 */
int create_log_process();

/** Closes the connection with the log process and terminates it. Also reaps the child process from the process table.
 * \return returns 0 if the log process was terminated successfully, and -1 if an error occurred.
 */
int end_log_process();

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine
typedef uint16_t room_id_t;


typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

typedef struct {
    void *param1;
    void *param2;
    void *param3;
} params_t;


#endif /* _CONFIG_H_ */
