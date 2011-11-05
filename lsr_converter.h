#ifndef CLUNK_LSR_CONVERTER_H__
#define CLUNK_LSR_CONVERTER_H__

/* libClunk - cross-platform 3D audio API built on top LSR library
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
#ifndef WITH_LIBSAMPLERATE
#error "libsamplerate support not compiled in."
#endif

#include <export_clunk.h>
#include <samplerate.h>
#include <cassert>
#include "converter.h"

#define BUFFERED_FRAMES 4096

namespace clunk {

//!Audio converter used in context.
class CLUNKAPI LSRConverter : public Converter {
public:
	LSRConverter(const int dst_rate, const Uint16 dst_format, const int src_rate,
	             const Uint16 src_format, const Uint8 channels) {
		src = src_new(SRC_SINC_BEST_QUALITY, channels, &error);
		this->dst_rate = dst_rate;
		this->src_rate = src_rate;
		this->channels = channels;
	}
	void convert(clunk::Buffer &dst, const clunk::Buffer &src) {
		float input[BUFFERED_FRAMES];
		float output[BUFFERED_FRAMES];
		int total_frames = src.get_size() / (2 * channels);
		int frames_processed = 0;
		int buffer_frames;
		short *s, *d;
		s = (short *)src.get_ptr();
		
		data.src_ratio = this->dst_rate / this->src_rate;
		//printf("ratio: %g\n", data.src_ratio);
		data.end_of_input = 0;
		data.data_in = input;
		data.data_out = output;
		
		dst.set_size(data.src_ratio * src.get_size());
		d = (short *)dst.get_ptr();
		buffer_frames = std::min((double)BUFFERED_FRAMES, (1 / data.src_ratio) * (BUFFERED_FRAMES / channels));
		//printf("buffer_frames: %d\n", buffer_frames);
		while (frames_processed < total_frames) {
			if (total_frames - frames_processed < buffer_frames) {
				buffer_frames = total_frames - frames_processed;
				data.end_of_input = 1;
			}
			data.input_frames = buffer_frames;
			data.output_frames = BUFFERED_FRAMES / channels;
			//printf("input: %ld, output: %ld\n", data.input_frames, data.output_frames);
			src_short_to_float_array(s, data.data_in, buffer_frames);
			if ((error = src_process(this->src, &data))) {
				fprintf(stderr, "%s\n", src_strerror(error));
				return;
			}
			src_float_to_short_array(data.data_out, d, data.output_frames_gen);
			//printf("used: %ld, gen: %ld\n", data.input_frames_used, data.output_frames_gen);
			frames_processed += data.input_frames_used;
			s += (data.input_frames_used * channels);
			d += (data.output_frames_gen * channels);
		}
	}
	~LSRConverter() {
		src_delete(src);
	}
private:
	double dst_rate, src_rate;
	int channels;
	SRC_STATE *src;
	SRC_DATA data;
	int error;
};

}

#endif

