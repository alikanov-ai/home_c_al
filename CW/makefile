ifeq ($(OS),Windows_NT)
	# Команды для Windows
	RM = del
	EXE = .exe
else
	# Команды для Unix-подобных систем
	RM = rm -f
	EXE =
endif
TARGET = area$(EXE)

all: $(TARGET)

$(TARGET): main.o area_functions.o
	gcc -o $(TARGET) main.o area_functions.o

main.o: main.c area_functions.h
	gcc -c -o main.o main.c

area_functions.o: area_functions.c area_functions.h
	gcc -c -o area_functions.o area_functions.c

clean:
	$(RM) *.o $(TARGET)
