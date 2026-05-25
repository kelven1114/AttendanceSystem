#ifndef STUDENT_H
#define STUDENT_H

#define MAX_NAME       100
#define MAX_EMAIL      100
#define MAX_CLASSID    20
#define MAX_CLASS_TYPE 100

typedef struct {
    int  studentID;
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char classid[MAX_CLASSID];
    char classtype[MAX_CLASS_TYPE];
} Student;

int isDuplicate(Student valid[], int count, int studentID);
int load_students_csv(const char *filename, Student **students, int *count);
int save_students_dat(const char *filename, const Student *students, int count);

#endif
