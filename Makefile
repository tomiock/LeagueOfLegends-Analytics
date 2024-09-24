CC = clang++
CFLAGS = $(shell pkg-config --cflags opencv4)
LDFLAGS = $(shell pkg-config --libs opencv4)
TARGET = opencv_lol
SRCS = cpp/main.cpp cpp/detect_champ.cpp cpp/remove_terrain.cpp cpp/circle_priority.cpp
OBJS = $(SRCS:.cpp=.o)
BUILD_DIR = build

# Flag to control generation of compile_commands.json
GENERATE_COMPILE_COMMANDS ?= 0

ifeq ($(OPENCV_DEBUG), 1)
CFLAGS += -DOPENCV_DEBUG
endif

all: $(TARGET)
ifeq ($(GENERATE_COMPILE_COMMANDS), 1)
all: compile_commands.json
endif

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

compile_commands.json: $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -MJ $(BUILD_DIR)/compile_commands.json.tmp -c $(SRCS)
	echo '[' > compile_commands.json
	cat $(BUILD_DIR)/compile_commands.json.tmp | sed '$$ s/,$$//' >> compile_commands.json
	echo ']' >> compile_commands.json

clean:
	rm -f *.o
	rm -f $(TARGET) $(OBJS)
	rm -rf $(BUILD_DIR)
