// $Id$

#include "SimpleScaler.hh"
#include "RenderSettings.hh"
#include "IntegerSetting.hh"
#include "HostCPU.hh"
#include "openmsx.hh"
#include <cassert>


namespace openmsx {

// Force template instantiation.
template class SimpleScaler<word>;
template class SimpleScaler<unsigned int>;

template <class Pixel>
SimpleScaler<Pixel>::SimpleScaler()
	: scanlineAlphaSetting(RenderSettings::instance().getScanlineAlpha())
{
	update(scanlineAlphaSetting);
	scanlineAlphaSetting->addListener(this);
}

template <class Pixel>
SimpleScaler<Pixel>::~SimpleScaler()
{
	scanlineAlphaSetting->removeListener(this);
}

template <class Pixel>
void SimpleScaler<Pixel>::update(const SettingLeafNode* setting)
{
	assert(setting == scanlineAlphaSetting);
	scanlineAlpha = (scanlineAlphaSetting->getValue() * 255) / 100;
}

// Upper 8 bits do not contain colours; use them as work area.
class HiFreeDarken {
public:
	static inline bool check(Uint32 rMask, Uint32 gMask, Uint32 bMask) {
		return ((rMask | gMask | bMask) & 0xFF000000) == 0;
	}
	static inline Uint32 darken(
		int darkenFactor,
		Uint32 rMask, Uint32 gMask, Uint32 bMask,
		Uint32 colour
		)
	{
		unsigned r = (((colour & rMask) * darkenFactor) >> 8) & rMask;
		unsigned g = (((colour & gMask) * darkenFactor) >> 8) & gMask;
		unsigned b = (((colour & bMask) * darkenFactor) >> 8) & bMask;
		return r | g | b;
	}
};

// Lower 8 bits do not contain colours; use them as work area.
class LoFreeDarken {
public:
	static inline bool check(Uint32 rMask, Uint32 gMask, Uint32 bMask) {
		return ((rMask | gMask | bMask) & 0x000000FF) == 0;
	}
	static inline Uint32 darken(
		int darkenFactor,
		Uint32 rMask, Uint32 gMask, Uint32 bMask,
		Uint32 colour
		)
	{
		unsigned r = (((colour & rMask) >> 8) * darkenFactor) & rMask;
		unsigned g = (((colour & gMask) >> 8) * darkenFactor) & gMask;
		unsigned b = (((colour & bMask) >> 8) * darkenFactor) & bMask;
		return r | g | b;
	}
};

// Uncommon pixel format; fall back to slightly slower routine.
class UniversalDarken {
public:
	static inline bool check(Uint32 rMask, Uint32 gMask, Uint32 bMask) {
		return true;
	}
	static inline Uint32 darken(
		int darkenFactor,
		Uint32 rMask, Uint32 gMask, Uint32 bMask,
		Uint32 colour
	) {
		Uint32 r =
			rMask & 0xFF
			? (((colour & rMask) * darkenFactor) >> 8) & rMask
			: (((colour & rMask) >> 8) * darkenFactor) & rMask;
		Uint32 g =
			gMask & 0xFF
			? (((colour & gMask) * darkenFactor) >> 8) & gMask
			: (((colour & gMask) >> 8) * darkenFactor) & gMask;
		Uint32 b =
			bMask & 0xFF
			? (((colour & bMask) * darkenFactor) >> 8) & bMask
			: (((colour & bMask) >> 8) * darkenFactor) & bMask;
		return r | g | b;
	}
};

template <class Pixel>
void SimpleScaler<Pixel>::scaleBlank(
	Pixel colour,
	SDL_Surface* dst, int dstY, int endDstY
) {
	if (colour == 0) {
		// No need to draw scanlines if border is black.
		// This is a special case that occurs very often.
		Scaler<Pixel>::scaleBlank(colour, dst, dstY, endDstY);
	} else {
		// Note: SDL_FillRect is generally not allowed on locked surfaces.
		//       However, we're using a software surface, which doesn't
		//       have locking.
		// TODO: But it would be more generic to just write bytes.
		assert(!SDL_MUSTLOCK(dst));

		SDL_PixelFormat* format = dst->format;
		Uint32 rMask = format->Rmask;
		Uint32 gMask = format->Gmask;
		Uint32 bMask = format->Bmask;
		int darkenFactor = 256 - scanlineAlpha;
		Pixel scanlineColour = UniversalDarken::darken(
			darkenFactor, rMask, gMask, bMask, colour );

		SDL_Rect rect;
		rect.x = 0;
		rect.w = dst->w;
		rect.h = 1;
		for (int y = dstY; y < endDstY; y += 2) {
			rect.y = y;
			// Note: return code ignored.
			SDL_FillRect(dst, &rect, colour);
			if (y + 1 == endDstY) break;
			rect.y = y + 1;
			// Note: return code ignored.
			SDL_FillRect(dst, &rect, scanlineColour);
		}
	}
}

static const bool ASM_NOSUCHMACHINE = false;

template <class Pixel>
void SimpleScaler<Pixel>::scale256(
	SDL_Surface* src, int srcY, int endSrcY,
	SDL_Surface* dst, int dstY )
{
	if (scanlineAlpha == 0) {
		Scaler<Pixel>::scale256(src, srcY, endSrcY, dst, dstY);
		return;
	}

	int darkenFactor = 256 - scanlineAlpha;
	const int WIDTH = dst->w / 2;
	assert(dst->w == WIDTH * 2);
	const HostCPU cpu = HostCPU::getInstance();
	while (srcY < endSrcY) {
		const Pixel* srcLine = Scaler<Pixel>::linePtr(src, srcY++);
		Pixel* dstUpper = Scaler<Pixel>::linePtr(dst, dstY++);
		Pixel* dstLower =
			dstY == dst->h ? dstUpper : Scaler<Pixel>::linePtr(dst, dstY++);
		/*if (ASM_X86 && cpu.hasMMXEXT() && sizeof(Pixel) == 2) {
			 TODO: Implement.
		} else*/ if (ASM_X86 && cpu.hasMMXEXT() && sizeof(Pixel) == 4) {
			asm (
				// Precalc: mm6 = darkenFactor, mm7 = 0.
				"movd	%[darkenFactor], %%mm6;"
				"pxor	%%mm7, %%mm7;"
				"punpcklwd	%%mm6, %%mm6;"
				"punpckldq	%%mm6, %%mm6;"
		
				// Upper line: scale, no scanline.
				// Note: Two separate loops is faster, probably because of
				//       linear memory access.
				"xorl	%%eax, %%eax;"
			"0:"
				// Load.
				"movq	(%[srcLine],%%eax,4), %%mm0;"
				"movq	%%mm0, %%mm1;"
				//"prefetchnta	128(%[srcLine],%%eax,4);"
				// Scale.
				"punpckldq %%mm0, %%mm0;"
				"punpckhdq %%mm1, %%mm1;"
				// Store.
				"movntq	%%mm0, (%[dstUpper],%%eax,8);"
				"movntq	%%mm1, 8(%[dstUpper],%%eax,8);"
				// Increment.
				"addl	$2, %%eax;"
				"cmpl	%[WIDTH], %%eax;"
				"jl	0b;"
		
				// Lower line: scale and scanline.
				"xorl	%%eax, %%eax;"
			"1:"
				// Load.
				"movq	(%[srcLine],%%eax,4), %%mm0;"
				"movq	%%mm0, %%mm1;"
				// Darken and scale.
				"punpcklbw %%mm7, %%mm0;"
				"punpckhbw %%mm7, %%mm1;"
				"pmullw	%%mm6, %%mm0;"
				"pmullw	%%mm6, %%mm1;"
				"psrlw	$8, %%mm0;"
				"psrlw	$8, %%mm1;"
				"packuswb %%mm0, %%mm0;"
				"packuswb %%mm1, %%mm1;"
				// Store.
				"movntq	%%mm0, (%[dstLower],%%eax,8);"
				"movntq	%%mm1, 8(%[dstLower],%%eax,8);"
				// Increment.
				"addl	$2, %%eax;"
				"cmpl	%[WIDTH], %%eax;"
				"jl	1b;"
		
				: // no output
				: [darkenFactor] "m" (darkenFactor)
				, [dstUpper] "r" (dstUpper)
				, [dstLower] "r" (dstLower)
				, [srcLine] "r" (srcLine)
				, [WIDTH] "r" (WIDTH)
				: "mm0", "mm1", "mm6", "mm7"
				, "eax"
				);
		// TODO: Test code, remove once we're satisfied all supported
		//       compilers skip code generation here.
		} else if (ASM_NOSUCHMACHINE) {
			asm ("nosuchinstruction");
		// End of test code.
		} else {
			unsigned rMask = dst->format->Rmask;
			unsigned gMask = dst->format->Gmask;
			unsigned bMask = dst->format->Bmask;
			for (int x = 0; x < WIDTH; x++) {
				Pixel p = srcLine[x];
				dstUpper[x * 2] = dstUpper[x * 2 + 1] = p;
				if (sizeof(Pixel) == 2) {
					dstLower[x * 2] = dstLower[x * 2 + 1] =
						HiFreeDarken::darken(
							darkenFactor, rMask, gMask, bMask, p );
				} else {
					// TODO: On my machine, HiFreeDarken is marginally
					//       faster, but is it worth the effort?
					dstLower[x * 2] = dstLower[x * 2 + 1] =
						UniversalDarken::darken(
							darkenFactor, rMask, gMask, bMask, p );
				}
			}
		}
	}
	if (ASM_X86 && cpu.hasMMXEXT()) {
		asm volatile ("emms");
	}
}

template <class Pixel>
void SimpleScaler<Pixel>::scale512(
	SDL_Surface* src, int srcY, int endSrcY,
	SDL_Surface* dst, int dstY )
{
	if (scanlineAlpha == 0) {
		Scaler<Pixel>::scale512(src, srcY, endSrcY, dst, dstY);
		return;
	}

	SDL_PixelFormat* format = dst->format;
	Uint32 rMask = format->Rmask;
	Uint32 gMask = format->Gmask;
	Uint32 bMask = format->Bmask;
	int darkenFactor = 256 - scanlineAlpha;
	const unsigned WIDTH = dst->w;
	while (srcY < endSrcY) {
		Scaler<Pixel>::copyLine(src, srcY, dst, dstY++);
		if (dstY == dst->h) break;
		const Pixel* srcLine = Scaler<Pixel>::linePtr(src, srcY);
		Pixel* dstLine = Scaler<Pixel>::linePtr(dst, dstY++);
		for (unsigned x = 0; x < WIDTH; x++) {
			dstLine[x] = UniversalDarken::darken(
				darkenFactor, rMask, gMask, bMask, srcLine[x] );
		}
		srcY++;
	}
}

} // namespace openmsx

