clang \
src/*.c \
src/utils/*.c \
src/network/*.c \
src/world/*.c \
-Iincludes -L/opt/homebrew/lib -o server -pthread -W -Wall -Wextra -Wpedantic -g3 -fno-omit-frame-pointer -fsanitize=address
