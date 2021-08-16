/*
 * qrencode - QR Code encoder
 *
 * Printing.
 * Copyright (c) 2021-∞ blurryroots innovation qanat OÜ
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or any later version.
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
/*! \file qrprint.h
    \brief QR code printint.
    
    ^^
*/

#include "qrprint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void writeUTF8_margin(FILE* fp, int margin, int realwidth, 
    const char* white, const char *reset, const char* full)
{
    int x, y;

    for (y = 0; y < margin/2; y++) {
        fputs(white, fp);
        for (x = 0; x < realwidth; x++)
            fputs(full, fp);
        fputs(reset, fp);
        fputc('\n', fp);
    }
}

static int writeUTF8(FILE *fp, const QRcode *qrcode, int use_ansi, int invert, int margin)
{
    int x, y;
    int realwidth;
    const char *white, *reset;
    const char *empty, *lowhalf, *uphalf, *full;

    empty = " ";
    lowhalf = "\342\226\204";
    uphalf = "\342\226\200";
    full = "\342\226\210";

    if (invert) {
        const char *tmp;

        tmp = empty;
        empty = full;
        full = tmp;

        tmp = lowhalf;
        lowhalf = uphalf;
        uphalf = tmp;
    }

    if (use_ansi){
        if (use_ansi == 2) {
            white = "\033[38;5;231m\033[48;5;16m";
        } else {
            white = "\033[40;37;1m";
        }
        reset = "\033[0m";
    } else {
        white = "";
        reset = "";
    }

    realwidth = (qrcode->width + margin * 2);

    /* top margin */
    writeUTF8_margin(fp, 0, realwidth, white, reset, full);

    /* data */
    for(y = 0; y < qrcode->width; y += 2) {
        unsigned char *row1, *row2;
        row1 = qrcode->data + y*qrcode->width;
        row2 = row1 + qrcode->width;

        fputs(white, fp);

        for (x = 0; x < margin; x++) {
            fputs(full, fp);
        }

        for (x = 0; x < qrcode->width; x++) {
            if(row1[x] & 1) {
                if(y < qrcode->width - 1 && row2[x] & 1) {
                    fputs(empty, fp);
                } else {
                    fputs(lowhalf, fp);
                }
            } else if(y < qrcode->width - 1 && row2[x] & 1) {
                fputs(uphalf, fp);
            } else {
                fputs(full, fp);
            }
        }

        for (x = 0; x < margin; x++)
            fputs(full, fp);

        fputs(reset, fp);
        fputc('\n', fp);
    }

    /* bottom margin */
    writeUTF8_margin(fp, 0, realwidth, white, reset, full);

    return 0;
}

void QRprint_utf8(FILE* out, const char* text, QRecLevel quality, int version, int casesensitive)
{
    QRcode* code = QRcode_encodeString(
        text,
        version,
        quality,
        QR_MODE_8,
        casesensitive
    );

    writeUTF8(out, code, 0, 0, 0);

    free(code);
}