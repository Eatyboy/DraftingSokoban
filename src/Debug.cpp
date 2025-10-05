#include <Debug.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdio>

DebugOutputMode debugOutputMode = DebugOutputMode::VS_OUTPUT;

void createDebugConsole() {
#ifndef DEBUG
    return;
#endif

    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);

        debugOutputMode = DebugOutputMode::CONSOLE;

        debugLog("Created console!\n");
    }
}


void debugLog(const char *format, ...) {
#ifndef DEBUG
    return;
#endif

    const char* prefix = "LOG: ";
    size_t prefixLen = strlen(prefix);

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    size_t formattedMessageLen = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    size_t totalBufferSize = prefixLen + formattedMessageLen + 1;
    std::vector<char> buffer(totalBufferSize);

    strcpy_s(buffer.data(), totalBufferSize, prefix);
    vsnprintf(buffer.data() + prefixLen, totalBufferSize - prefixLen, format, args);

    va_end(args);

    if (debugOutputMode == DebugOutputMode::VS_OUTPUT) {
        OutputDebugStringA(buffer.data());
    }
    else {
        printf(buffer.data());
    }
}

void debugError(const char *format, ...) {
#if PRODUCTION_BUILD == 1
    return;
#endif

    const char* prefix = "ERROR: ";
    size_t prefixLen = strlen(prefix);

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    size_t formattedMessageLen = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    size_t totalBufferSize = prefixLen + formattedMessageLen + 1;
    std::vector<char> buffer(totalBufferSize);

    strcpy_s(buffer.data(), totalBufferSize, prefix);
    vsnprintf(buffer.data() + prefixLen, totalBufferSize - prefixLen, format, args);

    va_end(args);

    if (debugOutputMode == DebugOutputMode::VS_OUTPUT) {
        OutputDebugStringA(buffer.data());
    }
    else {
        printf(buffer.data());
    }
}
