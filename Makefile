CC = clang++
CFLAGS = $(shell pkg-config --cflags opencv4)
LDFLAGS = $(shell pkg-config --libs opencv4)
TARGET = main
SRCS = cpp/main.cpp cpp/detect_champ.cpp cpp/remove_terrain.cpp
OBJS = $(SRCS:.cpp=.o)
BUILD_DIR = build

all: $(TARGET) compile_commands.json

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# maybe this will break sometime, use bear if so
compile_commands.json: $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -MJ $(BUILD_DIR)/compile_commands.json.tmp -c $(SRCS)
	echo '[' > compile_commands.json
	cat $(BUILD_DIR)/compile_commands.json.tmp | sed '$$ s/,$$//' >> compile_commands.json
	echo ']' >> compile_commands.json

clean:
	rm -f $(TARGET) $(OBJS)
	rm -rf $(BUILD_DIR)
	rm -f compile_commands.json
