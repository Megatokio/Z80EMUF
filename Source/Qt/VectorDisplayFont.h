#pragma once
/*	Copyright  (c)	Günter Woigk 2016 - 2019
					mailto:kio@little-bat.de

	This file is free software.

	Permission to use, copy, modify, distribute, and sell this software
	and its documentation for any purpose is hereby granted without fee,
	provided that the above copyright notice appears in all copies and
	that both that copyright notice, this permission notice and the
	following disclaimer appear in supporting documentation.

	THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY, NOT EVEN THE
	IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
	AND IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES
	ARISING FROM THE USE OF THIS SOFTWARE,
	TO THE EXTENT PERMITTED BY APPLICABLE LAW.
*/

#include "kio/kio.h"

/*	Font-Data:

	defb	left-side mask	*
	defb	right-side mask	*
	defb	L, x,y, x,y, …
	defb	L, x,y, x,y, …
	defb	E

	*) Pixelmaske, die für optionales Kerning benutzt werden kann:
	das erste Bit (Bit 7) steht für Grundlinie-1,
	das 2. Bit (Bit 6) für die Grundlinie,
	...
	das letzte Bit (Bit 0) steht für die oberste Versallinie bei Grundlinie+6

	Wenn man die rechte Maske eines Buchstabens mit der linken Maske des folgenden Buchstaben vergleicht
	(bitwise AND) kann man sehen, ob man die die Buchstaben um ein Pixel näher aneinander schieben darf.
*/

int8 vt_font_data[] =
{
// Leerzeichen
	0x00, 0x00,
	L, 3,0, E,

// !
	0b01111111, 0b00000011,
	L, 0,0, 0,1,
	L, 0,2, 0,6, 1,6, 0,2, E,

// "
	0b00000011, 0b00000011,
	L, 0,5, 0,6,
	L, 1,5, 1,6, E,

// #
	0b00010100, 0b00010100,
	L, 0,2, 4,2,
	L, 0,4, 4,4,
	L, 1,1, 1,5,
	L, 3,1, 3,5, E,

// $
	0b00100110, 0b00110000,
	L, 0,1, 1,0, 3,0, 4,2, 0,4, 1,6, 3,6,
	L, 2,-1, 2,7, E,

// %
	0b00100110, 0b00110010,
	L, 0,1, 3,5,
	L, 0,4, 1,4, 1,5, 0,5, 0,4,
	L, 2,1, 3,1, 3,2, 2,2, 2,1, E,

// &
	0b00110110, 0b01010000,
	L, 4,2, 2,0, 1,0, 0,1, 0,2, 2,4, 2,5, 1,6, 0,5, 0,4, 4,0, E,

// '
	0b00000011, 0b00000011,
	L, 0,5, 0,6, E,

// (
	-1, 0b01100011,
	L, 1,0, 0,2, 0,4, 1,6, E,

// )
	0b01100011, -1,
	L, 0,0, 1,2, 1,4, 0,6, E,

// *
	0b00001000, 0b00001000,
	L, 0,3, 4,3,
	L, 1,1, 3,5,
	L, 1,5, 3,1, E,

// +
	0b00001000, 0b00001000,
	L, 0,3, 4,3,
	L, 2,1, 2,5, E,

// ,
	int8(0b11000000), 0b01100000,
	L, 0,-1, 1,1, E,

// -
	0b00001000, 0b00001000,
	L, 0,3, 4,3, E,

// .
	0b01000000, 0b01000000,
	L, 0,0, 1,0, E,

// /
	0b00100000, 0b00000010,
	L, 0,1, 3,5, E,

// 0
	0b00111110, 0b00111110,
	L, 1,1, 3,5,
	L, 1,0, 0,2, 0,4, 1,6, 3,6, 4,4, 4,2, 3,0, 1,0, E,

// 1
	0b01001000, 0b01000000,
	L, 0,0, 4,0,
	L, 0,3, 2,6, 2,0, E,

// 2
	0b01000010, 0b01000110,
	L, 0,5, 1,6, 3,6, 4,5, 4,4, 0,0, 4,0, E,

// 3
	0b00100010, 0b00110110,
	L, 0,1, 1,0, 3,0, 4,2, 3,3, 4,4, 3,6, 1,6, 0,5, E,

// 4
	0b00011000, 0b01010000,
	L, 1,6, 0,2, 4,2,
	L, 3,4, 3,0,
	L, 2,0, 4,0, E,

// 5
	0b00100111, 0b00111000,
	L, 0,1, 1,0, 3,0, 4,2, 3,4, 0,4, 0,6, 3,6, E,

// 6
	0b00111000, 0b00111000,
	L, 3,6, 2,6, 0,2, 1,0, 3,0, 4,2, 3,4, 1,4, E,

// 7
	0b00000001, 0b00001011,
	L, 1,3, 4,3,
	L, 0,6, 4,6, 1,0, E,

// 8
	0b00110010, 0b00110010,
	L, 1,0, 0,1, 0,2, 4,5, 3,6, 1,6, 0,5, 4,2, 4,1, 3,0, 1,0, E,

// 9,
	0b00001110, 0b00001110,
	L, 1,0, 2,0, 4,4, 3,6, 1,6, 0,4, 1,2, 3,2, E,

// :
	0b00111100, 0b00111100,
	L, 0,1, 0,2,
	L, 0,3, 0,4, E,

// ;
	int8(0b11010000), 0b01110000,
	L, 0,2, 1,2,
	L, 1,1, 0,-1, E,

// <
	0b00001000, 0b00100010,
	L, 3,1, 0,3, 3,5, E,

// =
	0b00010100, 0b00010100,
	L, 0,2, 4,2,
	L, 0,4, 4,4, E,

// >
	0b00010100, 0b00010100,
	L, 0,1, 3,3, 0,5, E,

// ?
	0b00000010, 0b00001110,
	L, 0,5, 1,6, 2,5, 2,4, 1,2,
	L, 1,1, 1,0, E,

// @
	0b00111100, 0b01111100,
	L, 3,1, 3,3, 2,3, 1,2, 1,1, 4,1, 4,3, 3,5, 1,5, 0,3, 0,1, 1,0, 4,0, E,

// A
	0b01100000, 0b01100000,
	L, 0,0, 2,6, 4,0,
	L, 1,2, 3,2, E,

// B
	0b01111111, 0b00100000,
	L, 3,3, 3,4, 2,6, 0,6, 0,0, 3,0, 4,1, 3,3, 1,3, E,


// C
	0b00111110, 0b00100010,
	L, 4,1, 3,0, 1,0, 0,2, 0,4, 1,6, 3,6, 4,5, E,

// D
	0b01000000, 0b00111110,
	L, 0,0, 3,0, 4,2, 4,4, 3,6, 1,6, 1,0, E,

// E
	0b01111111, 0b01000001,
	L, 0,3, 2,3,
	L, 3,6, 0,6, 0,0, 3,0, E,

// F
	0b01000000, 0b00000011,
	L, 0,0, 2,0,
	L, 1,0, 1,6,
	L, 1,3, 2,3,
	L, 0,6, 4,6, 4,5, E,

// G
	0b00111110, 0b00111010,
	L, 2,3, 4,3, 4,1, 3,0, 1,0, 0,2, 0,4, 1,6, 3,6, 4,5, E,

// H
	0b01111111, 0b01111111,
	L, 0,0, 0,6,
	L, 0,3, 3,3,
	L, 3,6, 3,0, E,

// I
	0b01000001, 0b01000001,
	L, 0,0, 2,0,
	L, 1,0, 1,6,
	L, 0,6, 2,6, E,

// J
	0b00000011, 0b00111111,
	L, 0,5, 0,6, 3,6, 3,2, 2,0, 1,0, E,

// K
	0b01111111, 0b01000000,
	L, 0,0, 0,6,
	L, 0,3, 1,3,
	L, 2,6, 1,3, 3,0, E,

// L
	0b01000001, 0b01100000,
	L, 0,0, 4,0, 4,1,
	L, 1,0, 1,6,
	L, 0,6, 2,6, E,

// M
	0b01111111, 0b01111111,
	L, 0,0, 0,6, 2,2, 4,6, 4,0, E,

// N
	0b01111111, 0b01111111,
	L, 0,0, 0,6, 3,0, 3,6, E,

// O
	0b00111110, 0b00111110,
	L, 1,0, 0,2, 0,4, 1,6, 3,6, 4,4, 4,2, 3,0, 1,0, E,

// P
	0b01000001, 0b00001110,
	L, 0,0, 2,0,
	L, 1,0, 1,6,
	L, 0,6, 3,6, 4,4, 3,2, 1,2, E,

// Q
	0b00111110, 0b01111110,
	L, 1,0, 0,2, 0,4, 1,6, 3,6, 4,4, 4,2, 3,0, 1,0,
	L, 2,2, 4,0, E,

// R
	0b00000001, 0b01001110,
	L, 1,0, 1,6,
	L, 0,6, 3,6, 4,4, 3,2, 1,2,
	L, 2,2, 4,0, E,

// S
	0b00100110, 0b00110000,
	L, 0,1, 1,0, 3,0, 4,1, 4,2, 3,3, 0,4, 0,5, 1,6, 3,6, E,

// T
	0b00000011, 0b00000011,
	L, 1,0, 3,0,
	L, 2,0, 2,6,
	L, 0,5, 0,6, 4,6, 4,5, E,

// U
	0b00111111, 0b01000000,
	L, 3,6, 3,0,
	L, 0,6, 0,1, 1,0, 4,0, E,

// V
	0b00000011, 0b00000011,
	L, 0,6, 2,0, 4,6, E,

// W
	0b00000011, 0b00000011,
	L, 0,6, 1,0, 2,4, 3,0, 4,6, E,

// X
	0b01100011, 0b01100011,
	L, 0,0, 3,6,
	L, 0,6, 3,0, E,

// Y
	0b00000011, 0b00000011,
	L, 1,0, 3,0,
	L, 2,0, 2,2,
	L, 0,6, 2,2, 4,6, E,

// Z
	0b01100001, 0b01000011,
	L, 1,3, 3,3,
	L, 0,6, 4,6, 0,0, 4,0, E,

// [
	-1, 0b01000001,
	L, 1,0, 0,0, 0,6, 1,6, E,

// '\'
	0b0000010, 0b00100000,
	L, 3,1, 0,5, E,

// ]
	0b01000001, -1,
	L, 0,0, 1,0, 1,6, 0,6, E,

// ^
	0b00000010, 0b00000010,
	L, 0,5, 1,6, 2,5, E,

// _
	0x00, 0x00,
	L, 0,-1, 4,-1, E,

// `
	0b00000001, 0b00000010,
	L, 0,6, 1,5, E,

// a
	0b00111000, 0b01000000,
	L, 3,0, 3,4, 1,4, 0,2, 1,0, 4,0, E,

// b
	0b01111111, 0b00111000,
	L, 0,6, 0,0, 2,0, 3,2, 2,4, 0,4, E,

// c
	0b00111000, 0b01000100,
	L, 3,4, 1,4, 0,2, 1,0, 3,0, E,

// d
	0b00111000, 0b01000000,
	L, 3,6, 3,0,
	L, 3,4, 1,4, 0,2, 1,0, 4,0, E,

// e
	0b00111000, 0b00111000,
	L, 0,2, 4,2, 3,4, 1,4, 0,2, 1,0, 3,0, E,

// f
	0b01000000, 0b01000001,
	L, 0,0, 2,0,
	L, 1,-2,1,5, 2,6, E,

// g
	0b00111000, 0b01111000,
	L, 4,1, 3,0, 1,0, 0,2, 1,4, 3,4, 4,3, 4,1, 3, -1, 1,-1, E,

// h
	0b01111111, 0b01111000,
	L, 0,0, 0,6,
	L, 0,4, 2,4, 3,3, 3,0, E,

// i
	0b01000100, 0b01000000,
	L, 0,0, 2,0,
	L, 1,0, 1,4, 0,4,
	L, 1,5, 1,6, E,

// j
	0b00000100, int8(0b11111111),
	L, 0,4, 1,4, 1,0, 0,-2,
	L, 1,5, 1,6, E,

// k
	0b01111111, 0b01000100,
	L, 0,0, 0,6,
	L, 2,4, 0,2, 2,0, E,

// l
	0b01000000, 0b01000001,
	L, 0,0, 2,0,
	L, 1,0, 1,6, 2,6, E,

// m
	0b01111100, 0b01111000,
	L, 0,0, 0,4, 1,4, 2,3,
	L, 2,0, 2,4, 3,4, 4,3, 4,0, E,

// n
	0b01111100, 0b01111000,
	L, 0,0, 0,4, 2,4, 3,3, 3,0, E,

// o
	0b00111000, 0b00111000,
	L, 1,0, 3,0, 4,2, 3,4, 1,4, 0,2, 1,0, E,

// p
	int8(0b11111100), 0b00111000,
	L, 1,0, 2,0, 3,2, 2,4, 0,4, 0,-2, E,

// q
	0b00111000, int8(0b11111100),
	L, 2,0, 1,0, 0,2, 1,4, 3,4, 3,-2, E,

// r
	0b01111100, 0b00001000,
	L, 0,0, 0,4,
	L, 0,3, 1,4, 2,4, 3,3, E,

// s
	0b01001000, 0b00100000,
	L, 0,0, 2,0, 3,1, 2,2, 1,2, 0,3, 1,4, 2,4, E,

// t
	0b00000100, 0b01000100,
	L, 0,4, 2,4,
	L, 1,6, 1,1, 2,0, E,

// u
	0b00111100, 0b01111100,
	L, 0,4, 0,1, 1,0, 3,0, 3,4, E,

// v
	0b00000100, 0b00000100,
	L, 0,4, 2,0, 4,4, E,

// w
	0b00001100, 0b00001100,
	L, 0,4, 1,0, 2,3, 3,0, 4,4, E,

// x
	0b01000100, 0b01000100,
	L, 0,0, 4,4,
	L, 0,4, 4,0, E,

// y
	0b00001100,	0b00001100,
	L, 0,4, 2,0,
	L, 4,4, 2,-1, 1,-1, E,

// z
	0b01000100, 0b01000100,
	L, 0,4, 4,4, 0,0, 4,0, E,

// {
	-1, int8(0b10000000),
	L, 2,-1, 1,0, 1,2, 0,3, 1,4, 1,6, 2,7, E,

// |
	0b01111111, 0b01111111,
	L, 0,0, 0,6, E,

// }
	int8(0b10000000), -1,
	L, 0,-1, 1,0, 1,2, 2,3, 1,4, 1,6, 0,7, E,

// ~
	0b00000010, 0b00000001,
	L, 0,5, 1,6, 2,5, E,

// DEL
	0,0,E,

// 32 undefined characters
	0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,
	0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,
	0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,
	0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,0,0,E,

// nbsp
	0,0,E,

// ¡
	0,0,E,

// ¢
	0,0,E,

// £
	0,0,E,

// ¤
	0,0,E,

// ¥
	0,0,E,

// ¦
	0,0,E,

// §
	0,0,E,

// ¨
	0,0,E,

// ©
	0,0,E,

// ª
	0,0,E,

// «
	0,0,E,

// ¬
	0,0,E,

// soft-
	0,0,E,

// ®
	0,0,E,

// ¯
	0,0,E,

// °
	0,0,E,

// ±
	0,0,E,

// ²
	0,0,E,

// ³
	0,0,E,

// ´
	0,0,E,

// µ
	0,0,E,

// ¶
	0,0,E,

// ·
	0,0,E,

// ¸
	0,0,E,

// ¹
	0,0,E,

// º
	0,0,E,

// »
	0,0,E,

// ¼
	0,0,E,

// ½
	0,0,E,

// ¾
	0,0,E,

// ¿
	0,0,E,

// À
	0,0,E,

// Á
	0,0,E,

// Â
	0,0,E,

// Ã
	0,0,E,

// Ä
	0b01100001, 0b01100001,
	L, 0,0, 2,6, 4,0,
	L, 1,2, 3,2,
	L, 0,6, 1,6,
	L, 3,6, 4,6, E,

// Å
	0,0,E,

// Æ
	0,0,E,

// Ç
	0,0,E,

// È
	0,0,E,

// É
	0,0,E,

// Ê
	0,0,E,

// Ë
	0,0,E,

// Ì
	0,0,E,

// Í
	0,0,E,

// Î
	0,0,E,

// Ï
	0,0,E,

// Ð
	0,0,E,

// Ñ
	0,0,E,

// Ò
	0,0,E,

// Ó
	0,0,E,

// Ô
	0,0,E,

// Õ
	0,0,E,

// Ö
	0b00111101, 0b00111101,
	L, 1,0, 0,1, 0,4, 1,5, 2,5, 3,4, 3,1, 2,0, 1,0,
	L, 0,6, 1,6,
	L, 3,6, 4,6, E,

// ×
	0,0,E,

// Ø
	0,0,E,

// Ù
	0,0,E,

// Ú
	0,0,E,

// Û
	0,0,E,

// Ü
	0b00111111, 0b01000000,
	L, 3,0, 3,5,
	L, 0,5, 0,1, 1,0, 4,0,
	L, 0,6, 1,6,
	L, 3,6, 4,6, E,

// Ý
	0,0,E,

// Þ
	0,0,E,

// ß
	int8(0b11111110), 0b00111010,
	L, 1,0, 2,0, 3,1, 3,3, 2,4, 3,5, 2,6, 1,6, 0,5, 0,-2, E,

// à
	0,0,E,

// á
	0,0,E,

// â
	0,0,E,

// ã
	0,0,E,

// ä
	0b00111000, 0b01000000,
	L, 3,0, 3,4, 1,4, 0,2, 1,0, 4,0,
	L, 1,5, 1,6,
	L, 3,5, 3,6, E,

// å
	0,0,E,

// æ
	0,0,E,

// ç
	0,0,E,

// è
	0,0,E,

// é
	0,0,E,

// ê
	0,0,E,

// ë
	0,0,E,

// ì
	0,0,E,

// í
	0,0,E,

// î
	0,0,E,

// ï
	0,0,E,

// ð
	0,0,E,

// ñ
	0,0,E,

// ò
	0,0,E,

// ó
	0,0,E,

// ô
	0,0,E,

// õ
	0,0,E,

// ö
	0b00111000, 0b00111000,
	L, 1,0, 3,0, 4,2, 3,4, 1,4, 0,2, 1,0,
	L, 1,5, 1,6,
	L, 3,5, 3,6, E,

// ÷
	0,0,E,

// ø
	0,0,E,

// ù
	0,0,E,

// ú
	0,0,E,

// û
	0,0,E,

// ü
	0b00111100, 0b01111100,
	L, 0,4, 0,1, 1,0, 3,0, 3,4,
	L, 1,5, 1,6,
	L, 2,5, 2,6, E,

// ý
	0,0,E,

// þ
	0,0,E,

// ÿ
	0,0,E
};










