clang src/*.c src/utils/*.c src/network/*.c src/world/*.c -Iincludes -Llib -o server -lws2_32 -W -Wall -Wextra -Wpedantic -g3 -fno-omit-frame-pointer -fsanitize=address
