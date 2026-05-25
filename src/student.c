#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

/* Returns 1 if studentID is already in the valid array, 0 otherwise */
int isDuplicate(Student valid[], int count, int studentID)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (valid[i].studentID == studentID)
            return 1;
    }
    return 0;
}

/* Returns 1 if the line is a CSV header row */
static int is_header_line(const char *line)
{
    return (strstr(line, "Student ID") != NULL ||
            strstr(line, "student_id") != NULL ||
            strstr(line, "StudentID")  != NULL);
}

/* Trim trailing newline / carriage-return characters in place */
static void trim_newline(char *s)
{
    int len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    {
        s[--len] = '\0';
    }
}

/*
 * Load students from a CSV file.
 * Format: StudentID,Name,Email,Course,ClassType
 * Returns 1 on success, 0 on failure.
 */
int load_students_csv(const char *filename, Student **students, int *count)
{
    FILE   *csv;
    char    line[1024];
    Student temp;
    int     parsed;
    int     reject;
    int     rejectedCount = 0;
    int     capacity      = 128;

    *count    = 0;
    *students = (Student *)malloc((size_t)capacity * sizeof(Student));

    if (*students == NULL)
    {
        printf("Error: Memory allocation failed\n");
        return 0;
    }

    csv = fopen(filename, "r");
    if (csv == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        free(*students);
        *students = NULL;
        return 0;
    }

    while (fgets(line, sizeof(line), csv) != NULL)
    {
        trim_newline(line);

        /* Skip blank lines and any header rows embedded in the file */
        if (line[0] == '\0' || is_header_line(line))
            continue;

        memset(&temp, 0, sizeof(temp));
        parsed = sscanf(line,
                        "%d,%99[^,],%99[^,],%19[^,],%99[^\n\r]",
                        &temp.studentID,
                        temp.name,
                        temp.email,
                        temp.classid,
                        temp.classtype);

        if (parsed != 5)
        {
            printf("Error: Invalid record format - Record ignored\n");
            rejectedCount++;
            continue;
        }

        reject = 0;

        if (isDuplicate(*students, *count, temp.studentID))
        {
            printf("Error: Duplicate Student ID %d - Record ignored\n",
                   temp.studentID);
            reject = 1;
        }

        if (strchr(temp.email, '@') == NULL)
        {
            printf("Error: Invalid email '%s' (missing @) - Record ignored\n",
                   temp.email);
            reject = 1;
        }

        if (reject)
        {
            rejectedCount++;
            continue;
        }

        /* Grow array if needed */
        if (*count >= capacity)
        {
            Student *tmp;
            capacity *= 2;
            tmp = (Student *)realloc(*students,
                                     (size_t)capacity * sizeof(Student));
            if (tmp == NULL)
            {
                printf("Error: Memory reallocation failed\n");
                fclose(csv);
                return 0;
            }
            *students = tmp;
        }

        (*students)[*count] = temp;
        (*count)++;
    }

    fclose(csv);

    printf("Loaded %d valid records, %d records rejected\n",
           *count, rejectedCount);

    return 1;
}

/* Write all validated student records to a binary .dat file */
int save_students_dat(const char *filename,
                      const Student *students, int count)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Error: Cannot create file %s\n", filename);
        return 0;
    }

    if (fwrite(students, sizeof(Student), (size_t)count, f) != (size_t)count)
    {
        printf("Error: Failed to write data to %s\n", filename);
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}
