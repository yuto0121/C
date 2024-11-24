#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MIN_TEMP -10.0
#define MAX_TEMP 35.0
#define FREQUENCY 2

float gen_random_temp(float min, float max)
{
    return (float)(rand()) / (float)(RAND_MAX) * (max - min) + min;
}

int main()
{
    srand((unsigned int)time(NULL));
    while (1)
    {
        float temp = gen_random_temp(MIN_TEMP, MAX_TEMP);
        time_t rawtime = time(NULL);
        struct tm *timeinfo = localtime(&rawtime);
        char timeString[100];
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);
        printf("Temperature = %1.2f @%s\n", temp, timeString);
        fflush(stdout);
        sleep(FREQUENCY);
    }
    return 0;
}