#include "Font.h"

int Font::GetNumGlyphs() const
{
	return fontFace->num_glyphs;
}

void Font::GetGlyph(unsigned int code) const
{
	int glyph_index = FT_Get_Char_Index(fontFace, code);
	FT_Load_Glyph(
		fontFace,
		glyph_index,
		0
	);
	FT_Render_Glyph(fontFace->glyph, FT_Render_Mode_::FT_RENDER_MODE_NORMAL);
}

void Font::Init()
{
	auto error0 = FT_Init_FreeType(&library);
	if (error0)
	{
		EXIT_FAILURE;
	}
	auto error1 = FT_New_Face(library, "Fonts/Helvetica.ttf", 0, &fontFace);
	if(error1)
	{
		{
			EXIT_FAILURE;
		}
	}
	//Set Size of character
	auto error2 = FT_Set_Char_Size(
		fontFace, //handle to face object
		0, //char width 0 = same as height
		16 * 64, //point size
		1920, // screen output dimensions
		1080
	);
	if (error2)
	{
		{
			EXIT_FAILURE;
		}
	}
	
	

}
