# sed -i 's/\r$//' core/src/*.c core/src/library/*.c core/src/utils/*.c core/src/network/*.c core/src/world/*.c core/src/database/*.c core/src/sqlite/*.c build-core-linux-dev.sh
clang \
core/src/*.c \
core/src/library/*.c
core/src/utils/*.c \
core/src/network/*.c \
core/src/world/*.c \
core/src/database/*.c \
core/src/sqlite/*.c \
-Icore/includes -L/usr/lib -o server -pthread -W -Wall -Wextra -Wpedantic -Wno-unused-parameter -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-return=always
