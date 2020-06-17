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
		FT_GlyphSlot slot = fontFace->glyph;
		bool use_kerning = FT_HAS_KERNING(fontFace);

		for (int n = 0; n < str.size(); n++)
		{
			glyph_index = FT_Get_Char_Index(fontFace, str[n]);

			if (use_kerning && previous && glyph_index)
			{
				FT_Vector  delta;


				FT_Get_Kerning(fontFace, previous, glyph_index,
					FT_KERNING_DEFAULT, &delta);

				pen_x += delta.x * 64;
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
			//old but working code. 
			/*if (FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL))
			{
				OutputDebugString(L"...Error loading glyph slot...\n");
			}*/

			pen_x += slot->advance.x * 64;
			pen_y += slot->advance.y * 64;

			previous = glyph_index;
			num_glyphs++;

		}
	}
	FT_BBox ComputeBounds()
	{
		FT_BBox  bbox;
		FT_BBox  glyph_bbox;

		/* initialize string bbox to "empty" values */
		bbox.xMin = bbox.yMin = 32000;
		bbox.xMax = bbox.yMax = -32000;

		/* for each glyph image, compute its bounding box, */
		/* translate it, and grow the string bbox          */
		for (int n = 0; n < num_glyphs; n++)
		{
			FT_Glyph_Get_CBox(glyphs[n], ft_glyph_bbox_pixels,
				&glyph_bbox);

			glyph_bbox.xMin += positions[n].x;
			glyph_bbox.xMax += positions[n].x;
			glyph_bbox.yMin += positions[n].y;
			glyph_bbox.yMax += positions[n].y;

			if (glyph_bbox.xMin < bbox.xMin)
				bbox.xMin = glyph_bbox.xMin;

			if (glyph_bbox.yMin < bbox.yMin)
				bbox.yMin = glyph_bbox.yMin;

			if (glyph_bbox.xMax > bbox.xMax)
				bbox.xMax = glyph_bbox.xMax;

			if (glyph_bbox.yMax > bbox.yMax)
				bbox.yMax = glyph_bbox.yMax;
		}

		/* check that we really grew the string bbox */
		if (bbox.xMin > bbox.xMax)
		{
			bbox.xMin = 0;
			bbox.yMin = 0;
			bbox.xMax = 0;
			bbox.yMax = 0;
		}

		/* return string bbox */
		return bbox;
	}

	void RenderString(Graphics& gfx, Vec2& pos)
	{
		FT_BBox string_bbox = ComputeBounds();
		/* compute string dimensions in integer pixels */
		int string_width = string_bbox.xMax - string_bbox.xMin;
		int string_height = string_bbox.yMax - string_bbox.yMin;

		/* compute start pen position in 26.6 Cartesian pixels */
		int start_x = ((myTargetWidth - string_width) / 2) * 64;
		int start_y = ((myTargetHeight - string_height) / 2) * 64;

		for (int n = 0; n < num_glyphs; n++)
		{
			FT_Glyph   image;
			FT_Vector  pen;

			image = glyphs[n];

			pen.x = start_x + positions[n].x;
			pen.y = start_y + positions[n].y;

			if (FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, &pen, 0))
			{
				OutputDebugString(L"...Error rendering glyphs...\n");
			}
			else
			{
				FT_BitmapGlyph  bit = (FT_BitmapGlyph)image;

				Surface sfc{ &bit->bitmap, float(bit->left), float(myTargetHeight - bit->top) };
				gfx.DrawGlyph(pos.x + sfc.getPosOffset().x, pos.y + sfc.getPosOffset().y, sfc, gfx.GetScreenRect());

				FT_Done_Glyph(image);
			}
		
		}


		/////////////////////////////
		
	}


private:
	FreeType library; 
	FT_Face fontFace;
	static constexpr int maxChars = 1024;
	FT_UInt glyph_index = 0;
	float pen_x = 0;
	float pen_y = 0;
	int previous = 0;
	FT_Glyph glyphs[maxChars];
	FT_Vector positions[maxChars];
	FT_UInt num_glyphs = 0;
	FT_BBox* string_bbox;
	int myTargetWidth = 0;
	int myTargetHeight = 0;



};