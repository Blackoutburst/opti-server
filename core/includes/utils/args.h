#pragma once

#include "utils/types.h"

#define DB_FILE 0
#define DB_RAM 1

void argsHelp(void);
void argsParse(I32 argc, I8** argv);
void argsSetRenderDistance(I32 i, I32 argc, I8** argv);
U8 argsGetRenderDistance(void);
void argsSetDbType(I32 i, I32 argc, I8** argv);
U8 argsGetDbType(void);
