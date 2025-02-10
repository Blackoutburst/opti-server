# sed -i 's/\r$//' src/*.c src/utils/*.c src/network/*.c src/world/*.c src/database/*.c src/sqlite/*.c build-osx.sh
clang \
src/*.c \
src/utils/*.c \
src/network/*.c \
src/world/*.c \
src/database/*.c \
src/sqlite/*.c \
-Iincludes -L/opt/homebrew/lib -o server -pthread -W -Wall -Wextra -Wpedantic -Wno-unused-parameter -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-return=always
