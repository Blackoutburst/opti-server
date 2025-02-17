#include <string.h>
#include <stdlib.h>
#include "utils/args.h"
#include "utils/logger.h"

static U8 renderDistance = 2;
static U8 dbType = DB_FILE;

static I8* args[6] = {
    "-h", "--help",
    "-r", "--renderDistance",
    "-d", "--database",
};

void argsParse(I32 argc, I8** argv) {
    for (I32 i = 1; i < argc; i++) {
        if (!strcmp(argv[i], args[0]) || !strcmp(argv[i], args[1])) {
            argsHelp();
        }
        if (!strcmp(argv[i], args[2]) || !strcmp(argv[i], args[3])) {
            i++;
            argsSetRenderDistance(i, argc, argv);
        }
        if (!strcmp(argv[i], args[4]) || !strcmp(argv[i], args[5])) {
            i++;
            argsSetDbType(i, argc, argv);
        }
    }
}

void argsHelp(void) {
    const I8* help = "\033[37m==============================================\033[0m\n"
                    "\033[91m[Minecraft Wish server]\033[0m\n\n"
                    "\033[96m-h, --help\033[0m:\n"
                    "\tShow this message\n\n"
                    "\033[96m-r, --renderDistance\033[0m: \033[92m[1..255]\033[0m \033[93m(default: 2)\033[0m\n"
                    "\tSet the server maximum render distance\n\n"
                    "\033[96m-d, --database\033[0m: \033[92m[\"file\" \"ram\"]\033[0m \033[93m(default: \"file\")\033[0m\n"
                    "\tSet the server database type\n\n"
                    "\033[37m==============================================\033[0m\n";

    printf("%s", help);
    exit(0);
}

void argsSetRenderDistance(I32 i, I32 argc, I8** argv) {
    if (i >= argc) {
        logE("Missing value for argument --renderDistance");
        exit(1);
    }

    I32 r = atoi(argv[i]);
    if (r <= 0 || r > 255) {
        logE("--renderDistance value must be in range [1..255]");
        exit(1);
    }

    renderDistance = r;
}

U8 argsGetRenderDistance(void) {
    return renderDistance;
}

void argsSetDbType(I32 i, I32 argc, I8** argv) {
    if (i >= argc) {
        logE("Missing value for argument --database");
        exit(1);
    }
    if (strcmp(argv[i], "file") && strcmp(argv[i], "ram")) {
        logE("--database value must be either \"file\" or \"ram\" ");
        exit(1);
    }

    if (!strcmp(argv[i], "file")) {
        dbType = DB_FILE;
    }

    if (!strcmp(argv[i], "ram")) {
        dbType = DB_RAM;
    }
}

U8 argsGetDbType(void) {
    return dbType;
}

