#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "attendance.h"

/* Convert "HH:MM" string to total minutes since midnight.
   Returns -1 on error. */
int time_to_minutes(const char *time_str)
{
    int h, m;

    if (strlen(time_str) != 5 || time_str[2] != ':')
        return -1;

    if (sscanf(time_str, "%d:%d", &h, &m) != 2)
        return -1;

    if (h < 0 || h > 23 || m < 0 || m > 59)
        return -1;

    return h * 60 + m;
}

/* Returns 1 if the student has already checked into this class on this date */
int is_duplicate(const AttendanceRecord *records, int count,
                 const char *student_id,
                 const char *class_id,
                 const char *date)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (strcmp(records[i].student_id, student_id) == 0 &&
            strcmp(records[i].class_id,   class_id)   == 0 &&
            strcmp(records[i].date,        date)       == 0)
        {
            return 1;
        }
    }
    return 0;
}

/* Find index of student by ID string; returns -1 if not found */
static int find_student(const Student *students, int count, const char *id)
{
    int i;
    int sid = atoi(id);
    for (i = 0; i < count; i++)
    {
        if (students[i].studentID == sid)
            return i;
    }
    return -1;
}

/*
 * Attempt to check a student into a matching class.
 * Appends to *records on success; prints an error on failure.
 */
void do_checkin(const char *student_id,
                const Student  *students,  int student_count,
                const Schedule *schedules, int schedule_count,
                AttendanceRecord **records, int *record_count)
{
    int  current_total;
    char current_day[20];
    char date_str[11];
    char time_str[20];
    int  grace = 60; /* minutes */
    int  i;

    time_t     now     = time(NULL);
    struct tm *tm_info = localtime(&now);

    if (find_student(students, student_count, student_id) == -1)
    {
        printf("Error: Student ID %s not found\n", student_id);
        return;
    }

    current_total = tm_info->tm_hour * 60 + tm_info->tm_min;

    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    switch (tm_info->tm_wday)
    {
        case 1: strcpy(current_day, "Monday");    break;
        case 2: strcpy(current_day, "Tuesday");   break;
        case 3: strcpy(current_day, "Wednesday"); break;
        case 4: strcpy(current_day, "Thursday");  break;
        case 5: strcpy(current_day, "Friday");    break;
        default:
            printf("Error: No classes on weekends\n");
            return;
    }

    printf("Checking in at: %s, %s, %s\n",
           current_day, date_str, time_str);

    for (i = 0; i < schedule_count; i++)
    {
        int start, end;
        char status[10];
        AttendanceRecord *temp;

        if (strcmp(schedules[i].day, current_day) != 0)
            continue;

        start = time_to_minutes(schedules[i].start_time);
        end   = time_to_minutes(schedules[i].end_time);

        if (start == -1 || end == -1)
            continue;

        /* Before class starts */
        if (current_total < start)
            continue;

        /* After grace period */
        if (current_total > end + grace)
            continue;

        /* Determine status */
        if (current_total <= end)
            strcpy(status, "PRESENT");
        else
            strcpy(status, "LATE");

        /* Duplicate check */
        if (is_duplicate(*records, *record_count,
                         student_id, schedules[i].class_id, date_str))
        {
            printf("Error: Already checked in to %s on %s\n",
                   schedules[i].class_id, date_str);
            return;
        }

        /* Grow array */
        temp = (AttendanceRecord *)realloc(
            *records,
            (size_t)(*record_count + 1) * sizeof(AttendanceRecord));

        if (temp == NULL)
        {
            printf("Error: Memory allocation failed\n");
            return;
        }

        *records = temp;

        memset(&(*records)[*record_count], 0, sizeof(AttendanceRecord));
        strcpy((*records)[*record_count].student_id,  student_id);
        strcpy((*records)[*record_count].class_id,    schedules[i].class_id);
        strcpy((*records)[*record_count].class_type,  schedules[i].class_type);
        strcpy((*records)[*record_count].date,         date_str);
        strcpy((*records)[*record_count].timestamp,    time_str);
        strcpy((*records)[*record_count].status,       status);

        (*record_count)++;

        printf("Matched class: %s (%s)\n",
               schedules[i].class_id, schedules[i].class_type);
        printf("Check-in successful - Status: %s\n", status);

        return;
    }

    printf("No matching class found - Status: REJECTED\n");
}

/* Load attendance records from binary file; allocates *records */
int load_attendance_dat(const char *filename,
                        AttendanceRecord **records, int *count)
{
    FILE           *f;
    AttendanceRecord rec;
    int             capacity = 64;

    *count   = 0;
    *records = (AttendanceRecord *)malloc(
                   (size_t)capacity * sizeof(AttendanceRecord));

    if (*records == NULL)
    {
        printf("Error: Memory allocation failed\n");
        return 0;
    }

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        /* File may not exist yet on first run - that is OK */
        return 1;
    }

    while (fread(&rec, sizeof(AttendanceRecord), 1, f) == 1)
    {
        if (*count >= capacity)
        {
            AttendanceRecord *tmp;
            capacity *= 2;
            tmp = (AttendanceRecord *)realloc(
                      *records,
                      (size_t)capacity * sizeof(AttendanceRecord));
            if (tmp == NULL)
            {
                printf("Error: Memory reallocation failed\n");
                fclose(f);
                return 0;
            }
            *records = tmp;
        }

        (*records)[*count] = rec;
        (*count)++;
    }

    fclose(f);
    return 1;
}

/* Overwrite the binary file with all current records */
int save_attendance_dat(const char *filename,
                        const AttendanceRecord *records, int count)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Error: Cannot create file %s\n", filename);
        return 0;
    }

    if (count > 0 &&
        fwrite(records, sizeof(AttendanceRecord), (size_t)count, f) !=
        (size_t)count)
    {
        printf("Error: Failed to write attendance data\n");
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}
