# sed -i 's/\r$//' src/*.c src/utils/*.c src/network/*.c src/world/*.c src/database/*.c src/sqlite/*.c build-linux.sh
clang \
src/*.c \
src/utils/*.c \
src/network/*.c \
src/world/*.c \
src/database/*.c \
src/sqlite/*.c \
-Iincludes -L/usr/lib -o server -pthread -O3
