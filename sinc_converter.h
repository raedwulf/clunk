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

namespace clunk {

template <typename T = double, int Width = 128>
struct HannZeroWindow
{
	inline T operator()(T n)
	{
		return T(0.5) + T(0.5) * cos((2 * M_PI * n) / (Width - 1));
	}
};

template <typename T = double, int Width = 128, int Resolution = 32,
	 typename Window = HannZeroWindow<T, Width> >
struct SincKernel
{
	SincKernel()
	{
		table[0] = T(1.0);
		for (int i = 1; i < (Width/2) * Resolution; i++) {
			T n, x, nsinc;
			n = T(i) / T(Resolution);
			x = n * M_PI;
			nsinc = sin(x) / x;
			table[i] = nsinc * window(n);
		}
	}

	inline T operator()(T x)
	{
		// Linear interpolation, TODO: Something better...
		T fabsx = fabs(x);
		if (fabsx >= Width/2)
			return 0;
		T x1 = fabsx * T(Resolution);
		int x_ = x1;
		T delta = x1 - x_;
		// linearly interpolate the table entries
		T a = table[x_];
		T b = table[x_ + 1];
		return (a + (b - a) * delta);
	}

	T table[Width/2 * Resolution];
	Window window;
};

template <typename T = double, int WindowWidth = 128, typename Kernel = SincKernel<double, WindowWidth> >
class CLUNKAPI SincConverter : public Converter {
public:
	SincConverter(const int dst_rate, const Uint16 dst_format, const int src_rate,
		const Uint16 src_format, const Uint8 channels) {
		this->dst_rate = dst_rate;
		this->src_rate = src_rate;
		this->dst_format = dst_format;
		this->src_format = src_format;
		this->channels = channels;
	}
	void convert(clunk::Buffer &dst, const clunk::Buffer &src) {
		T ratio = T(dst_rate) / T(src_rate);
		T invratio = 1 / ratio;
		int samples = src.get_size() / (channels * 2);
		int dsamples = ratio * samples;
		dst.set_size(dsamples * (channels * 2));

		Sint16 *s = (Sint16 *)src.get_ptr();
		Sint16 *d = (Sint16 *)dst.get_ptr();
		int k = 0;
		int l = 0;

		if (invratio > 1.0) {
			int last_time;
			for (T t = 0; t < samples; t += invratio) {
				int i = t;

				// Window limits
				int left = t - WindowWidth/2 + 1;
				int right = t + WindowWidth/2;

				// Clamp window sides
				if (left < 0) left = 0;
				if (right >= samples) right = samples - 1;
				//printf("%g: %d %d\n", t, left, right);

				// Convolution over window with sinc
				for (int j = left; j < right; j++) {
					for (int c = 0; c < channels; c++) {
						Sint16 sv = 0;
						if (j >= 0 && j < samples)
							sv =  s[j * channels + c];
						d[k + c] += sv *
							ratio *
							kernel(ratio * (t - j));
					}
				}
				//printf("out: %d\n", d[k]);
				k += channels;
			}
		} else {
			for (T t = 0; t < samples; t += invratio) {
				int i = t;

				// Window limits
				int left = t - WindowWidth/2 + 1;
				int right = t + WindowWidth/2;

				// Clamp window sides
				if (left < 0) left = 0;
				if (right >= samples) right = samples - 1;

				// Convolution over window with sinc
				for (int j = left; j < right; j++) {
					for (int c = 0; c < channels; c++) {
						Sint16 sv = 0;
						if (j >= 0 && j < samples)
							sv =  s[j * channels + c];
						d[k + c] += sv * kernel(t - j);
					}
				}
				k += channels;
			}
		}
	}
private:
	int dst_rate, src_rate;
	Uint16 dst_format, src_format;
	Uint8 channels;
	Kernel kernel;
};

}

#endif

