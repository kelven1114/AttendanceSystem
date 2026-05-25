#ifndef REPORTS_H
#define REPORTS_H

#include "attendance.h"
#include "schedule.h"

void generate_student_report(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id
);

void generate_student_report_filtered(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *date
);

void generate_class_report(
    const AttendanceRecord *records,
    int record_count,
    const char *class_id
);

void generate_class_report_filtered(
    const AttendanceRecord *records,
    int record_count,
    const char *class_id,
    const char *date
);

void export_student_report_csv(
    const AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *filename
);

void show_today_schedule(
    const Schedule *schedules,
    int schedule_count
);

void show_active_classes(
    const Schedule *schedules,
    int schedule_count
);

void modify_attendance_status(
    AttendanceRecord *records,
    int record_count,
    const char *student_id,
    const char *class_id,
    const char *status
);

#endif
