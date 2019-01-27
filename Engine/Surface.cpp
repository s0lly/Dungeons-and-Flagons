/******************************************************************************************
*	Game code and amendments by s0lly													   *
*	https://www.youtube.com/channel/UCOyCXEB8NTx3Xjm8UptwsIg							   *
*	https://s0lly.itch.io/																   *
*	https://www.instagram.com/s0lly.gaming/												   *
******************************************************************************************/

#include "Surface.h"
#include "ChiliWin.h"
#include <cassert>
#include <fstream>

Surface::Surface(const std::string & filename)
{
	std::ifstream file(filename, std::ios::binary);
	assert(file);

	BITMAPFILEHEADER bmFileHeader;
	file.read(reinterpret_cast<char*>(&bmFileHeader), sizeof(bmFileHeader));

	BITMAPINFOHEADER bmInfoHeader;
	file.read(reinterpret_cast<char*>(&bmInfoHeader), sizeof(bmInfoHeader));

	assert(bmInfoHeader.biBitCount == 24 || bmInfoHeader.biBitCount == 32);
	assert(bmInfoHeader.biCompression == BI_RGB);

	width = bmInfoHeader.biWidth;
	height = bmInfoHeader.biHeight;

	bool heightReverse = (height < 0);

	height = abs(height);

	pPixels = new Color[width*height];

	file.seekg(bmFileHeader.bfOffBits);
	const int padding = (4 - (width * 3) % 4) % 4;

	int colCounter = 0;

	for (int y = 0; y < height; y++)
	{
		if (!heightReverse)
		{
			colCounter = height - 1 - y;
		}
		else
		{
			colCounter = y;
		}

		for (int x = 0; x < width; x++)
		{
			//pPixels[y * width + x].SetR(file.get());
			//pPixels[y * width + x].SetG(file.get());
			//pPixels[y * width + x].SetB(file.get());
			Color c = Color(file.get(), file.get(), file.get());

			if (bmInfoHeader.biBitCount == 32)
			{
				file.seekg(1, std::ios::cur);
			}
			
			PutPixel(x, colCounter, c);
		}

		if (bmInfoHeader.biBitCount == 24)
		{
			file.seekg(padding, std::ios::cur);
		}
	}

}

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	pPixels(new Color[width*height])
{
}

Surface::Surface(const Surface& rhs)
	:
	Surface(rhs.width,rhs.height)
{
	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}
}

Surface::~Surface()
{
	delete[] pPixels;
	pPixels = nullptr;
}

Surface & Surface::operator=(const Surface& rhs)
{
	width = rhs.width;
	height = rhs.height;

	delete[] pPixels;
	pPixels = new Color[width*height];

	const int nPixels = width * height;
	for (int i = 0; i < nPixels; i++)
	{
		pPixels[i] = rhs.pPixels[i];
	}

	return *this;
}

void Surface::PutPixel(int x, int y, Color c)
{
	//assert(x >= 0);
	//assert(x < width);
	//assert(y >= 0);
	//assert(y < height);
	pPixels[y * width + x] = c;
}

Color Surface::GetPixel(int x, int y) const
{
	//assert(x >= 0);
	//assert(x < width);
	//assert(y >= 0);
	//assert(y < height);
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

RectI Surface::GetRect() const
{
	return { 0, width, 0, height };
}

void Surface::DrawSprite(int x, int y, RectI srcRect, const RectI & clip, const Surface & s, Color chroma)
{
	assert(srcRect.left >= 0);
	assert(srcRect.right <= s.GetWidth());
	assert(srcRect.top >= 0);
	assert(srcRect.bottom <= s.GetHeight());

	if (x < clip.left)
	{
		srcRect.left += clip.left - x;
		x = clip.left;
	}
	if (x + srcRect.GetWidth() > clip.right)
	{
		srcRect.right -= x + srcRect.GetWidth() - clip.right;
	}

	if (y < clip.top)
	{
		srcRect.top += clip.top - y;
		y = clip.top;
	}
	if (y + srcRect.GetHeight() > clip.bottom)
	{
		srcRect.bottom -= y + srcRect.GetHeight() - clip.bottom;
	}

	for (int sy = srcRect.top; sy < srcRect.bottom; sy++)
	{
		for (int sx = srcRect.left; sx < srcRect.right; sx++)
		{
			Color srcPixel = s.GetPixel(sx, sy);
			if (srcPixel != chroma)
			{
				PutPixel(x + sx - srcRect.left, y + sy - srcRect.top, s.GetPixel(sx, sy));
			}
		}
	}
}

void Surface::DrawSpriteScaledUpRotated(int x, int y, RectI srcRect, const RectI & clip, const Surface & s, int scale, MOVETYPE direction, Color chroma)
{
	if (x + srcRect.GetWidth() * scale >= clip.left && x < clip.right
		&& y + srcRect.GetHeight() * scale >= clip.top && y < clip.bottom)
	{
		for (int sy = srcRect.top; sy < srcRect.bottom; sy++)
		{
			for (int sx = srcRect.left; sx < srcRect.right; sx++)
			{
				Color srcPixel = s.GetPixel(sx, sy);
				if (srcPixel != chroma)
				{
					for (int scaleRow = 0; scaleRow < scale; scaleRow++)
					{
						for (int scaleCol = 0; scaleCol < scale; scaleCol++)
						{
							switch (direction)
							{
							case MOVETYPE::NONE:
							case MOVETYPE::UP:
								if (x + (sx - srcRect.left) * scale + scaleCol >= clip.left && x + (sx - srcRect.left) * scale + scaleCol < clip.right
									&& y + (sy - srcRect.top) * scale + scaleRow >= clip.top && y + (sy - srcRect.top) * scale + scaleRow < clip.bottom)
								{
									PutPixel(x + (sx - srcRect.left) * scale + scaleCol, y + (sy - srcRect.top) * scale + scaleRow, s.GetPixel(sx, sy));
								}
								break;
							case MOVETYPE::DOWN:
								if (x + (srcRect.right - (sx)-1) * scale + scaleCol >= clip.left && x + (srcRect.right - (sx)-1) * scale + scaleCol < clip.right
									&& y + (srcRect.bottom - (sy)-1) * scale + scaleRow >= clip.top && y + (srcRect.bottom - (sy)-1) * scale + scaleRow < clip.bottom)
								{
									PutPixel(x + (srcRect.right - (sx)-1) * scale + scaleCol, y + (srcRect.bottom - (sy)-1) * scale + scaleRow, s.GetPixel(sx, sy));
								}
								break;
							case MOVETYPE::LEFT:
								if (x + (sy - srcRect.top) * scale + scaleRow >= clip.left && x + (sy - srcRect.top) * scale + scaleRow < clip.right
									&& y + (srcRect.right - (sx)-1) * scale + scaleCol >= clip.top && y + (srcRect.right - (sx)-1) * scale + scaleCol < clip.bottom)
								{
									PutPixel(x + (sy - srcRect.top) * scale + scaleRow, y + (srcRect.right - (sx)-1) * scale + scaleCol, s.GetPixel(sx, sy));
								}
								break;
							case MOVETYPE::RIGHT:
								if (x + (srcRect.bottom - (sy)-1) * scale + scaleRow >= clip.left && x + (srcRect.bottom - (sy)-1) * scale + scaleRow < clip.right
									&& y + (sx - srcRect.left) * scale + scaleCol >= clip.top && y + (sx - srcRect.left) * scale + scaleCol < clip.bottom)
								{
									PutPixel(x + (srcRect.bottom - (sy)-1) * scale + scaleRow, y + (sx - srcRect.left) * scale + scaleCol, s.GetPixel(sx, sy));
								}
								break;
							}
						}
					}
				}
			}
		}
	}
}

void Surface::AddFOV(Surface & cameraSurface, std::vector<bool>& pixelFOVToggle)
{
	for (int y = 0; y < cameraSurface.GetHeight(); y++)
	{
		for (int x = 0; x < cameraSurface.GetWidth(); x++)
		{
			if (pixelFOVToggle[y * cameraSurface.GetWidth() + x])
			{
				PutPixel(x, y, cameraSurface.GetPixel(x, y));
			}
			else
			{
				PutPixel(x, y, Colors::Black);
			}
		}
	}

}
