# Command-Line Attendance Tracking System
## CMDP2063 Unix and C Programming — Group Project

---

## Project Overview

This system implements a command-line attendance management system in C for the School of Pre-University and Continuing Education (SPACE). It replaces a QR code-based system by providing robust student check-in, schedule management, and reporting entirely through terminal commands.

Key capabilities:
- Load and validate student records from a CSV file, saving valid records to a binary file
- Load and validate class schedules from a CSV file
- Automatic check-in with real-time class matching, grace period, and duplicate prevention
- Full reporting: per-student history, per-class reports, date filtering, CSV export
- Administrative commands: view today's schedule, active classes, manual record correction
- All attendance records persist across program runs via a binary file

---

## Compilation Instructions

Ensure you are in the project root directory (where the `Makefile` is located).

```bash
# Build the project
make

# Clean all compiled files and generated .dat files
make clean

# Rebuild from scratch
make clean && make
```

The project compiles with mandatory flags:
```
gcc -Wall -ansi -pedantic -Werror -g
```

---

## Usage Examples

All commands are run from the project root directory where the `attendance` binary is produced.

### Check in a student
```bash
./attendance --checkin 70001234
```
Output example:
```
Checking in at: Monday, 2026-05-25, 09:15:00
Matched class: CMDP1023_LEC (Lecture)
Check-in successful - Status: PRESENT
```

### Duplicate check-in prevention
```bash
./attendance --checkin 70001234
# Error: Already checked in to CMDP1023_LEC on 2026-05-25
```

### View today's schedule
```bash
./attendance --schedule
```

### View active or upcoming classes (within next 60 minutes)
```bash
./attendance --active
```

### Attendance report for a student
```bash
./attendance --report --student 70001234
```

### Student report filtered by date
```bash
./attendance --report --student 70001234 --date 2026-05-25
```

### Export student report to CSV
```bash
./attendance --report --student 70001234 --output report.csv
```

### Attendance report for a class
```bash
./attendance --report --class CMDP1023_LEC
```

### Class report filtered by date
```bash
./attendance --report --class CMDP1023_LEC --date 2026-05-25
```

### Manually correct an attendance record
```bash
./attendance --modify --student 70001234 --class CMDP1023_LEC --status PRESENT
```

### Display help
```bash
./attendance --help
```

---

## Data Files

### Input files (provided, do not modify)

| File | Format | Description |
|------|--------|-------------|
| `data/students.csv` | StudentID, Name, Email, Course, ClassType | Student records |
| `data/schedule.csv` | ClassID, ClassType, Day, StartTime, EndTime | Class timetable |

#### students.csv validation rules
- Student ID must be unique (duplicates are rejected)
- Email must contain the `@` symbol
- Records with invalid format are rejected with a clear error message

#### schedule.csv validation rules
- Class ID must be unique
- Class type must be exactly `Lecture` or `Tutorial`
- Day must be a valid weekday (Monday through Friday)
- Times must be in `HH:MM` 24-hour format
- End time must be after start time

### Auto-generated binary files

| File | Description |
|------|-------------|
| `data/students.dat` | Binary file of validated student records (created on each run) |
| `data/schedule.dat` | Binary file of validated schedule records (created on each run) |
| `data/attendance.dat` | Persistent binary file of all attendance records |

---

## Attendance Status Rules

| Status | Condition |
|--------|-----------|
| PRESENT | Check-in occurs during the scheduled class window |
| LATE | Check-in occurs after class end time but within 60-minute grace period |
| REJECTED | Check-in occurs after grace period, or no matching class is found |

---

## Team Contributions

| Name | Student ID | Modules Implemented | Notes |
|------|------------|---------------------|-------|
| Yong Hwa King | 700051531 | student.c / student.h(feature 2) | Imlemented automatic creation of students.dat, schedule progress checks |
| Matthias Leong Yong Kiang | | students.c student.h, makefile| Schedule loading, validation, binary storage |
| Kelven Tan Kai Wen | 700052260 | attendance.c / attendance.h | Implemented check-in logic, duplicate prevention, attendance persistence and status detection (PRESENT / LATE / REJECTED) |
| Christopher Wee Tian Ci | 700051770 | reports.c / README.md | Implemented reporting system, CSV export, date-filtered reports, and project documentation |
| Aron Lee Siew Ho| 700053800  | schedule.c and textevidence.pdf  | Schedule management, active class checking, time validation and system integration  |
