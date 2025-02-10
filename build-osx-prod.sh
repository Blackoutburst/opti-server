# sed -i 's/\r$//' src/*.c src/utils/*.c src/network/*.c src/world/*.c src/database/*.c src/sqlite/*.c build-osx.sh
clang \
src/*.c \
src/utils/*.c \
src/network/*.c \
src/world/*.c \
src/database/*.c \
src/sqlite/*.c \
-Iincludes -L/opt/homebrew/lib -o server -pthread -O3
