#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftimage.h>
#include <freetype/ftmodapi.h>
#include <freetype/ftglyph.h>
#include <stdexcept>
class FreeType
{
public:
	FreeType()
	{
		if (FT_Init_FreeType(&factory) != 0)
		{
			throw std::runtime_error("Failed to initialize FreeType library\n");
		}
			
	}
	// Disable copying of FreeType class
	FreeType(FreeType const&) = delete;
	FreeType& operator=(FreeType const&) = delete;
	// Allow moving of FreeType class
	FreeType(FreeType&& other)noexcept
		:
		factory(other.factory)
	{
		// Set other.factory to nullptr so destructor of other won't free the FT_Library
		other.factory = nullptr;
	}
	FreeType& operator=(FreeType&& other) noexcept
	{
		// Disable self assignment
		if (this != &other)
		{
			// Release old factory if exists
			if (factory)
			{
				FT_Done_Library(factory);
			}

			factory = other.factory;
			other.factory = nullptr;
		}

		return *this;
	}

	~FreeType() noexcept
	{
		// Even if error, do not throw exceptions inside destructors.
		if (factory)
		{
			FT_Done_Library(factory);
		}
	}

	operator bool() const noexcept { return factory != nullptr; }

	FT_Library get() noexcept { return factory; }
	FT_Library get() const noexcept { return factory; }

private:
	FT_Library factory = nullptr;
};

