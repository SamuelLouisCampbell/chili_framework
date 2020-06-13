#pragma once
#include "ChiliWin.h"
#include "Colors.h"
#include <memory>
#include <assert.h>
#include <string>
#include <freetype/ftimage.h>

class Surface
{
public:
	Surface(const FT_Bitmap descriptor);
	Surface(const std::string& filename);
	Surface(int width, int height);
	Surface(const Surface& rhs);
	~Surface();
	Surface& operator= (const Surface& rhs);
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	int GetWidth() const; 
	int GetHeight() const; 

private:
	std::unique_ptr<Color[]> pPixels;
	int width;
	int height; 
};

