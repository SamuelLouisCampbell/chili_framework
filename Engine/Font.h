#pragma once
#include "ChiliWin.h"
#include <ft2build.h>
//#include <freetype/freetype.h>
#include FT_FREETYPE_H

class Font
{
public:
	Font()
	{
		Init();
	}

	int GetNumGlyphs() const;
	FT_Bitmap GetGlyph(unsigned int code) const;
	

private:
	void Init();
	FT_Library library;
	FT_Face fontFace;
	
};
