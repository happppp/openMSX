// $Id$

/* PNG save code by Darren Grant sdl@lokigames.com */
/* heavily modified for openMSX by Joost Damad joost@lumatec.be */

#include "ScreenShotSaver.hh"
#include "CommandException.hh"
#include "build-info.hh"
#include "Version.hh"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <png.h>
#include <SDL.h>

namespace openmsx {
namespace ScreenShotSaver {

static bool IMG_SavePNG_RW(int width, int height, const void** row_pointers,
                           const std::string& filename, bool color)
{
	// initialize these before the goto
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	png_bytep* ptrs = reinterpret_cast<png_bytep*>(const_cast<void**>(row_pointers));

	FILE* fp = fopen(filename.c_str(), "wb");
	if (!fp) {
		return false;
	}
	png_infop info_ptr = 0;

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	        NULL, NULL, NULL);
	if (png_ptr == NULL) {
		// Couldn't allocate memory for PNG file
		goto error;
	}
	png_ptr->io_ptr = fp;

	// Allocate/initialize the image information data.  REQUIRED
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		// Couldn't create image information for PNG file
		goto error;
	}

	// Set error handling.
	if (setjmp(png_ptr->jmpbuf)) {
		// Error writing the PNG file
		goto error;
	}

	/* Set the image information here.  Width and height are up to 2^31,
	 * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
	 * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
	 * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
	 * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
	 * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
	 * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
	 */

	// WARNING Joost: for now always convert to 8bit/color (==24bit) image
	// because that is by far the easiest thing to do

	// First mark this image as being generated by openMSX and add creation time
	png_text text[2];
	text[0].compression = PNG_TEXT_COMPRESSION_NONE;
	text[0].key  = const_cast<char*>("Software");
	text[0].text = const_cast<char*>(Version::FULL_VERSION.c_str());
	text[1].compression = PNG_TEXT_COMPRESSION_NONE;
	text[1].key  = const_cast<char*>("Creation Time");
	char timeStr[10 + 1 + 8 + 1];
	snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d",
	         1900 + tm->tm_year, tm->tm_mon + 1, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec);
	text[1].text = timeStr;
	png_set_text(png_ptr, info_ptr, text, 2);

	png_set_IHDR(png_ptr, info_ptr, width, height, 8,
	             color ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY,
	             PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
	             PNG_FILTER_TYPE_BASE);

	// Write the file header information.  REQUIRED
	png_write_info(png_ptr, info_ptr);

	// write out the entire image data in one call
	png_write_image(png_ptr, ptrs);
	png_write_end(png_ptr, info_ptr);

	if (info_ptr->palette) {
		free(info_ptr->palette);
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
	return true;

error:
	if (info_ptr->palette) {
		free(info_ptr->palette);
	}
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
	return false;
}

void save(SDL_Surface* surface, const std::string& filename)
{
	SDL_PixelFormat frmt24;
	frmt24.palette = 0;
	frmt24.BitsPerPixel = 24;
	frmt24.BytesPerPixel = 3;
	frmt24.Rmask = OPENMSX_BIGENDIAN ? 0xFF0000 : 0x0000FF;
	frmt24.Gmask = 0x00FF00;
	frmt24.Bmask = OPENMSX_BIGENDIAN ? 0x0000FF : 0xFF0000;
	frmt24.Amask = 0;
	frmt24.Rshift = 0;
	frmt24.Gshift = 8;
	frmt24.Bshift = 16;
	frmt24.Ashift = 0;
	frmt24.Rloss = 0;
	frmt24.Gloss = 0;
	frmt24.Bloss = 0;
	frmt24.Aloss = 8;
	frmt24.colorkey = 0;
	frmt24.alpha = 0;
	SDL_Surface* surf24 = SDL_ConvertSurface(surface, &frmt24, 0);

	// Create the array of pointers to image data
	const void* row_pointers[surface->h];
	for (int i = 0; i < surface->h; ++i) {
		row_pointers[i] = static_cast<char*>(surf24->pixels) + (i * surf24->pitch);
	}

	bool result = IMG_SavePNG_RW(surface->w, surface->h,
	                             row_pointers, filename, true);

	SDL_FreeSurface(surf24);

	if (!result) {
		throw CommandException("Failed to write " + filename);
	}
}

void save(unsigned width, unsigned height, const void** rowPointers,
          const SDL_PixelFormat& format, const std::string& filename)
{
	// this implementation creates 1 extra copy, can be optimized if required
	SDL_Surface* surface = SDL_CreateRGBSurface(
		SDL_SWSURFACE, width, height, format.BitsPerPixel,
		format.Rmask, format.Gmask, format.Bmask, format.Amask);
	for (unsigned y = 0; y < height; ++y) {
		memcpy(static_cast<char*>(surface->pixels) + y * surface->pitch,
		       rowPointers[y], width * format.BytesPerPixel);
	}
	try {
		save(surface, filename);
		SDL_FreeSurface(surface);
	} catch (...) {
		SDL_FreeSurface(surface);
		throw;
	}
}

void save(unsigned width, unsigned height,
          const void** rowPointers, const std::string& filename)
{
	if (!IMG_SavePNG_RW(width, height, rowPointers, filename, true)) {
		throw CommandException("Failed to write " + filename);
	}
}

void saveGrayscale(unsigned width, unsigned height,
	           const void** rowPointers, const std::string& filename)
{
	if (!IMG_SavePNG_RW(width, height, rowPointers, filename, false)) {
		throw CommandException("Failed to write " + filename);
	}
}

} // namespace ScreenShotSaver
} // namespace openmsx
