#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <string.h>

/* Poll every 10s by default */
#ifndef POLL_INTERVAL
    #define POLL_INTERVAL 10
#endif

#if DEBUG
    #undef DEBUG
    #define DEBUG(...) fprintf(stderr, "DEBUG: "__VA_ARGS__); fprintf(stderr, "\n")
#else
    #undef DEBUG
    #define DEBUG(...)
#endif

void set_fan_speed(const char *name, int speed){
    DEBUG("set %s = %d", name, speed);
    int r=sysctlbyname(name, NULL, NULL, &speed, sizeof(speed));
    if (r!=0){
        fprintf(stderr, "Error setting %s: %s\n", name, strerror(errno));
        exit(1);
    }
}

int get_temps(char **names){
    int temp_avg=0;
    
    int i;
    for (i=0; names[i]!=NULL; i++){
        int temp;
        size_t len=sizeof(temp);
        int r=sysctlbyname(names[i], &temp, &len, NULL, 0);
        if (r!=0){
            fprintf(stderr, "Error setting %s: %s\n", names[i], strerror(errno));
            exit(1);
        }
        temp_avg+=temp;
    }
    temp_avg/=i;
    
    return temp_avg;
}

void usage(char *argv[]){
    printf(
        "%s MIN_MAX_ARGS SYSCTL_NAMES...\n"
        "\n"
        "The following MIN_MAX_ARGS are required:\n"
        "--fan-min INT    Minimum speed the fan should run at\n"
        "--fan-max INT    Maximum speed the fan should run at\n"
        "--temp-min INT   Temperature at which the fan should spin at minimum speed\n"
        "--temp-max INT   Temperature at which the fan should spin at maximum speed\n"
        "\n"
        "SYSCTL_NAMES:\n"
        "List of all sysctl names to use, consisting of one or more temperatures and one fan\n"
        "\n"
        "Example: %s --fan-min 1400 --fan-max 4350 --temp-min 55 --temp-max 65 dev.asmc.0.temp.gpu_0_die dev.asmc.0.fan.0.minspeed\n",
        argv[0],
        argv[0]
    );
}

int main(int argc, char *argv[]){
    /* Process arguments */
    int fan_speed_min=-1, fan_speed_max=-1;
    int temp_min=-1, temp_max=-1;
    char *temp_names[16]={NULL};
    char *fan_name=NULL;
    int j=0;
    for (int i=1; i<argc; i++){
        if (strstr(argv[i], ".temp.")){
            temp_names[j]=argv[i];
            j++;
        } else if (strstr(argv[i], ".fan.")){
            fan_name=argv[i];
        } else if (!strcmp(argv[i], "--fan-min")){
            fan_speed_min=atoi(argv[i+1]); i++;
        } else if (!strcmp(argv[i], "--fan-max")){
            fan_speed_max=atoi(argv[i+1]); i++;
        } else if (!strcmp(argv[i], "--temp-min")){
            temp_min=atoi(argv[i+1]); i++;
        } else if (!strcmp(argv[i], "--temp-max")){
            temp_max=atoi(argv[i+1]); i++;
        } else if (!strcmp(argv[i], "--help")){
            usage(argv);
            exit(0);
        }
    }
    
    if (fan_speed_min==-1 || fan_speed_max==-1 || temp_min==-1 || temp_max==-1 || fan_name==NULL || temp_names[0]==NULL){
        usage(argv);
        exit(1);
    }
    
    /* Main loop */
    int temp_new=0;
    while(1){
        int temp_old=temp_new;
        temp_new=get_temps(temp_names);
        DEBUG("temp_new=%d", temp_new);
        
        if (temp_new!=temp_old){
            int speed;
            if (temp_new>=temp_max){
                speed=fan_speed_max;
            } else if (temp_new<=temp_min){
                speed=fan_speed_min;
            } else {
                speed=(int)(fan_speed_min+(float)(fan_speed_max-fan_speed_min)/(float)(temp_max-temp_min)*(float)(temp_new-temp_min));
            }
            set_fan_speed(fan_name, speed);
        }
        
        sleep(POLL_INTERVAL);
    }
    
    return 0;
}
