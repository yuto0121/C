/**
 * \author Yusuf Hussein
 */

#include "sensor_db.h"

bool open = false;

FILE * open_db(char * filename, bool append) {

    FILE *db = fopen(filename,append ? "a" : "w+"); // open file

    // create the log process and handle errors
    if(db == NULL) {
        printf("File Error: failed to open db file.\n"); // handle errors
        write_to_log_process("Data file failed to open.");
    }
    else {
        write_to_log_process("A new data.csv file has been created.");
    }

    open = true;
    return db;
}


int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if(!open) { // if the db is not currently open
        printf("Database Error: can't insert sensor. Database is not currently open.\n");
        return -1;
    }

    int response_code = fprintf(f, "%d, %lf, %ld\n", id, value, ts); // write to file
    fflush(f);
    if(response_code < 0) {
        printf("File Write Error: failed to write sensor data.\n"); // handle errors
        write_to_log_process("Data insertion failed.");
    }
    else {
        char insert_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
        sprintf(insert_msg, "Data insertion from sensor %hu succeeded.", id);
        write_to_log_process(insert_msg);
    }

    return response_code < 0 ? -1 : 0;
}

int close_db(FILE * f) {
    if(!open) { // if the db is not currently open.
        printf("Database Error: can't close database. Database is not currently open.\n");
        return -1;
    }
    open = false; // set db to closed.

    int response_code = fclose(f); // close file
    if(response_code < 0) {
        printf("File Write Error: failed to write sensor data.\n"); // handle errors
        write_to_log_process("Data file failed to close.");
    }
    else {
        write_to_log_process("The data.csv file has been closed.");
    }

    return response_code < 0 ? -1 : 0;
}