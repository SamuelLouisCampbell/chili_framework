/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
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
#pragma once

#include "ChiliWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Font.h"
#include "Surface.h"
#include "CoordinateTransformer.h"
#include "Entity.h"
#include "FreeType.h"
#include <Netstroker\IncludeMe.h>
#include "MyServer.h"
#include "FrameTimer.h"
#include <sstream>
#include "NDI_Send.h"


class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();

private:
	MainWindow& wnd;
	Graphics gfx;
    FrameTimer ft;
    Network ntwrk;
    IPEndpoint ip = { "127.0.0.1", 8000 };
    MyServer server;
    
    Font fpsCounterFont = { "Fonts/consola.ttf",20 };
    Vec2 fpsPos = { 20,20 };
    
    NDI_Send ndi;

    std::string fontName = "Fonts/arial.ttf";
    const size_t fontSize = 42;

    std::vector<std::string> strings;
    std::vector<std::unique_ptr<Font>> fonts;
    std::vector<Vec2> fontPositions;
    int counter = 0;


};