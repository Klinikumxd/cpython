""" Python Character Mapping Codec generated from 'LATIN2.TXT'.


Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)
        
    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_map)

class StreamWriter(Codec,codecs.StreamWriter):
    pass
        
class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():

    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)

### Decoding Map

decoding_map = {

	0x0080: 0x00c4,	# LATIN CAPITAL LETTER A WITH DIAERESIS
	0x0081: 0x0100,	# LATIN CAPITAL LETTER A WITH MACRON
	0x0082: 0x0101,	# LATIN SMALL LETTER A WITH MACRON
	0x0083: 0x00c9,	# LATIN CAPITAL LETTER E WITH ACUTE
	0x0084: 0x0104,	# LATIN CAPITAL LETTER A WITH OGONEK
	0x0085: 0x00d6,	# LATIN CAPITAL LETTER O WITH DIAERESIS
	0x0086: 0x00dc,	# LATIN CAPITAL LETTER U WITH DIAERESIS
	0x0087: 0x00e1,	# LATIN SMALL LETTER A WITH ACUTE
	0x0088: 0x0105,	# LATIN SMALL LETTER A WITH OGONEK
	0x0089: 0x010c,	# LATIN CAPITAL LETTER C WITH CARON
	0x008a: 0x00e4,	# LATIN SMALL LETTER A WITH DIAERESIS
	0x008b: 0x010d,	# LATIN SMALL LETTER C WITH CARON
	0x008c: 0x0106,	# LATIN CAPITAL LETTER C WITH ACUTE
	0x008d: 0x0107,	# LATIN SMALL LETTER C WITH ACUTE
	0x008e: 0x00e9,	# LATIN SMALL LETTER E WITH ACUTE
	0x008f: 0x0179,	# LATIN CAPITAL LETTER Z WITH ACUTE
	0x0090: 0x017a,	# LATIN SMALL LETTER Z WITH ACUTE
	0x0091: 0x010e,	# LATIN CAPITAL LETTER D WITH CARON
	0x0092: 0x00ed,	# LATIN SMALL LETTER I WITH ACUTE
	0x0093: 0x010f,	# LATIN SMALL LETTER D WITH CARON
	0x0094: 0x0112,	# LATIN CAPITAL LETTER E WITH MACRON
	0x0095: 0x0113,	# LATIN SMALL LETTER E WITH MACRON
	0x0096: 0x0116,	# LATIN CAPITAL LETTER E WITH DOT ABOVE
	0x0097: 0x00f3,	# LATIN SMALL LETTER O WITH ACUTE
	0x0098: 0x0117,	# LATIN SMALL LETTER E WITH DOT ABOVE
	0x0099: 0x00f4,	# LATIN SMALL LETTER O WITH CIRCUMFLEX
	0x009a: 0x00f6,	# LATIN SMALL LETTER O WITH DIAERESIS
	0x009b: 0x00f5,	# LATIN SMALL LETTER O WITH TILDE
	0x009c: 0x00fa,	# LATIN SMALL LETTER U WITH ACUTE
	0x009d: 0x011a,	# LATIN CAPITAL LETTER E WITH CARON
	0x009e: 0x011b,	# LATIN SMALL LETTER E WITH CARON
	0x009f: 0x00fc,	# LATIN SMALL LETTER U WITH DIAERESIS
	0x00a0: 0x2020,	# DAGGER
	0x00a1: 0x00b0,	# DEGREE SIGN
	0x00a2: 0x0118,	# LATIN CAPITAL LETTER E WITH OGONEK
	0x00a4: 0x00a7,	# SECTION SIGN
	0x00a5: 0x2022,	# BULLET
	0x00a6: 0x00b6,	# PILCROW SIGN
	0x00a7: 0x00df,	# LATIN SMALL LETTER SHARP S
	0x00a8: 0x00ae,	# REGISTERED SIGN
	0x00aa: 0x2122,	# TRADE MARK SIGN
	0x00ab: 0x0119,	# LATIN SMALL LETTER E WITH OGONEK
	0x00ac: 0x00a8,	# DIAERESIS
	0x00ad: 0x2260,	# NOT EQUAL TO
	0x00ae: 0x0123,	# LATIN SMALL LETTER G WITH CEDILLA
	0x00af: 0x012e,	# LATIN CAPITAL LETTER I WITH OGONEK
	0x00b0: 0x012f,	# LATIN SMALL LETTER I WITH OGONEK
	0x00b1: 0x012a,	# LATIN CAPITAL LETTER I WITH MACRON
	0x00b2: 0x2264,	# LESS-THAN OR EQUAL TO
	0x00b3: 0x2265,	# GREATER-THAN OR EQUAL TO
	0x00b4: 0x012b,	# LATIN SMALL LETTER I WITH MACRON
	0x00b5: 0x0136,	# LATIN CAPITAL LETTER K WITH CEDILLA
	0x00b6: 0x2202,	# PARTIAL DIFFERENTIAL
	0x00b7: 0x2211,	# N-ARY SUMMATION
	0x00b8: 0x0142,	# LATIN SMALL LETTER L WITH STROKE
	0x00b9: 0x013b,	# LATIN CAPITAL LETTER L WITH CEDILLA
	0x00ba: 0x013c,	# LATIN SMALL LETTER L WITH CEDILLA
	0x00bb: 0x013d,	# LATIN CAPITAL LETTER L WITH CARON
	0x00bc: 0x013e,	# LATIN SMALL LETTER L WITH CARON
	0x00bd: 0x0139,	# LATIN CAPITAL LETTER L WITH ACUTE
	0x00be: 0x013a,	# LATIN SMALL LETTER L WITH ACUTE
	0x00bf: 0x0145,	# LATIN CAPITAL LETTER N WITH CEDILLA
	0x00c0: 0x0146,	# LATIN SMALL LETTER N WITH CEDILLA
	0x00c1: 0x0143,	# LATIN CAPITAL LETTER N WITH ACUTE
	0x00c2: 0x00ac,	# NOT SIGN
	0x00c3: 0x221a,	# SQUARE ROOT
	0x00c4: 0x0144,	# LATIN SMALL LETTER N WITH ACUTE
	0x00c5: 0x0147,	# LATIN CAPITAL LETTER N WITH CARON
	0x00c6: 0x2206,	# INCREMENT
	0x00c7: 0x00ab,	# LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	0x00c8: 0x00bb,	# RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	0x00c9: 0x2026,	# HORIZONTAL ELLIPSIS
	0x00ca: 0x00a0,	# NO-BREAK SPACE
	0x00cb: 0x0148,	# LATIN SMALL LETTER N WITH CARON
	0x00cc: 0x0150,	# LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
	0x00cd: 0x00d5,	# LATIN CAPITAL LETTER O WITH TILDE
	0x00ce: 0x0151,	# LATIN SMALL LETTER O WITH DOUBLE ACUTE
	0x00cf: 0x014c,	# LATIN CAPITAL LETTER O WITH MACRON
	0x00d0: 0x2013,	# EN DASH
	0x00d1: 0x2014,	# EM DASH
	0x00d2: 0x201c,	# LEFT DOUBLE QUOTATION MARK
	0x00d3: 0x201d,	# RIGHT DOUBLE QUOTATION MARK
	0x00d4: 0x2018,	# LEFT SINGLE QUOTATION MARK
	0x00d5: 0x2019,	# RIGHT SINGLE QUOTATION MARK
	0x00d6: 0x00f7,	# DIVISION SIGN
	0x00d7: 0x25ca,	# LOZENGE
	0x00d8: 0x014d,	# LATIN SMALL LETTER O WITH MACRON
	0x00d9: 0x0154,	# LATIN CAPITAL LETTER R WITH ACUTE
	0x00da: 0x0155,	# LATIN SMALL LETTER R WITH ACUTE
	0x00db: 0x0158,	# LATIN CAPITAL LETTER R WITH CARON
	0x00dc: 0x2039,	# SINGLE LEFT-POINTING ANGLE QUOTATION MARK
	0x00dd: 0x203a,	# SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
	0x00de: 0x0159,	# LATIN SMALL LETTER R WITH CARON
	0x00df: 0x0156,	# LATIN CAPITAL LETTER R WITH CEDILLA
	0x00e0: 0x0157,	# LATIN SMALL LETTER R WITH CEDILLA
	0x00e1: 0x0160,	# LATIN CAPITAL LETTER S WITH CARON
	0x00e2: 0x201a,	# SINGLE LOW-9 QUOTATION MARK
	0x00e3: 0x201e,	# DOUBLE LOW-9 QUOTATION MARK
	0x00e4: 0x0161,	# LATIN SMALL LETTER S WITH CARON
	0x00e5: 0x015a,	# LATIN CAPITAL LETTER S WITH ACUTE
	0x00e6: 0x015b,	# LATIN SMALL LETTER S WITH ACUTE
	0x00e7: 0x00c1,	# LATIN CAPITAL LETTER A WITH ACUTE
	0x00e8: 0x0164,	# LATIN CAPITAL LETTER T WITH CARON
	0x00e9: 0x0165,	# LATIN SMALL LETTER T WITH CARON
	0x00ea: 0x00cd,	# LATIN CAPITAL LETTER I WITH ACUTE
	0x00eb: 0x017d,	# LATIN CAPITAL LETTER Z WITH CARON
	0x00ec: 0x017e,	# LATIN SMALL LETTER Z WITH CARON
	0x00ed: 0x016a,	# LATIN CAPITAL LETTER U WITH MACRON
	0x00ee: 0x00d3,	# LATIN CAPITAL LETTER O WITH ACUTE
	0x00ef: 0x00d4,	# LATIN CAPITAL LETTER O WITH CIRCUMFLEX
	0x00f0: 0x016b,	# LATIN SMALL LETTER U WITH MACRON
	0x00f1: 0x016e,	# LATIN CAPITAL LETTER U WITH RING ABOVE
	0x00f2: 0x00da,	# LATIN CAPITAL LETTER U WITH ACUTE
	0x00f3: 0x016f,	# LATIN SMALL LETTER U WITH RING ABOVE
	0x00f4: 0x0170,	# LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
	0x00f5: 0x0171,	# LATIN SMALL LETTER U WITH DOUBLE ACUTE
	0x00f6: 0x0172,	# LATIN CAPITAL LETTER U WITH OGONEK
	0x00f7: 0x0173,	# LATIN SMALL LETTER U WITH OGONEK
	0x00f8: 0x00dd,	# LATIN CAPITAL LETTER Y WITH ACUTE
	0x00f9: 0x00fd,	# LATIN SMALL LETTER Y WITH ACUTE
	0x00fa: 0x0137,	# LATIN SMALL LETTER K WITH CEDILLA
	0x00fb: 0x017b,	# LATIN CAPITAL LETTER Z WITH DOT ABOVE
	0x00fc: 0x0141,	# LATIN CAPITAL LETTER L WITH STROKE
	0x00fd: 0x017c,	# LATIN SMALL LETTER Z WITH DOT ABOVE
	0x00fe: 0x0122,	# LATIN CAPITAL LETTER G WITH CEDILLA
	0x00ff: 0x02c7,	# CARON
}

### Encoding Map

encoding_map = {}
for k,v in decoding_map.items():
    encoding_map[v] = k
