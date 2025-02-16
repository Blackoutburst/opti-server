# sed -i 's/\r$//' core/src/*.c core/src/library/*.c core/src/utils/*.c core/src/network/*.c core/src/world/*.c core/src/database/*.c core/src/sqlite/*.c build-core-osx-prod.sh
clang \
core/src/*.c \
core/src/library/*.c \
core/src/utils/*.c \
core/src/network/*.c \
core/src/world/*.c \
core/src/database/*.c \
core/src/sqlite/*.c \
-Icore/includes -L/opt/homebrew/lib -o server -pthread -O3
