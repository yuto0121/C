#include "sensor_db.h"
#include "logger.h"
#include <string.h>

FILE *open_db(char *filename, bool append)
{
    FILE *f;
    if (append)
    {
        f = fopen(filename, "a");
    }
    else
    {
        f = fopen(filename, "w");
    }
    if (f == NULL)
    {
        perror("Failed to open data file");
    }

    create_log_process();

    write_to_log_process("Data file opened.");

    return f;
}

int insert_sensor(FILE *f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
    if (f == NULL)
    {
        fprintf(stderr, "File pointer is null\n");
        return -1;
    }
    fprintf(f, "%u, %f, %ld\n", id, value, ts);
    fflush(f);

    write_to_log_process("Data inserted.");

    return 0;
}

int close_db(FILE *f)
{
    if (f == NULL)
    {
        fprintf(stderr, "File pointer is null\n");
        return -1;
    }
    fclose(f);

    write_to_log_process("Data file closed.");

    end_log_process();

    return 0;
}
