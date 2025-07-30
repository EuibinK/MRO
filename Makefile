# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17
TARGET = run
SOURCE = multiple_downloader.cpp

# Default target
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean target
clean:
	rm -f $(TARGET)

# Phony targets (not actual files)
.PHONY: clean
