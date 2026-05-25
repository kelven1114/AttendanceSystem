#ifndef SCHEDULE_H
#define SCHEDULE_H

#define MAX_LINE_LENGTH  256
#define MAX_FIELD_LENGTH  50

typedef struct {
    char class_id[MAX_FIELD_LENGTH];
    char class_type[MAX_FIELD_LENGTH];
    char day[MAX_FIELD_LENGTH];
    char start_time[MAX_FIELD_LENGTH];
    char end_time[MAX_FIELD_LENGTH];
} Schedule;

int  load_schedule_csv(const char *filename,
                       Schedule **schedules, int *count);
int  save_schedule_dat(const char *filename,
                       const Schedule *schedules, int count);

#endif
