CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = toml-verify.exe
SOURCE = toml_verify.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	del $(TARGET) 2>nul || true

.PHONY: all clean
