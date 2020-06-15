#pragma once
#include "ChiliWin.h"
#include <ft2build.h>
//#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include "FreeType.h"


class Font
{
public:
	Font(std::string const& fontFilename, FreeType& library)
	{
		// Here we call the .get() function on the 'library' FreeType object reference
		if (FT_New_Face(library.get(), fontFilename.c_str(), 0, &fontFace) != 0)
		{
			throw std::runtime_error("Failed to create new font face\n");
		}

		// Probably not a good idea to hard code the screen width and height, instead 
		// use the width and height used in Graphics so they always match.
		if (FT_Set_Char_Size(fontFace, 0, 16 * 64, Graphics::ScreenHeight, Graphics::ScreenWidth) != 0)
		{
			throw std::runtime_error("Failed to set char size\n");
		}
	}

	Font(Font const&) = delete;
	Font(Font&& other)noexcept
		:
		fontFace(other.fontFace)
	{
		other.fontFace = nullptr;
	}

	Font& operator=(Font const&) = delete;
	Font& operator=(Font&& other)noexcept
	{
		if (this != &other)
		{
			if (fontFace)
				FT_Done_Face(fontFace);

			fontFace = other.fontFace;
			other.fontFace = nullptr;
		}

		return *this;
	}
	~Font()noexcept
	{
		if (fontFace)
			FT_Done_Face(fontFace);
	}

	int GetNumGlyphs() const
	{
		return fontFace->num_glyphs;
	}
	FT_Bitmap GetGlyph(unsigned int code) const
	{
		int glyph_index = FT_Get_Char_Index(fontFace, code);
		FT_Load_Glyph(fontFace, glyph_index, 0);

		if (FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL))
		{
			throw std::runtime_error("Failed to render glyph to glyph bitmap\n");
		}

		return fontFace->glyph->bitmap;
	}



private:
	FT_Face fontFace;
};