#ifndef CLUNK_SINC_CONVERTER_H__
#define CLUNK_SINC_CONVERTER_H__

/* libClunk - cross-platform 3D audio API
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

// By Tai Chi Minh Ralph Eastwood

#include "config.h"
#include "converter.h"
#include <cassert>
#include <cmath>

// Precalculate sinc table
#define SINC_PRECALCULATE
// controls the number of neighbouring samples which are used to interpolate
// the new samples.  The processing time is linear related to this width.
#define SINC_WIDTH 128
// how finely tuned the sinc function is stored in the table
#define SINC_SAMPLES_PER_ZERO_CROSSING 32
// ditto... double version
#define SINC_SAMPLES_PER_ZERO_CROSSING_DBL ((double)SINC_SAMPLES_PER_ZERO_CROSSING)

#define SINC_WIN_FREQ (M_PI / (SINC_WIDTH/2) / SINC_SAMPLES_PER_ZERO_CROSSING)

namespace clunk {

/*template <typename T>*/
//struct HannWindow
//{
	//operator
//}

//template <typename window_function = HannWindow<T>, typename T = double>
//struct SincKernel
//{
	//operator (double x)
	//{
	//}
//};

/*template <typename resampling_kernel>*/

class CLUNKAPI SincConverter : public Converter {
public:
	SincConverter(const int dst_rate, const Uint16 dst_format, const int src_rate,
		const Uint16 src_format, const Uint8 channels) {
#ifdef SINC_PRECALCULATE
		// Create the sinc table
		for (int i = 0; i < (SINC_WIDTH/2) *
			SINC_SAMPLES_PER_ZERO_CROSSING; i++) {
			double x = i * M_PI / SINC_SAMPLES_PER_ZERO_CROSSING;
			double sinmxinv = x == 0.0 ? 1.0 : sin(x) / x;
			// TODO: Use kaiser windowing function here...
			sinc_table[i] = sinmxinv * (0.5 + 0.5 * cos(SINC_WIN_FREQ * i));
		}
#endif
		// Converter parameters
		this->dst_rate = dst_rate;
		this->src_rate = src_rate;
		this->channels = channels;
	}
	void convert(clunk::Buffer &dst, const clunk::Buffer &src) {
		double ratio = dst_rate / src_rate;
		double invratio = 1 / ratio;
		int samples = src.get_size() / (channels * 2);
		int dsamples = ratio * samples;
		dst.set_size(dsamples * (channels * 2));

		int16_t *s = (int16_t *)src.get_ptr();
		int16_t *d = (int16_t *)dst.get_ptr();
		int k = 0;
		int l = 0;

		if (invratio > 1.0) {
			int last_time;
			for (double t = 0; t < samples; t += invratio) {
				int i = t;

				// Window limits
				int left = t - SINC_WIDTH/2 + 1;
				int right = t + SINC_WIDTH/2;

				// Clamp window sides
				//if (left < 0) left = 0;
				//if (right >= samples) right = samples - 1;
				//printf("%g: %d %d\n", t, left, right);

				// Convolution over window with sinc
				for (int j = left; j < right; j++) {
					for (int c = 0; c < channels; c++) {
						int16_t sv = 0;
						if (j >= 0 && j < samples)
							sv =  s[j * channels + c];
						d[k + c] += sv *
							ratio *
							sinc(ratio * (t - j));
					}
				}
				//printf("out: %d\n", d[k]);
				k += channels;
			}
		} else {
			for (double t = 0; t < samples; t += invratio) {
				int i = t;

				// Window limits
				int left = t - SINC_WIDTH/2 + 1;
				int right = t + SINC_WIDTH/2;

				// Clamp window sides
				//if (left < 0) left = 0;
				//if (right >= samples) right = samples - 1;

				// Convolution over window with sinc
				for (int j = left; j < right; j++) {
					for (int c = 0; c < channels; c++) {
						int16_t sv = 0;
						if (j >= 0 && j < samples)
							sv =  s[j * channels + c];
						d[k + c] += sv * sinc(t - j);
					}
				}
				k += channels;
			}
		}
	}
private:
	inline double sinc(double x) {
#ifdef SINC_PRECALCULATE
		double fabsx = fabs(x);
		if (fabsx > SINC_WIDTH/2 - 1)
			return 0;
		double x1 = fabsx * SINC_SAMPLES_PER_ZERO_CROSSING_DBL;
		int x_ = x1;
		double delta = x1 - x_;
		// linearly interpolate the table entries
		double a = sinc_table[x_];
		double b = sinc_table[x_ + 1];
		return (a + (b - a) * delta);
#else
		return x == 0.0 ? 1.0 : sin(x * M_PI) / (x * M_PI);
#endif
	}
	double dst_rate, src_rate;
	int channels;
#ifdef SINC_PRECALCULATE
	double sinc_table[SINC_WIDTH/2 * SINC_SAMPLES_PER_ZERO_CROSSING];
#endif
};

}

#endif

