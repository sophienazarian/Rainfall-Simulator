# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL



# Target executable
TARGET = 3d_simulation

# Source file
SRC = 3d.cpp Droplet.cpp ShaderUtils.cpp

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)