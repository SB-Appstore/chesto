#include "Texture.hpp"

std::unordered_map<std::string, TextureData> Texture::texCache;

Texture::~Texture()
{
}

void Texture::clear(void)
{
	mTexture = nullptr;
	texW = 0;
	texH = 0;
	texFirstPixel = (CST_Color){0,0,0,0};
}

bool Texture::loadFromSurface(CST_Surface *surface)
{
	if (!surface)
		return false;

	// will default MainDisplay's renderer if we don't have one in this->renderer
	CST_Renderer* renderer = getRenderer();

	// try to create a texture from the surface
	CST_Texture *texture = CST_CreateTextureFromSurface(renderer, surface);
	if (!texture)
		return false;

	// load first pixel color
	Uint32 pixelcolor = *(Uint32*)surface->pixels;
	Uint32 emptybits = 8 * (4 - surface->format->BytesPerPixel);
	pixelcolor >>= emptybits * (SDL_BYTEORDER == SDL_BIG_ENDIAN);
	pixelcolor &= 0xffffffff >> emptybits;
	SDL_GetRGB(pixelcolor, surface->format, &texFirstPixel.r, &texFirstPixel.g, &texFirstPixel.b);

	// load texture size
	CST_QueryTexture(texture, &texW, &texH);

	// load texture
	mTexture = texture;

	return true;
}

bool Texture::loadFromCache(std::string &key)
{
	// check if the texture is cached
	if (texCache.count(key))
	{
		TextureData *texData = &texCache[key];
		mTexture = texData->texture;
		texFirstPixel = texData->firstPixel;
		CST_QueryTexture(mTexture, &texW, &texH);
		return true;
	}

	return false;
}

bool Texture::loadFromSurfaceSaveToCache(std::string &key, CST_Surface *surface)
{
	bool success = loadFromSurface(surface);

	// only save to caches if loading was successful
	// and the texture isn't already cached
	if (success && !texCache.count(key))
	{
		TextureData texData;
		texData.texture = mTexture;
		texData.firstPixel = texFirstPixel;
		texCache[key] = texData;
	}

	return success;
}

void Texture::render(Element* parent)
{
	if (!mTexture)
		return;

	if (hidden)
		return;

	// update xAbs and yAbs
	super::render(parent);

	// rect of element's size
	CST_Rect rect;
	rect.x = this->xAbs;
	rect.y = this->yAbs;
	rect.w = this->width;
	rect.h = this->height;

	if (CST_isRectOffscreen(&rect))
		return;

	CST_Renderer* renderer = getRenderer();

	if (texScaleMode == SCALE_PROPORTIONAL_WITH_BG)
	{
		// draw colored background
		CST_SetDrawColor(renderer, texFirstPixel);
		CST_FillRect(renderer, &rect);

		// recompute drawing rect
		if ((width * texH) > (height * texW))
		{
			// keep height, scale width
			rect.h = height;
			rect.w = (texW * rect.h) / texH;
		}
		else
		{
			// keep width, scale height
			rect.w = width;
			rect.h = (texH * rect.w) / texW;
		}

		// center the texture
		rect.x += (width - rect.w) / 2;
		rect.y += (height - rect.h) / 2;
	}

	if (angle!=0)
	{
		// render the texture with a rotation
		CST_SetQualityHint("best");
		CST_RenderCopyRotate(renderer, mTexture, NULL, &rect, this->angle);
	}
	else
	{
		// render the texture normally
		CST_RenderCopy(renderer, mTexture, NULL, &rect);
	}
}

void Texture::resize(int w, int h)
{
	width = w;
	height = h;
}

Texture* Texture::setSize(int w, int h)
{
	this->resize(w, h);
	return this;
}

void Texture::setScaleMode(TextureScaleMode mode)
{
	texScaleMode = mode;
}

void Texture::getTextureSize(int *w, int *h)
{
	if (w)
		*w = texW;
	if (h)
		*h = texH;
}
