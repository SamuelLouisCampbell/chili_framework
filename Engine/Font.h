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

	Font(const Font& other)
		:
		fontFace(other.fontFace)
	{

	}
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
		RectI rect = { 0,0,0,0 };
		RectI glyph_rect = { 0,0,0,0 };
		int hasKern = 0;

	
		FT_GlyphSlot slot = fontFace->glyph;
		bool use_kerning = FT_HAS_KERNING(fontFace);

		for (unsigned int n = 0; n < str.size(); n++)
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

			//calcultate bounds

			FT_BBox temp;
			FT_Glyph_Get_CBox(glyphs[n], ft_glyph_bbox_pixels,
				&temp);
			glyph_rect = Convert_FT_BBox(temp);

			if (n < 1)
			{
				rect.left = glyph_rect.left;
			}

			rect.right += (fontFace->glyph->advance.x / 64);
			rect.top = std::max(rect.top, glyph_rect.top);
			rect.bottom = std::max(rect.bottom, glyph_rect.bottom);

			//update draw position
			pen_x += slot->advance.x >> 6;

			previous = glyph_index;
			num_glyphs++;
		
		}

		rect.top -= rect.bottom;
		rect.bottom -= rect.bottom;
		string_rect = rect;
		hasComputed = true; 
		
	}

	void RenderString(Graphics& gfx, Vec2& pos)
	{
		if (hasComputed)
		{
			int string_width = string_rect.right - string_rect.left;
			int string_height = string_rect.bottom - string_rect.top;

			for (unsigned int n = 0; n < num_glyphs; n++)
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
					gfx.DrawGlyph(int(pos.x) + positions[n].x, int(pos.y) + positions[n].y, sfc, gfx.GetScreenRect(), 0.5f);
					
					FT_Done_Glyph(glyphs[n]); //free old glyph to prevent memory leak
					FT_Done_Glyph(image); //free cur glyph
				}
			
			}
		}
		else
		{
			OutputDebugString(L"Please compute string before calling RenderString function\n");
		}
		//reset computed flag for next frame
		hasComputed = false;
	}

	RectI GetStringBox() const
	{
		return string_rect;
	}
	int GetStringWidth() const
	{
		return string_rect.right - string_rect.left;
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
	FT_Face fontFace = nullptr;

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
	bool hasComputed = false;



};