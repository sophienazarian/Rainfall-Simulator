CC = g++
CFLAGS = -std=c++11 -Wall -I/opt/homebrew/include -I/usr/local/include
LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -L/opt/homebrew/lib -L/usr/local/lib -lglew -lglfw

TARGET = water_simulation
SOURCE = water_simulation.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
