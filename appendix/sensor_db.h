/**
 * \author Yusuf Hussein
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include "config.h"

/** Opens the sensor data file and logs the operation.
 * \param filename a string identifying the name of the file that the data will be written to.
 * \param append a boolean determining whether the file will be overwritten with the data or if the data will just be appended.
 * \return returns a FILE* to the file if the file was opened properly, otherwise returns NULL.
 */
FILE * open_db(char * filename, bool append);

/** Inserts the sensor data to the csv file and logs the operation.
 * \param f a FILE* to the file to be written into.
 * \param id the id of the sensor to be written into the file.
 * \param value the value read by the sensor to be written into the file.
 * \param ts the timestamp of the sensor reading to be written into the file.
 * \return returns 0 if the data was written successfully and a -1 if the write failed.
 */
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/** Closes the sensor data file and logs the operation.
 * \param f a FILE* to the file to be written into.
 * \return returns 0 if the file was closed successfully and the log process was terminated or a -1 otherwise
 */
int close_db(FILE * f);


#endif /* _SENSOR_DB_H_ */