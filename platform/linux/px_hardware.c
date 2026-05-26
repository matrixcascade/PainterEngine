#include "../modules/px_hardware.h"

#include <stdio.h>
#include <stdlib.h>


static float PX_Hardware_read_value(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror(path);
        return -1000000;
    }

    float value;
    if (fscanf(fp, "%f", &value) != 1) {
        fclose(fp);
        return -1000000;
    }

    fclose(fp);
    return value;
}


float PX_Hardware_GetCPUTemperature()
{
    const char *raw_path  = "/sys/bus/iio/devices/iio:device0/in_temp0_raw";
    float raw  = PX_Hardware_read_value(raw_path);
    if (raw < -999999 )
    {
        return 0;
    }

    return (raw*503.975)/4096 - 273.15; // Convert raw value to Celsius
}
