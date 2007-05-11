// $Id$

// Based on libsamplerate-0.1.2 (aka Secret Rabit Code)
//
//  simplified code in several ways:
//   - resample algorithm is no longer switchable, we took this variant:
//        Band limited sinc interpolation, fastest, 97dB SNR, 80% BW
//   - only handle a single channel (mono)
//   - don't allow to change sample rate on-the-fly
//   - assume input (and thus also output) signals have infinte length, so
//     there is no special code to handle the ending of the signal
//   - changed/simplified API to better match openmsx use model
//     (e.g. remove all error checking)


#include "Resample.hh"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace openmsx {

static const float coeffs[] = {
	#include "ResampleCoeffs.ii"
};
static const int INDEX_INC = 128;
static const int COEFF_LEN = sizeof(coeffs) / sizeof(float);
static const int COEFF_HALF_LEN = COEFF_LEN - 1;
static const unsigned TAB_LEN = 4096;

template <unsigned CHANNELS>
Resample<CHANNELS>::Resample()
	: increment(0)
{
	ratio = 1.0f;
	lastPos = 0.0f;
	bufCurrent = BUF_LEN / 2;
	bufEnd     = BUF_LEN / 2;
	memset(buffer, 0, sizeof(buffer));
	nonzeroSamples = 0;
}

template <unsigned CHANNELS>
Resample<CHANNELS>::~Resample()
{
}

template <unsigned CHANNELS>
void Resample<CHANNELS>::setResampleRatio(double inFreq, double outFreq)
{
	ratio = inFreq / outFreq;
	bufCurrent = BUF_LEN / 2;
	bufEnd     = BUF_LEN / 2;
	memset(buffer, 0, sizeof(buffer));

	// check the sample rate ratio wrt the buffer len
	double count = (COEFF_HALF_LEN + 2.0) / INDEX_INC;
	if (ratio > 1.0f) {
		count *= ratio;
	}
	// maximum coefficients on either side of center point
	halfFilterLen = lrint(count) + 1;

	floatIncr = (ratio > 1.0f) ? INDEX_INC / ratio : INDEX_INC;
	normFactor = floatIncr / INDEX_INC;
	increment = FilterIndex(floatIncr);

	calculateCoeffs();
}

template <unsigned CHANNELS>
double Resample<CHANNELS>::getCoeff(FilterIndex index)
{
	double fraction = index.fractionAsDouble();
	int indx = index.toInt();
	return coeffs[indx] + fraction * (coeffs[indx + 1] - coeffs[indx]);
}

template <unsigned CHANNELS>
void Resample<CHANNELS>::calculateCoeffs()
{
	FilterIndex maxFilterIndex(COEFF_HALF_LEN);
	int min_idx = -maxFilterIndex.divAsInt(increment);
	int max_idx = 1 + (maxFilterIndex - (increment - FilterIndex(floatIncr))).divAsInt(increment);
	int idx_cnt = max_idx - min_idx + 1;
	filterLen = (idx_cnt + 3) & ~3; // round up to multiple of 4
	min_idx -= (filterLen - idx_cnt);
	table.resize(TAB_LEN * filterLen);

	for (unsigned t = 0; t < TAB_LEN; ++t) {
		double lastPos = double(t) / TAB_LEN;
		FilterIndex startFilterIndex(lastPos * floatIncr);

		FilterIndex filterIndex(startFilterIndex);
		int coeffCount = (maxFilterIndex - filterIndex).divAsInt(increment);
		filterIndex += increment * coeffCount;
		int bufIndex = -coeffCount;
		do {
			table[t * filterLen + bufIndex - min_idx] =
				getCoeff(filterIndex) * normFactor;
			filterIndex -= increment;
			bufIndex += 1;
		} while (filterIndex >= FilterIndex(0));

		filterIndex = increment - startFilterIndex;
		coeffCount = (maxFilterIndex - filterIndex).divAsInt(increment);
		filterIndex += increment * coeffCount;
		bufIndex = 1 + coeffCount;
		do {
			table[t * filterLen + bufIndex - min_idx] =
				getCoeff(filterIndex) * normFactor;
			filterIndex -= increment;
			bufIndex -= 1;
		} while (filterIndex > FilterIndex(0));
	}
}

template <unsigned CHANNELS>
void Resample<CHANNELS>::calcOutput2(float lastPos, float* output)
{
	int t = static_cast<int>(lastPos * TAB_LEN + 0.5f);

	for (unsigned ch = 0; ch < CHANNELS; ++ch) {
		float r = 0.0f;
		for (unsigned i = 0; i < filterLen; ++i) {
			r += table[t * filterLen + i] * buffer[bufCurrent - halfFilterLen + i];
		}
		output[ch] = r;
	}
}

template <unsigned CHANNELS>
void Resample<CHANNELS>::calcOutput(FilterIndex startFilterIndex, float* output)
{
	FilterIndex maxFilterIndex(COEFF_HALF_LEN);

	// apply the left half of the filter
	FilterIndex filterIndex(startFilterIndex);
	int coeffCount = (maxFilterIndex - filterIndex).divAsInt(increment);
	filterIndex += increment * coeffCount;
	int bufIndex = (bufCurrent - coeffCount) * CHANNELS;

	float left[CHANNELS];
	for (unsigned i = 0; i < CHANNELS; ++i) {
		left[i] = 0.0f;
	}
	do {
		float fraction = filterIndex.fractionAsFloat();
		int indx = filterIndex.toInt();
		float icoeff = coeffs[indx] +
		                fraction * (coeffs[indx + 1] - coeffs[indx]);
		for (unsigned i = 0; i < CHANNELS; ++i) {
			left[i] += icoeff * buffer[bufIndex + i];
		}
		filterIndex -= increment;
		bufIndex += CHANNELS;
	} while (filterIndex >= FilterIndex(0));

	// apply the right half of the filter
	filterIndex = increment - startFilterIndex;
	coeffCount = (maxFilterIndex - filterIndex).divAsInt(increment);
	filterIndex += increment * coeffCount;
	bufIndex = (bufCurrent + (1 + coeffCount)) * CHANNELS;

	float right[CHANNELS];
	for (unsigned i = 0; i < CHANNELS; ++i) {
		right[i] = 0.0f;
	}
	do {
		float fraction = filterIndex.fractionAsFloat();
		int indx = filterIndex.toInt();
		float icoeff = coeffs[indx] +
		                fraction * (coeffs[indx + 1] - coeffs[indx]);
		for (unsigned i = 0; i < CHANNELS; ++i) {
			right[i] += icoeff * buffer[bufIndex + i];
		}
		filterIndex -= increment;
		bufIndex -= CHANNELS;
	} while (filterIndex > FilterIndex(0));

	for (unsigned i = 0; i < CHANNELS; ++i) {
		output[i] = (left[i] + right[i]) * normFactor;
	}
}

template <unsigned CHANNELS>
void Resample<CHANNELS>::prepareData(unsigned extra)
{
	assert(bufCurrent <= bufEnd);
	assert(bufEnd <= BUF_LEN);
	assert(halfFilterLen <= bufCurrent);

	unsigned available = bufEnd - bufCurrent;
	unsigned request = halfFilterLen + extra;
	int missing = request - available;
	assert(missing > 0);

	unsigned free = BUF_LEN - bufEnd;
	int overflow = missing - free;
	if (overflow > 0) {
		// close to end, restart at begin
		memmove(buffer,
			buffer + (bufCurrent - halfFilterLen) * CHANNELS,
			(halfFilterLen + available) * sizeof(float) * CHANNELS);
		bufCurrent = halfFilterLen;
		bufEnd = halfFilterLen + available;
		missing = std::min<unsigned>(missing, BUF_LEN - bufEnd);
	}
	if (generateInput(buffer + bufEnd * CHANNELS, missing)) {
		bufEnd += missing;
		nonzeroSamples = bufEnd - bufCurrent + halfFilterLen;
	} else {
		memset(buffer + bufEnd * CHANNELS, 0,
		       missing * sizeof(float));
		bufEnd += missing;
	}

	assert(bufCurrent + halfFilterLen <= bufEnd);
	assert(bufEnd <= BUF_LEN);
}

template <unsigned CHANNELS>
bool Resample<CHANNELS>::generateOutput(float* dataOut, unsigned num)
{
	bool anyNonZero = false;

	// main processing loop
	for (unsigned i = 0; i < num; ++i) {
		// need to reload buffer?
		assert(bufCurrent <= bufEnd);
		int available = bufEnd - bufCurrent;
		if (available <= (int)halfFilterLen) {
			int extra = (ratio > 1.0f)
			          ? lrint((num - i) * ratio) + 1
			          :       (num - i);
			prepareData(extra);
		}
		if (nonzeroSamples) {
			// -- old implementation
			//FilterIndex startFilterIndex(lastPos * floatIncr);
			//calcOutput(startFilterIndex, &dataOut[i * CHANNELS]);
			// -- new implementation
			calcOutput2(lastPos, &dataOut[i * CHANNELS]);
			// --
			anyNonZero = true;
		} else {
			for (unsigned j = 0; j < CHANNELS; ++j) {
				dataOut[i * CHANNELS + j] = 0.0f;
			}
		}

		// figure out the next index
		lastPos += ratio;
		assert(lastPos >= 0.0f);
		float intPos = truncf(lastPos);
		lastPos -= intPos;
		int consumed = lrint(intPos);
		bufCurrent += consumed;
		nonzeroSamples = std::max<int>(0, nonzeroSamples - consumed);
		assert(bufCurrent <= bufEnd);
	}
	return anyNonZero;
}

// Force template instantiation.
template class Resample<1>;
template class Resample<2>;

} // namespace openmsx
