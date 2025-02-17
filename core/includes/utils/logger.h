#pragma once
#include <stdio.h>

#define LOG_TAG "CORE"

#define logT(x, ...) (printf("\033[95m[%s]\033[0m \033[96m[Trace] " x " \033[0m\n", LOG_TAG, ##__VA_ARGS__))
#define logD(x, ...) (printf("\033[95m[%s]\033[0m \033[94m[Debug] " x " \033[0m\n", LOG_TAG, ##__VA_ARGS__))
#define logI(x, ...) (printf("\033[95m[%s]\033[0m \033[92m[Info] " x " \033[0m\n", LOG_TAG, ##__VA_ARGS__))
#define logW(x, ...) (printf("\033[95m[%s]\033[0m \033[93m[Warn] " x " \033[0m\n", LOG_TAG, ##__VA_ARGS__))
#define logE(x, ...) (printf("\033[95m[%s]\033[0m \033[91m[Error] " x " \033[0m\n", LOG_TAG, ##__VA_ARGS__))
