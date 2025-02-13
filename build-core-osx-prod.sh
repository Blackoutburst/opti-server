# sed -i 's/\r$//' src/*.c src/utils/*.c src/network/*.c src/world/*.c src/database/*.c src/sqlite/*.c build-osx.sh
clang \
core/src/*.c \
core/src/utils/*.c \
core/src/network/*.c \
core/src/world/*.c \
core/src/database/*.c \
core/src/sqlite/*.c \
-Icore/includes -L/opt/homebrew/lib -o server -pthread -O3
