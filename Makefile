CC     = gcc
CFLAGS = -Wall -ansi -pedantic -Werror -g
SRCS   = src/main.c src/student.c src/schedule.c src/attendance.c src/reports.c
OBJS   = main.o student.o schedule.o attendance.o reports.o
EXEC   = attendance

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

main.o: src/main.c src/student.h src/schedule.h src/attendance.h src/reports.h
	$(CC) $(CFLAGS) -c src/main.c -o main.o

student.o: src/student.c src/student.h
	$(CC) $(CFLAGS) -c src/student.c -o student.o

schedule.o: src/schedule.c src/schedule.h
	$(CC) $(CFLAGS) -c src/schedule.c -o schedule.o

attendance.o: src/attendance.c src/attendance.h src/student.h src/schedule.h
	$(CC) $(CFLAGS) -c src/attendance.c -o attendance.o

reports.o: src/reports.c src/reports.h src/attendance.h src/schedule.h
	$(CC) $(CFLAGS) -c src/reports.c -o reports.o

clean:
	rm -f $(OBJS) $(EXEC) data/students.dat data/schedule.dat data/attendance.dat

# Sample run targets
run:
	./$(EXEC) --help

checkin:
	./$(EXEC) --checkin 70001234

schedule:
	./$(EXEC) --schedule

active:
	./$(EXEC) --active
