#pragma once
#include "ChiliWin.h"
#include <ft2build.h>
//#include <freetype/freetype.h>
#include FT_FREETYPE_H
#include "FreeType.h"
#include <thread>
#include <chrono>


class Font
{
public:

	//Pixel unit based constuctor
	Font(std::string const& fontFilename, int sizeInPix)
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
	Font(std::string const& fontFilename, int pointsize, int monitorDPI)
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
	int GetLineSpacing() const
	{
		return fontFace->height;
	}
	
	void ComputeString(std::string str)
	{
		pen_x = 0;
		pen_y = 0;
		num_glyphs = 0;

		FT_GlyphSlot slot = fontFace->glyph;
		bool use_kerning = FT_HAS_KERNING(fontFace);

		for (int n = 0; n < str.size(); n++)
		{
			glyph_index = FT_Get_Char_Index(fontFace, str[n]);

			if (use_kerning && previous && glyph_index)
			{
				FT_Vector delta;

				FT_Get_Kerning(fontFace, previous, glyph_index, FT_KERNING_DEFAULT, &delta);

				pen_x += (delta.x >> 6);
			}

			positions[num_glyphs].x = pen_x;
			positions[num_glyphs].y = pen_y;

			if (FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT))
			{
				OutputDebugString(L"...Error loading glyph slot...\n");
			}

			if (FT_Get_Glyph(fontFace->glyph, &glyphs[num_glyphs]))
			{
				OutputDebugString(L"...Error getting glyphs...\n");
			}

			pen_x += slot->advance.x >> 6;

			previous = glyph_index;
			num_glyphs++;
			ComputeBounds();


		}
	}

	void ComputeBounds()
	{
		RectI rect = { 100,200,0,0 };
		RectI glyph_rect = { 0,0,0,0 };
		RectI prev = glyph_rect;

		for (int n = 0; n < num_glyphs; n++)
		{
			FT_BBox temp;
			FT_Glyph_Get_CBox(glyphs[n], ft_glyph_bbox_pixels,
				&temp);
			glyph_rect = Convert_FT_BBox(temp);

			if (n < 1)
			{
				rect.left = glyph_rect.left;
			}
			rect.right += glyph_rect.right;
			rect.top = std::max(glyph_rect.top, prev.top);
			rect.bottom = rect.top - std::max(glyph_rect.bottom, prev.bottom);

		}
		string_rect = rect;
		
	}

	void RenderString(Graphics& gfx, Vec2& pos)
	{
		/* compute string dimensions in integer pixels */
		int string_width = string_rect.right - string_rect.left;
		int string_height = string_rect.bottom - string_rect.top;

		for (int n = 0; n < num_glyphs; n++)
		{
			FT_Glyph   image;
			FT_Vector  pen;

			image = glyphs[n];

			pen.x = positions[n].x;
			pen.y = positions[n].y;

			if (FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, &pen, 0))
			{
				OutputDebugString(L"...Error rendering glyphs...\n");
			}
			else
			{
				FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;

				Surface sfc{ &bit->bitmap, float(bit->left), float(myTargetHeight - bit->top) };
				gfx.DrawGlyph(pos.x + positions[n].x, pos.y + positions[n].y, sfc, gfx.GetScreenRect());

				FT_Done_Glyph(image);
			}
		
		}
		
	}

	RectI GetStringBox() const
	{
		return string_rect;
	}

private:
	RectI Convert_FT_BBox(const FT_BBox& box)
	{
		return RectI{
			int(box.yMin),
			int(box.yMax),
			int(box.xMin),
			int(box.yMax)
		};
	}
private:
	FreeType library; 
	FT_Face fontFace;

	static constexpr int maxChars = 1024;
	FT_UInt glyph_index = 0;
	int pen_x = 0;
	int pen_y = 0;
	int previous = 0;
	FT_UInt num_glyphs = 0;
	FT_Glyph glyphs[maxChars];
	Vec2i positions[maxChars];
	RectI string_rect;
	int myTargetWidth = 0;
	int myTargetHeight = 0;



};