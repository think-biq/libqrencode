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

#include <qrencode.h>
#include <stdio.h>

/**
* Prints given QR code to given file stream.
* 
* @param out Output file stream pointer.
* @param text Text to base the QR code on.
* @param quality Quality of QR code.
* @param version Version of QR code.
* @param casesensitive If text should be treated case-sensitive.
*/
void QRprint_utf8(const FILE* out, const char* text, QRecLevel quality, int version, int casesensitive);