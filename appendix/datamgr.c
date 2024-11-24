/**
 * \author Yusuf Hussein
 */


#include "config.h"
#include "datamgr.h"
#include "lib/dplist.h"

void *element_copy(void *element)
{
    sensor_node_data_t *copy = malloc(sizeof(sensor_node_data_t));
    copy->sensor_id = ((sensor_node_data_t *)element)->sensor_id;
    copy->room_id = ((sensor_node_data_t *)element)->room_id;
    copy->reading_count = ((sensor_node_data_t *)element)->reading_count;
    for(int i = 0; i < RUN_AVG_LENGTH; i++)
        copy->past_readings[i] = ((sensor_node_data_t *)element)->past_readings[i];
    copy->running_avg = ((sensor_node_data_t *)element)->running_avg;
    copy->last_modified = ((sensor_node_data_t *)element)->last_modified;
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((sensor_node_data_t *)x)->sensor_id < ((sensor_node_data_t *)y)->sensor_id) ? -1
    : (((sensor_node_data_t *)x)->sensor_id == ((sensor_node_data_t *)y)->sensor_id) ? 0
    : 1);
}

dplist_t *sensor_list = NULL;

void add_past_reading(sensor_node_data_t *sensor, double value) {
    // increment count if its less than 5
    int count = sensor->reading_count < 5 ? ++sensor->reading_count : sensor->reading_count;

    // if the buffer isn't fully filled keep adding to the end
    if(count < RUN_AVG_LENGTH) {
        sensor->past_readings[count-1] = value;
    } else {
        // move all the elements back and add new value at end of buffer
        for(int i = 1; i < RUN_AVG_LENGTH; i++) {
            sensor->past_readings[i-1] = sensor->past_readings[i];
        }
        sensor->past_readings[RUN_AVG_LENGTH-1] = value;
    }
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t *buf) {
    // free sensor list if it has data allocated
    if(sensor_list != NULL) datamgr_free();

    // initialize sensor list
    sensor_list = dpl_create(element_copy, element_free, element_compare);

    // allocate memory for temporary node element
    sensor_node_data_t temp_node;

    // scan data from room_sensor_map
    while(fscanf(fp_sensor_map, "%hu %hu", &temp_node.room_id, &temp_node.sensor_id ) == 2) {
        // initialize other attributes to empty
        temp_node.running_avg = 0;
        temp_node.last_modified = 0;
        temp_node.reading_count = 0;
        for(int i = 0; i < RUN_AVG_LENGTH; i++) {
            temp_node.past_readings[i] = 0;
        }
        dpl_insert_at_index(sensor_list, &temp_node, dpl_size(sensor_list), true);
    }


    // allocate memory for temporary data struct
    sensor_data_t data;

    while(sbuffer_remove(buf, &data, 1) != SBUFFER_NO_DATA) {
        // go over list and find correct sensor
        bool found = false;
        for(int i = 0; i < dpl_size(sensor_list); i++) {
            sensor_node_data_t *sensor = dpl_get_element_at_index(sensor_list, i);

            // if correct sensor
            if(data.id == sensor->sensor_id) {
                // add value to past_readings
                add_past_reading(sensor, data.value);

                // update running average
                int r_count = sensor->reading_count;
                sensor->running_avg = 0;
                for(int j = 0; j < r_count; j++) sensor->running_avg += sensor->past_readings[j]/(r_count);


                // update last modified
                sensor->last_modified = data.ts;

                // log if temp is too high or too low
                if(sensor->running_avg > SET_MAX_TEMP) {
                    char temp_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
                    sprintf(temp_msg, "Sensor node %hu reports it’s too hot (avg temp = %f).", sensor->sensor_id, sensor->running_avg);
                    write_to_log_process(temp_msg);
                }
                else if(sensor->running_avg < SET_MIN_TEMP) {
                    char temp_msg[MSG_LENGTH] = {[0 ... MSG_LENGTH-1] = '\0'};
                    sprintf(temp_msg, "Sensor node %hu reports it’s too cold (avg temp = %f).", sensor->sensor_id, sensor->running_avg);
                    write_to_log_process(temp_msg);
                }

                found = true;
                break;
            }
        }

        // if sensor isn't found log it
        if(!found) {
            char not_found_msg[75];
            sprintf(not_found_msg, "Received sensor data with invalid sensor node ID %hu.", data.id);
            write_to_log_process(not_found_msg);
        }

    }

}



void datamgr_free() {
    dpl_free(&sensor_list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id) {
    // go over list and find correct sensor
    for(int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_node_data_t *sensor = dpl_get_element_at_index(sensor_list, i);

        // if correct sensor
        if(sensor_id == sensor->sensor_id) {
            return sensor->room_id;
        }
    }
    ERROR_HANDLER(true, "sensor_id was not found in the sensor list");
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id) {
    // go over list and find correct sensor
    for(int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_node_data_t *sensor = dpl_get_element_at_index(sensor_list, i);

        // if correct sensor
        if(sensor_id == sensor->sensor_id) {
            return sensor->running_avg;
        }
    }
    ERROR_HANDLER(true, "sensor_id was not found in the sensor list");
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id) {
    // go over list and find correct sensor
    for(int i = 0; i < dpl_size(sensor_list); i++) {
        sensor_node_data_t *sensor = dpl_get_element_at_index(sensor_list, i);

        // if correct sensor
        if(sensor_id == sensor->sensor_id) {
            return sensor->last_modified;
        }
    }
    ERROR_HANDLER(true, "sensor_id was not found in the sensor list");
}

int datamgr_get_total_sensors() {
    return dpl_size(sensor_list);
}

