/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "Vec2.h"
#include <ft2build.h>
#include FT_FREETYPE_H


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
    font("Fonts/Helvetica.ttf", library, 32)
{ 
 
   /* 
    if (int errorFTlib = FT_Init_FreeType(&library))
    {
        OutputDebugString(L"... an error occurred during library initialization ...\n");
    }

    int errorFTface = FT_New_Face(library, "Fonts/Helvetica.ttf", 0, &face);
    if (errorFTface == FT_Err_Unknown_File_Format)
    {
        OutputDebugString(L"... the font file could be openedand read, but it appears that its font format is unsupported\n");
    }
    else if (errorFTface)
    {
        OutputDebugString(L"... the font file could not be opened or read, or it is broken...\n");
    }

    if (FT_Set_Char_Size(face,  0, 80 * 64, 800, 600))
    {
        OutputDebugString(L"...Could not set font size...\n");
    }
    if (FT_Set_Pixel_Sizes(face, 0, 80))
    {
        OutputDebugString(L"...Could not set pixel size...\n");
    }*/
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
  
}

void Game::ComposeFrame()
{
    std::string str = "Hello!";
    font.RenderString(gfx, fontPos, str);

}
