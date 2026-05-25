#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "schedule.h"
#include "attendance.h"
#include "reports.h"

/* ------------------------------------------------------------------ */
/* Help                                                                 */
/* ------------------------------------------------------------------ */
static void print_help(const char *prog)
{
    printf("Usage: %s <command> [options]\n\n", prog);
    printf("Commands:\n");
    printf("  --checkin <StudentID>\n");
    printf("      Check a student into the current class.\n\n");
    printf("  --report --student <StudentID>\n");
    printf("      Show full attendance history for a student.\n\n");
    printf("  --report --student <StudentID> --date <YYYY-MM-DD>\n");
    printf("      Show student attendance filtered by date.\n\n");
    printf("  --report --student <StudentID> --output <file.csv>\n");
    printf("      Export student attendance history to a CSV file.\n\n");
    printf("  --report --class <ClassID>\n");
    printf("      Show all check-ins for a specific class.\n\n");
    printf("  --report --class <ClassID> --date <YYYY-MM-DD>\n");
    printf("      Show class attendance filtered by date.\n\n");
    printf("  --schedule\n");
    printf("      View all classes scheduled for today.\n\n");
    printf("  --active\n");
    printf("      View classes currently active or starting within"
           " the next hour.\n\n");
    printf("  --modify --student <ID> --class <ClassID>"
           " --status <STATUS>\n");
    printf("      Manually correct an attendance record status.\n\n");
    printf("  --help\n");
    printf("      Display this help message.\n");
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    Student          *students  = NULL;
    Schedule         *schedules = NULL;
    AttendanceRecord *records   = NULL;
    int student_count   = 0;
    int schedule_count  = 0;
    int record_count    = 0;

    /* ---- 1. Show help without loading data ---- */
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        print_help(argv[0]);
        return 0;
    }

    /* ---- 2. Load students ---- */
    if (!load_students_csv("data/students.csv", &students, &student_count))
    {
        printf("Failed to load students. Exiting.\n");
        return 1;
    }

    /* Save validated records to binary file */
    save_students_dat("data/students.dat", students, student_count);

    /* ---- 3. Load schedule ---- */
    if (!load_schedule_csv("data/schedule.csv", &schedules, &schedule_count))
    {
        printf("Failed to load schedule. Exiting.\n");
        free(students);
        return 1;
    }

    /* Save validated schedule to binary file */
    save_schedule_dat("data/schedule.dat", schedules, schedule_count);

    /* ---- 4. Load persisted attendance records ---- */
    if (!load_attendance_dat("data/attendance.dat", &records, &record_count))
    {
        printf("Failed to load attendance records. Exiting.\n");
        free(schedules);
        free(students);
        return 1;
    }

    /* ---------------------------------------------------------------- */
    /* Route commands                                                    */
    /* ---------------------------------------------------------------- */

    /* --checkin <StudentID> */
    if (argc == 3 && strcmp(argv[1], "--checkin") == 0)
    {
        do_checkin(argv[2],
                   students,  student_count,
                   schedules, schedule_count,
                   &records,  &record_count);

        save_attendance_dat("data/attendance.dat", records, record_count);
    }

    /* --schedule */
    else if (argc == 2 && strcmp(argv[1], "--schedule") == 0)
    {
        show_today_schedule(schedules, schedule_count);
    }

    /* --active */
    else if (argc == 2 && strcmp(argv[1], "--active") == 0)
    {
        show_active_classes(schedules, schedule_count);
    }

    /* --report --student <ID> --output <file.csv> */
    else if (argc == 6 &&
             strcmp(argv[1], "--report")  == 0 &&
             strcmp(argv[2], "--student") == 0 &&
             strcmp(argv[4], "--output")  == 0)
    {
        export_student_report_csv(records, record_count,
                                  argv[3], argv[5]);
    }

    /* --report --student <ID> --date <YYYY-MM-DD> */
    else if (argc == 6 &&
             strcmp(argv[1], "--report")  == 0 &&
             strcmp(argv[2], "--student") == 0 &&
             strcmp(argv[4], "--date")    == 0)
    {
        generate_student_report_filtered(records, record_count,
                                         argv[3], argv[5]);
    }

    /* --report --class <ClassID> --date <YYYY-MM-DD> */
    else if (argc == 6 &&
             strcmp(argv[1], "--report") == 0 &&
             strcmp(argv[2], "--class")  == 0 &&
             strcmp(argv[4], "--date")   == 0)
    {
        generate_class_report_filtered(records, record_count,
                                       argv[3], argv[5]);
    }

    /* --report --student <ID> */
    else if (argc == 4 &&
             strcmp(argv[1], "--report")  == 0 &&
             strcmp(argv[2], "--student") == 0)
    {
        generate_student_report(records, record_count, argv[3]);
    }

    /* --report --class <ClassID> */
    else if (argc == 4 &&
             strcmp(argv[1], "--report") == 0 &&
             strcmp(argv[2], "--class")  == 0)
    {
        generate_class_report(records, record_count, argv[3]);
    }

    /* --modify --student <ID> --class <ClassID> --status <STATUS> */
    else if (argc == 8 &&
             strcmp(argv[1], "--modify")   == 0 &&
             strcmp(argv[2], "--student")  == 0 &&
             strcmp(argv[4], "--class")    == 0 &&
             strcmp(argv[6], "--status")   == 0)
    {
        modify_attendance_status(records, record_count,
                                 argv[3], argv[5], argv[7]);
        save_attendance_dat("data/attendance.dat", records, record_count);
    }

    /* Unknown / missing command */
    else
    {
        printf("Unknown or incomplete command.\n");
        print_help(argv[0]);
        free(records);
        free(schedules);
        free(students);
        return 1;
    }

    free(records);
    free(schedules);
    free(students);

    return 0;
}
