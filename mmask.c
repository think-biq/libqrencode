/*
 * qrencode - QR Code encoder
 *
 * Masking for Micro QR Code.
 * Copyright (C) 2006-2017 Kentaro Fukuchi <kentaro@fukuchi.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "mmask.h"

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "qrencode.h"
#include "mqrspec.h"

STATIC_IN_RELEASE void MMask_writeFormatInformation(int version, int width, unsigned char *frame, int mask, QRecLevel level)
{
	unsigned int format;
	unsigned char v;
	int i;

	format = MQRspec_getFormatInfo(mask, version, level);

	for(i = 0; i < 8; i++) {
		v = 0x84 | (format & 1);
		frame[width * (i + 1) + 8] = v;
		format = format >> 1;
	}
	for(i = 0; i < 7; i++) {
		v = 0x84 | (format & 1);
		frame[width * 8 + 7 - i] = v;
		format = format >> 1;
	}
}

static void MMask_mask0(int width, const unsigned char *s, unsigned char *d)
{
	MMASKMAKER(y&1)
}

static void MMask_mask1(int width, const unsigned char *s, unsigned char *d)
{
	MMASKMAKER(((y/2)+(x/3))&1)
}

static void MMask_mask2(int width, const unsigned char *s, unsigned char *d)
{
	MMASKMAKER((((x*y)&1)+(x*y)%3)&1)
}

static void MMask_mask3(int width, const unsigned char *s, unsigned char *d)
{
	MMASKMAKER((((x+y)&1)+((x*y)%3))&1)
}

#define mmaskNum (4)
typedef void MMaskMaker(int, const unsigned char *, unsigned char *);
static MMaskMaker *mmaskMakers[mmaskNum] = {
	MMask_mask0, MMask_mask1, MMask_mask2, MMask_mask3
};

#ifdef WITH_TESTS
unsigned char *MMask_makeMaskedFrame(int width, unsigned char *frame, int mask)
{
	unsigned char *masked;

	masked = (unsigned char *)malloc((size_t)(width * width));
	if(masked == NULL) return NULL;

	mmaskMakers[mask](width, frame, masked);

	return masked;
}
#endif

unsigned char *MMask_makeMask(int version, unsigned char *frame, int mask, QRecLevel level)
{
	unsigned char *masked;
	int width;

	if(mask < 0 || mask >= mmaskNum) {
		errno = EINVAL;
		return NULL;
	}

	width = MQRspec_getWidth(version);
	masked = (unsigned char *)malloc((size_t)(width * width));
	if(masked == NULL) return NULL;

	mmaskMakers[mask](width, frame, masked);
	MMask_writeFormatInformation(version, width, masked, mask, level);

	return masked;
}

STATIC_IN_RELEASE int MMask_evaluateSymbol(int width, unsigned char *frame)
{
	int x, y;
	unsigned char *p;
	int sum1 = 0, sum2 = 0;

	p = frame + width * (width - 1);
	for(x = 1; x < width; x++) {
		sum1 += (p[x] & 1);
	}

	p = frame + width * 2 - 1;
	for(y = 1; y < width; y++) {
		sum2 += (*p & 1);
		p += width;
	}

	return (sum1 <= sum2)?(sum1 * 16 + sum2):(sum2 * 16 + sum1);
}

unsigned char *MMask_mask(int version, unsigned char *frame, QRecLevel level)
{
	int i;
	unsigned char *mask, *bestMask;
	int maxScore = 0;
	int score;
	int width;

	width = MQRspec_getWidth(version);

	mask = (unsigned char *)malloc((size_t)(width * width));
	if(mask == NULL) return NULL;
	bestMask = NULL;

	for(i = 0; i < mmaskNum; i++) {
		score = 0;
		mmaskMakers[i](width, frame, mask);
		MMask_writeFormatInformation(version, width, mask, i, level);
		score = MMask_evaluateSymbol(width, mask);
		if(score > maxScore) {
			maxScore = score;
			free(bestMask);
			bestMask = mask;
			mask = (unsigned char *)malloc((size_t)(width * width));
			if(mask == NULL) break;
		}
	}
	free(mask);
	return bestMask;
}
