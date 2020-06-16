#include "Surface.h"
#include <fstream>


Surface::Surface(const FT_Bitmap* ftbmp, const float penX, const float penY)
{
	posOffset = { penX,penY };
	width = ftbmp->width;
	height = ftbmp->rows;
	
	pPixels = std::make_unique<Color[]>(width * height);

	assert(ftbmp->pixel_mode == FT_PIXEL_MODE_GRAY);

	unsigned char* p = ftbmp->buffer;
	for (int y = 0; y < height; y++)
	{
		
		for (int x = 0; x < width ; x++)
		{
			Color c = { *p,*p,*p };
			PutPixel(x, y, c);
			p++;
		}
	}
}


Surface::Surface(const std::string& filename) //load a bitmap from file and create surface.
{
	std::ifstream file(filename, std::ios::binary);
	assert(file); //file error

	BITMAPFILEHEADER bmFileheader; //Microsoft struct
	file.read(reinterpret_cast<char*>(&bmFileheader),sizeof(bmFileheader));
	BITMAPINFOHEADER bmInfoheader; //Microsoft struct
	file.read(reinterpret_cast<char*>(&bmInfoheader), sizeof(bmInfoheader));

	assert(bmInfoheader.biBitCount == 24); //bitdepth error
	assert(bmInfoheader.biCompression == BI_RGB); //no compression

	width = bmInfoheader.biWidth;
	height = bmInfoheader.biHeight;

	pPixels = std::make_unique<Color[]>(width * height);

	file.seekg(bmFileheader.bfOffBits);
	const int padding = (4 - (width *3) % 4) % 4;
	if (height < 0)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				PutPixel(x, y, Color(file.get(), file.get(), file.get()));

			}
			file.seekg(padding, std::ios::cur);
		}
	}
	else
	{
		for (int y = height -1; y > 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				PutPixel(x, y, Color(file.get(), file.get(), file.get()));

			}
			file.seekg(padding, std::ios::cur);
		}
	}



}

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	pPixels(std::make_unique<Color[]>(width* height))
{
}

Surface::Surface(const Surface& rhs)
	:
	Surface(rhs.width, rhs.height)
{
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}
}

Surface::~Surface()
{
}

Surface& Surface::operator=(const Surface& rhs)
{
	width = rhs.width;
	height = rhs.height;
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}
	return *this;
}

void Surface::PutPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	pPixels[y * width + x] = c;
}

Color Surface::GetPixel(int x, int y) const
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	return pPixels[y * width + x];
}

int Surface::GetWidth() const
{
	return width;
}

int Surface::GetHeight() const
{
	return height;
}

Vec2 Surface::getPosOffset() const
{
	return posOffset;
}
