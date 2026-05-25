#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schedule.h"

static void trim_newline(char *text)
{
    int len = (int)strlen(text);
    while (len > 0 &&
           (text[len - 1] == '\n' || text[len - 1] == '\r'))
    {
        text[--len] = '\0';
    }
}

static int is_valid_day(const char *day)
{
    return (strcmp(day, "Monday")    == 0 ||
            strcmp(day, "Tuesday")   == 0 ||
            strcmp(day, "Wednesday") == 0 ||
            strcmp(day, "Thursday")  == 0 ||
            strcmp(day, "Friday")    == 0);
}

static int is_valid_class_type(const char *class_type)
{
    return (strcmp(class_type, "Lecture")  == 0 ||
            strcmp(class_type, "Tutorial") == 0);
}

static int parse_time_to_minutes(const char *t)
{
    int h, m;
    if (strlen(t) != 5 || t[2] != ':') return -1;
    if (sscanf(t, "%d:%d", &h, &m) != 2) return -1;
    if (h < 0 || h > 23 || m < 0 || m > 59) return -1;
    return h * 60 + m;
}

static int class_id_exists(const Schedule *s, int count, const char *id)
{
    int i;
    for (i = 0; i < count; i++)
        if (strcmp(s[i].class_id, id) == 0) return 1;
    return 0;
}

/* Returns 1 if the line is a CSV header row (contains "Class ID" or "Class Type") */
static int is_header_line(const char *line)
{
    return (strstr(line, "Class ID")   != NULL ||
            strstr(line, "Class Type") != NULL ||
            strstr(line, "class_id")   != NULL);
}

static int validate_schedule(const Schedule *rec,
                              const Schedule *list, int count)
{
    int s, e;

    if (class_id_exists(list, count, rec->class_id))
    {
        printf("Error: Duplicate Class ID %s - Record ignored\n",
               rec->class_id);
        return 0;
    }

    if (!is_valid_class_type(rec->class_type))
    {
        printf("Error: Invalid class type '%s' - Record ignored\n",
               rec->class_type);
        return 0;
    }

    if (!is_valid_day(rec->day))
    {
        printf("Error: Invalid day '%s' - Record ignored\n", rec->day);
        return 0;
    }

    s = parse_time_to_minutes(rec->start_time);
    if (s == -1)
    {
        printf("Error: Invalid start time '%s' - Record ignored\n",
               rec->start_time);
        return 0;
    }

    e = parse_time_to_minutes(rec->end_time);
    if (e == -1)
    {
        printf("Error: Invalid end time '%s' - Record ignored\n",
               rec->end_time);
        return 0;
    }

    if (e <= s)
    {
        printf("Error: End time must be after start time for %s"
               " - Record ignored\n", rec->class_id);
        return 0;
    }

    return 1;
}

int load_schedule_csv(const char *filename,
                      Schedule **schedules, int *count)
{
    FILE     *file;
    char      line[MAX_LINE_LENGTH];
    Schedule  record;
    Schedule *temp;
    int       capacity = 8;
    int       rejected = 0;

    *count     = 0;
    *schedules = (Schedule *)malloc((size_t)capacity * sizeof(Schedule));

    if (*schedules == NULL)
    {
        printf("Error: Memory allocation failed\n");
        return 0;
    }

    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        free(*schedules);
        *schedules = NULL;
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        trim_newline(line);

        if (line[0] == '\0') continue;

        /* Skip header rows */
        if (is_header_line(line)) continue;

        memset(&record, 0, sizeof(record));
        if (sscanf(line, "%49[^,],%49[^,],%49[^,],%49[^,],%49[^\n\r]",
                   record.class_id,
                   record.class_type,
                   record.day,
                   record.start_time,
                   record.end_time) != 5)
        {
            printf("Error: Invalid CSV format - Record ignored\n");
            rejected++;
            continue;
        }

        if (!validate_schedule(&record, *schedules, *count))
        {
            rejected++;
            continue;
        }

        if (*count >= capacity)
        {
            capacity *= 2;
            temp = (Schedule *)realloc(*schedules,
                                       (size_t)capacity * sizeof(Schedule));
            if (temp == NULL)
            {
                printf("Error: Memory reallocation failed\n");
                free(*schedules);
                *schedules = NULL;
                fclose(file);
                return 0;
            }
            *schedules = temp;
        }

        (*schedules)[*count] = record;
        (*count)++;
    }

    fclose(file);

    printf("Loaded %d valid schedule records, %d records rejected\n",
           *count, rejected);
    return 1;
}

int save_schedule_dat(const char *filename,
                      const Schedule *schedules, int count)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Error: Cannot create file %s\n", filename);
        return 0;
    }

    if (fwrite(schedules, sizeof(Schedule), (size_t)count, f) !=
        (size_t)count)
    {
        printf("Error: Failed to write data to %s\n", filename);
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}
