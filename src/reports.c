#include <stdio.h>
#include <string.h>
#include <time.h>
#include "reports.h"
#include "attendance.h"
#include "schedule.h"

/* ------------------------------------------------------------------ */
/* Student report                                                       */
/* ------------------------------------------------------------------ */

void generate_student_report(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id)
{
    int i;
    int found = 0;

    printf("Attendance Report for Student %s\n\n", student_id);

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].student_id, student_id) == 0)
        {
            printf("Class ID   : %s\n", records[i].class_id);
            printf("Class Type : %s\n", records[i].class_type);
            printf("Date       : %s\n", records[i].date);
            printf("Time       : %s\n", records[i].timestamp);
            printf("Status     : %s\n\n", records[i].status);
            found = 1;
        }
    }

    if (!found)
        printf("No attendance records found.\n");
}

/* Student report filtered by date */
void generate_student_report_filtered(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *date)
{
    int i;
    int found = 0;

    printf("Attendance Report for Student %s on %s\n\n",
           student_id, date);

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].student_id, student_id) == 0 &&
            strcmp(records[i].date,        date)       == 0)
        {
            printf("Class ID   : %s\n", records[i].class_id);
            printf("Class Type : %s\n", records[i].class_type);
            printf("Date       : %s\n", records[i].date);
            printf("Time       : %s\n", records[i].timestamp);
            printf("Status     : %s\n\n", records[i].status);
            found = 1;
        }
    }

    if (!found)
        printf("No attendance records found.\n");
}

/* ------------------------------------------------------------------ */
/* Class report                                                         */
/* ------------------------------------------------------------------ */

void generate_class_report(
    const AttendanceRecord *records,
    int record_count,
    const char *class_id)
{
    int i;
    int found = 0;

    printf("Attendance Report for Class %s\n\n", class_id);

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].class_id, class_id) == 0)
        {
            printf("Student ID : %s\n", records[i].student_id);
            printf("Date       : %s\n", records[i].date);
            printf("Time       : %s\n", records[i].timestamp);
            printf("Status     : %s\n\n", records[i].status);
            found = 1;
        }
    }

    if (!found)
        printf("No attendance records found.\n");
}

/* Class report filtered by date */
void generate_class_report_filtered(
    const AttendanceRecord *records,
    int record_count,
    const char *class_id,
    const char *date)
{
    int i;
    int found = 0;

    printf("Attendance Report for Class %s on %s\n\n", class_id, date);

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].class_id, class_id) == 0 &&
            strcmp(records[i].date,      date)     == 0)
        {
            printf("Student ID : %s\n", records[i].student_id);
            printf("Date       : %s\n", records[i].date);
            printf("Time       : %s\n", records[i].timestamp);
            printf("Status     : %s\n\n", records[i].status);
            found = 1;
        }
    }

    if (!found)
        printf("No attendance records found.\n");
}

/* ------------------------------------------------------------------ */
/* CSV export                                                           */
/* ------------------------------------------------------------------ */

void export_student_report_csv(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *filename)
{
    FILE *file;
    int   i;
    int   found = 0;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error: Cannot create file %s\n", filename);
        return;
    }

    fprintf(file,
            "Student ID,Class ID,Class Type,Date,Time,Status\n");

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].student_id, student_id) == 0)
        {
            fprintf(file, "%s,%s,%s,%s,%s,%s\n",
                    records[i].student_id,
                    records[i].class_id,
                    records[i].class_type,
                    records[i].date,
                    records[i].timestamp,
                    records[i].status);
            found = 1;
        }
    }

    fclose(file);

    if (found)
        printf("CSV report exported to %s\n", filename);
    else
        printf("No attendance records found.\n");
}

/* ------------------------------------------------------------------ */
/* Schedule helpers                                                     */
/* ------------------------------------------------------------------ */

/* Convert "HH:MM" to minutes since midnight; returns -1 on error */
static int hhmm_to_minutes(const char *t)
{
    int h, m;
    if (strlen(t) != 5 || t[2] != ':') return -1;
    if (sscanf(t, "%d:%d", &h, &m) != 2) return -1;
    if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
    return h * 60 + m;
}

/* Print all classes scheduled for today */
void show_today_schedule(
    const Schedule *schedules,
    int schedule_count)
{
    int        i;
    time_t     t       = time(NULL);
    struct tm *tm_info = localtime(&t);
    char       today[20];

    strftime(today, sizeof(today), "%A", tm_info);

    printf("Today's Schedule (%s)\n\n", today);

    for (i = 0; i < schedule_count; i++)
    {
        if (strcmp(schedules[i].day, today) == 0)
        {
            printf("%s  |  %s  |  %s - %s\n",
                   schedules[i].class_id,
                   schedules[i].class_type,
                   schedules[i].start_time,
                   schedules[i].end_time);
        }
    }
}

/*
 * Print classes that are currently active OR start within the next 60 minutes.
 * Uses precise minute-level comparison.
 */
void show_active_classes(
    const Schedule *schedules,
    int schedule_count)
{
    int        i;
    int        now_min;
    time_t     t       = time(NULL);
    struct tm *tm_info = localtime(&t);
    char       today[20];

    strftime(today, sizeof(today), "%A", tm_info);
    now_min = tm_info->tm_hour * 60 + tm_info->tm_min;

    printf("Active / Upcoming Classes\n\n");

    for (i = 0; i < schedule_count; i++)
    {
        int start, end;

        if (strcmp(schedules[i].day, today) != 0)
            continue;

        start = hhmm_to_minutes(schedules[i].start_time);
        end   = hhmm_to_minutes(schedules[i].end_time);

        if (start == -1 || end == -1)
            continue;

        /* Active now: class has started but not ended */
        /* Upcoming:   starts within 60 minutes        */
        if (now_min >= start && now_min <= end)
        {
            printf("[ACTIVE]   %s (%s)  %s - %s\n",
                   schedules[i].class_id,
                   schedules[i].class_type,
                   schedules[i].start_time,
                   schedules[i].end_time);
        }
        else if (start > now_min && start - now_min <= 60)
        {
            printf("[UPCOMING] %s (%s)  starts at %s\n",
                   schedules[i].class_id,
                   schedules[i].class_type,
                   schedules[i].start_time);
        }
    }
}

/* Manually correct an attendance record's status */
void modify_attendance_status(
    AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *class_id,
    const char *status)
{
    int i;
    int found = 0;

    for (i = 0; i < record_count; i++)
    {
        if (strcmp(records[i].student_id, student_id) == 0 &&
            strcmp(records[i].class_id,   class_id)   == 0)
        {
            strncpy(records[i].status, status,
                    sizeof(records[i].status) - 1);
            records[i].status[sizeof(records[i].status) - 1] = '\0';

            printf("Attendance status updated: student %s, class %s -> %s\n",
                   student_id, class_id, status);
            found = 1;
        }
    }

    if (!found)
        printf("Attendance record not found.\n");
}
