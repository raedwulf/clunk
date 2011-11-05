#ifndef CLUNK_SDL_CONVERTER_H__
#define CLUNK_SDL_CONVERTER_H__

/* libClunk - cross-platform 3D audio API built on top SDL library
 * Copyright (C) 2007-2008 Netive Media Group
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "config.h"
#include "converter.h"
#include "sdl_ex.h"
#include <SDL.h>
#include <SDL_audio.h>
#include <cassert>

namespace clunk {

//!Audio converter used in context.

class CLUNKAPI SDLConverter : public Converter {
public:
	SDLConverter(const int dst_rate, const Uint16 dst_format, const int src_rate,
	             const Uint16 src_format, const Uint8 channels) {
		memset(&cvt, 0, sizeof(cvt));
		if (SDL_BuildAudioCVT(&cvt, src_format, channels, src_rate, dst_format, channels, dst_rate) == -1) {
			throw_sdl(("DL_BuildAudioCVT(%d, %04x, %u)", src_rate, src_format, channels));
		}
	}
	void convert(clunk::Buffer &dst, const clunk::Buffer &src) {
		size_t buf_size = (size_t)(src.get_size() * cvt.len_mult);
		cvt.buf = (Uint8 *)malloc(buf_size);
		cvt.len = (int)src.get_size();

		assert(buf_size >= src.get_size());
		memcpy(cvt.buf, src.get_ptr(), src.get_size());

		if (SDL_ConvertAudio(&cvt) == -1) 
			throw_sdl(("SDL_ConvertAudio"));

		dst.set_data(cvt.buf, (size_t)(cvt.len * cvt.len_ratio), true);
	}
private:
	SDL_AudioCVT cvt;
};

}

#endif

