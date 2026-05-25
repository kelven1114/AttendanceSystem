#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include "student.h"
#include "schedule.h"

typedef struct
{
    char student_id[20];
    char class_id[50];
    char class_type[50];
    char date[11];       /* YYYY-MM-DD */
    char timestamp[20];  /* HH:MM:SS   */
    char status[10];     /* PRESENT / LATE / REJECTED */
} AttendanceRecord;

int  time_to_minutes(const char *time_str);

int  is_duplicate(const AttendanceRecord *records, int count,
                  const char *student_id,
                  const char *class_id,
                  const char *date);

void do_checkin(const char *student_id,
                const Student  *students,  int student_count,
                const Schedule *schedules, int schedule_count,
                AttendanceRecord **records, int *record_count);

/* Persistence helpers */
int load_attendance_dat(const char *filename,
                        AttendanceRecord **records, int *count);
int save_attendance_dat(const char *filename,
                        const AttendanceRecord *records, int count);

#endif
