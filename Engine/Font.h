#pragma once
#include "ChiliWin.h"
#include <ft2build.h>
//#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include "FreeType.h"


class Font
{
public:

	//Pixel unit based constuctor
	Font(std::string const& fontFilename, FreeType& library, int sizeInPix)
	{
		if (FT_New_Face(library.get(), fontFilename.c_str(), 0, &fontFace) != 0)
		{
			throw std::runtime_error("Failed to create new font face\n");
		}

		if (FT_Set_Pixel_Sizes(fontFace, sizeInPix, sizeInPix) != 0)
		{
			throw std::runtime_error("Failed to set char size\n");
		}
	}
	//Classic use of fonts relate point size to DPI of display, use this constructor if you want this functionality
	Font(std::string const& fontFilename, FreeType& library, int pointsize, int monitorDPI)
	{
		if (FT_New_Face(library.get(), fontFilename.c_str(), 0, &fontFace) != 0)
		{
			throw std::runtime_error("Failed to create new font face\n");
		}
		if (FT_Set_Char_Size(fontFace, 0, pointsize * 64, monitorDPI, monitorDPI) != 0)
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
			{
				FT_Done_Face(fontFace);
			}

			fontFace = other.fontFace;
			other.fontFace = nullptr;
		}
		return *this;
	}
	~Font()noexcept
	{
		if (fontFace)
		{
			FT_Done_Face(fontFace);
		}
	}

	int GetNumGlyphs() const
	{
		return fontFace->num_glyphs;
	}
	
	void RenderString(Graphics& gfx, Vec2& pos, std::string str)
	{
		FT_GlyphSlot slot = fontFace->glyph;
		float pen_x = 0;
		float pen_y = 0;
		
		for (int n = 0; n < str.size(); n++)
		{
			FT_UInt glyph_index = FT_Get_Char_Index(fontFace, str[n]);

			if (FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT))
			{
				OutputDebugString(L"...Error loading glyph slot...\n");
			}
			if (FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL))
			{
				OutputDebugString(L"...Error loading glyph slot...\n");
			}

			Surface sfc{ &slot->bitmap, pen_x + slot->bitmap_left, pen_y - slot->bitmap_top };
			gfx.DrawSurface(pos.x + sfc.getPosOffset().x, pos.y + sfc.getPosOffset().y, sfc);

			pen_x += slot->advance.x >> 6;
			pen_y += slot->advance.y >> 6;
		}
	}
private:
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