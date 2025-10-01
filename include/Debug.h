#ifndef DEBUG_H
#define DEBUG_H

enum class DebugOutputMode {
	CONSOLE,
	VS_OUTPUT,
};

void createDebugConsole();
void debugLog(const char *format, ...);
void debugError(const char *format, ...);

#endif // !DEBUG_H
