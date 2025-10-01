#include <text.h>
#include <Debug.h>
#include <Windows.h>

void InitFreeType(Vec2 screenDPI) {
	if (FT_Init_FreeType(&ft)) {
		debugError("Failed to initialize FreeType\n");
		return;
	}
	dpi = screenDPI;
}

void LoadFont(const char* path) {
	FT_Face face;
	auto error = FT_New_Face(ft, path, 0, &face);
	if (error == FT_Err_Unknown_File_Format) {
		debugError("Font from \"%s\" is unsupported\n", path);
		return;
	} else if (error) {
		debugError("Failed to load font from \"%s\"\n", path);
		return;
	}

	FT_Set_Char_Size(face, 0, 12 * 64, dpi.x, dpi.y);
}