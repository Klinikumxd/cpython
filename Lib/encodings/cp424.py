""" Python Character Mapping Codec generated from 'CP424.TXT' with gencodec.py.

Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.
(c) Copyright 2000 Guido van Rossum.

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

decoding_map = codecs.make_identity_dict(range(256))
decoding_map.update({
        0x0004: 0x009c, # SELECT
        0x0005: 0x0009, # HORIZONTAL TABULATION
        0x0006: 0x0086, # REQUIRED NEW LINE
        0x0007: 0x007f, # DELETE
        0x0008: 0x0097, # GRAPHIC ESCAPE
        0x0009: 0x008d, # SUPERSCRIPT
        0x000a: 0x008e, # REPEAT
        0x0014: 0x009d, # RESTORE/ENABLE PRESENTATION
        0x0015: 0x0085, # NEW LINE
        0x0016: 0x0008, # BACKSPACE
        0x0017: 0x0087, # PROGRAM OPERATOR COMMUNICATION
        0x001a: 0x0092, # UNIT BACK SPACE
        0x001b: 0x008f, # CUSTOMER USE ONE
        0x0020: 0x0080, # DIGIT SELECT
        0x0021: 0x0081, # START OF SIGNIFICANCE
        0x0022: 0x0082, # FIELD SEPARATOR
        0x0023: 0x0083, # WORD UNDERSCORE
        0x0024: 0x0084, # BYPASS OR INHIBIT PRESENTATION
        0x0025: 0x000a, # LINE FEED
        0x0026: 0x0017, # END OF TRANSMISSION BLOCK
        0x0027: 0x001b, # ESCAPE
        0x0028: 0x0088, # SET ATTRIBUTE
        0x0029: 0x0089, # START FIELD EXTENDED
        0x002a: 0x008a, # SET MODE OR SWITCH
        0x002b: 0x008b, # CONTROL SEQUENCE PREFIX
        0x002c: 0x008c, # MODIFY FIELD ATTRIBUTE
        0x002d: 0x0005, # ENQUIRY
        0x002e: 0x0006, # ACKNOWLEDGE
        0x002f: 0x0007, # BELL
        0x0030: 0x0090, # <reserved>
        0x0031: 0x0091, # <reserved>
        0x0032: 0x0016, # SYNCHRONOUS IDLE
        0x0033: 0x0093, # INDEX RETURN
        0x0034: 0x0094, # PRESENTATION POSITION
        0x0035: 0x0095, # TRANSPARENT
        0x0036: 0x0096, # NUMERIC BACKSPACE
        0x0037: 0x0004, # END OF TRANSMISSION
        0x0038: 0x0098, # SUBSCRIPT
        0x0039: 0x0099, # INDENT TABULATION
        0x003a: 0x009a, # REVERSE FORM FEED
        0x003b: 0x009b, # CUSTOMER USE THREE
        0x003c: 0x0014, # DEVICE CONTROL FOUR
        0x003d: 0x0015, # NEGATIVE ACKNOWLEDGE
        0x003e: 0x009e, # <reserved>
        0x003f: 0x001a, # SUBSTITUTE
        0x0040: 0x0020, # SPACE
        0x0041: 0x05d0, # HEBREW LETTER ALEF
        0x0042: 0x05d1, # HEBREW LETTER BET
        0x0043: 0x05d2, # HEBREW LETTER GIMEL
        0x0044: 0x05d3, # HEBREW LETTER DALET
        0x0045: 0x05d4, # HEBREW LETTER HE
        0x0046: 0x05d5, # HEBREW LETTER VAV
        0x0047: 0x05d6, # HEBREW LETTER ZAYIN
        0x0048: 0x05d7, # HEBREW LETTER HET
        0x0049: 0x05d8, # HEBREW LETTER TET
        0x004a: 0x00a2, # CENT SIGN
        0x004b: 0x002e, # FULL STOP
        0x004c: 0x003c, # LESS-THAN SIGN
        0x004d: 0x0028, # LEFT PARENTHESIS
        0x004e: 0x002b, # PLUS SIGN
        0x004f: 0x007c, # VERTICAL LINE
        0x0050: 0x0026, # AMPERSAND
        0x0051: 0x05d9, # HEBREW LETTER YOD
        0x0052: 0x05da, # HEBREW LETTER FINAL KAF
        0x0053: 0x05db, # HEBREW LETTER KAF
        0x0054: 0x05dc, # HEBREW LETTER LAMED
        0x0055: 0x05dd, # HEBREW LETTER FINAL MEM
        0x0056: 0x05de, # HEBREW LETTER MEM
        0x0057: 0x05df, # HEBREW LETTER FINAL NUN
        0x0058: 0x05e0, # HEBREW LETTER NUN
        0x0059: 0x05e1, # HEBREW LETTER SAMEKH
        0x005a: 0x0021, # EXCLAMATION MARK
        0x005b: 0x0024, # DOLLAR SIGN
        0x005c: 0x002a, # ASTERISK
        0x005d: 0x0029, # RIGHT PARENTHESIS
        0x005e: 0x003b, # SEMICOLON
        0x005f: 0x00ac, # NOT SIGN
        0x0060: 0x002d, # HYPHEN-MINUS
        0x0061: 0x002f, # SOLIDUS
        0x0062: 0x05e2, # HEBREW LETTER AYIN
        0x0063: 0x05e3, # HEBREW LETTER FINAL PE
        0x0064: 0x05e4, # HEBREW LETTER PE
        0x0065: 0x05e5, # HEBREW LETTER FINAL TSADI
        0x0066: 0x05e6, # HEBREW LETTER TSADI
        0x0067: 0x05e7, # HEBREW LETTER QOF
        0x0068: 0x05e8, # HEBREW LETTER RESH
        0x0069: 0x05e9, # HEBREW LETTER SHIN
        0x006a: 0x00a6, # BROKEN BAR
        0x006b: 0x002c, # COMMA
        0x006c: 0x0025, # PERCENT SIGN
        0x006d: 0x005f, # LOW LINE
        0x006e: 0x003e, # GREATER-THAN SIGN
        0x006f: 0x003f, # QUESTION MARK
        0x0070: None,   # UNDEFINED
        0x0071: 0x05ea, # HEBREW LETTER TAV
        0x0072: None,   # UNDEFINED
        0x0073: None,   # UNDEFINED
        0x0074: 0x00a0, # NO-BREAK SPACE
        0x0075: None,   # UNDEFINED
        0x0076: None,   # UNDEFINED
        0x0077: None,   # UNDEFINED
        0x0078: 0x2017, # DOUBLE LOW LINE
        0x0079: 0x0060, # GRAVE ACCENT
        0x007a: 0x003a, # COLON
        0x007b: 0x0023, # NUMBER SIGN
        0x007c: 0x0040, # COMMERCIAL AT
        0x007d: 0x0027, # APOSTROPHE
        0x007e: 0x003d, # EQUALS SIGN
        0x007f: 0x0022, # QUOTATION MARK
        0x0080: None,   # UNDEFINED
        0x0081: 0x0061, # LATIN SMALL LETTER A
        0x0082: 0x0062, # LATIN SMALL LETTER B
        0x0083: 0x0063, # LATIN SMALL LETTER C
        0x0084: 0x0064, # LATIN SMALL LETTER D
        0x0085: 0x0065, # LATIN SMALL LETTER E
        0x0086: 0x0066, # LATIN SMALL LETTER F
        0x0087: 0x0067, # LATIN SMALL LETTER G
        0x0088: 0x0068, # LATIN SMALL LETTER H
        0x0089: 0x0069, # LATIN SMALL LETTER I
        0x008a: 0x00ab, # LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        0x008b: 0x00bb, # RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        0x008c: None,   # UNDEFINED
        0x008d: None,   # UNDEFINED
        0x008e: None,   # UNDEFINED
        0x008f: 0x00b1, # PLUS-MINUS SIGN
        0x0090: 0x00b0, # DEGREE SIGN
        0x0091: 0x006a, # LATIN SMALL LETTER J
        0x0092: 0x006b, # LATIN SMALL LETTER K
        0x0093: 0x006c, # LATIN SMALL LETTER L
        0x0094: 0x006d, # LATIN SMALL LETTER M
        0x0095: 0x006e, # LATIN SMALL LETTER N
        0x0096: 0x006f, # LATIN SMALL LETTER O
        0x0097: 0x0070, # LATIN SMALL LETTER P
        0x0098: 0x0071, # LATIN SMALL LETTER Q
        0x0099: 0x0072, # LATIN SMALL LETTER R
        0x009a: None,   # UNDEFINED
        0x009b: None,   # UNDEFINED
        0x009c: None,   # UNDEFINED
        0x009d: 0x00b8, # CEDILLA
        0x009e: None,   # UNDEFINED
        0x009f: 0x00a4, # CURRENCY SIGN
        0x00a0: 0x00b5, # MICRO SIGN
        0x00a1: 0x007e, # TILDE
        0x00a2: 0x0073, # LATIN SMALL LETTER S
        0x00a3: 0x0074, # LATIN SMALL LETTER T
        0x00a4: 0x0075, # LATIN SMALL LETTER U
        0x00a5: 0x0076, # LATIN SMALL LETTER V
        0x00a6: 0x0077, # LATIN SMALL LETTER W
        0x00a7: 0x0078, # LATIN SMALL LETTER X
        0x00a8: 0x0079, # LATIN SMALL LETTER Y
        0x00a9: 0x007a, # LATIN SMALL LETTER Z
        0x00aa: None,   # UNDEFINED
        0x00ab: None,   # UNDEFINED
        0x00ac: None,   # UNDEFINED
        0x00ad: None,   # UNDEFINED
        0x00ae: None,   # UNDEFINED
        0x00af: 0x00ae, # REGISTERED SIGN
        0x00b0: 0x005e, # CIRCUMFLEX ACCENT
        0x00b1: 0x00a3, # POUND SIGN
        0x00b2: 0x00a5, # YEN SIGN
        0x00b3: 0x00b7, # MIDDLE DOT
        0x00b4: 0x00a9, # COPYRIGHT SIGN
        0x00b5: 0x00a7, # SECTION SIGN
        0x00b7: 0x00bc, # VULGAR FRACTION ONE QUARTER
        0x00b8: 0x00bd, # VULGAR FRACTION ONE HALF
        0x00b9: 0x00be, # VULGAR FRACTION THREE QUARTERS
        0x00ba: 0x005b, # LEFT SQUARE BRACKET
        0x00bb: 0x005d, # RIGHT SQUARE BRACKET
        0x00bc: 0x00af, # MACRON
        0x00bd: 0x00a8, # DIAERESIS
        0x00be: 0x00b4, # ACUTE ACCENT
        0x00bf: 0x00d7, # MULTIPLICATION SIGN
        0x00c0: 0x007b, # LEFT CURLY BRACKET
        0x00c1: 0x0041, # LATIN CAPITAL LETTER A
        0x00c2: 0x0042, # LATIN CAPITAL LETTER B
        0x00c3: 0x0043, # LATIN CAPITAL LETTER C
        0x00c4: 0x0044, # LATIN CAPITAL LETTER D
        0x00c5: 0x0045, # LATIN CAPITAL LETTER E
        0x00c6: 0x0046, # LATIN CAPITAL LETTER F
        0x00c7: 0x0047, # LATIN CAPITAL LETTER G
        0x00c8: 0x0048, # LATIN CAPITAL LETTER H
        0x00c9: 0x0049, # LATIN CAPITAL LETTER I
        0x00ca: 0x00ad, # SOFT HYPHEN
        0x00cb: None,   # UNDEFINED
        0x00cc: None,   # UNDEFINED
        0x00cd: None,   # UNDEFINED
        0x00ce: None,   # UNDEFINED
        0x00cf: None,   # UNDEFINED
        0x00d0: 0x007d, # RIGHT CURLY BRACKET
        0x00d1: 0x004a, # LATIN CAPITAL LETTER J
        0x00d2: 0x004b, # LATIN CAPITAL LETTER K
        0x00d3: 0x004c, # LATIN CAPITAL LETTER L
        0x00d4: 0x004d, # LATIN CAPITAL LETTER M
        0x00d5: 0x004e, # LATIN CAPITAL LETTER N
        0x00d6: 0x004f, # LATIN CAPITAL LETTER O
        0x00d7: 0x0050, # LATIN CAPITAL LETTER P
        0x00d8: 0x0051, # LATIN CAPITAL LETTER Q
        0x00d9: 0x0052, # LATIN CAPITAL LETTER R
        0x00da: 0x00b9, # SUPERSCRIPT ONE
        0x00db: None,   # UNDEFINED
        0x00dc: None,   # UNDEFINED
        0x00dd: None,   # UNDEFINED
        0x00de: None,   # UNDEFINED
        0x00df: None,   # UNDEFINED
        0x00e0: 0x005c, # REVERSE SOLIDUS
        0x00e1: 0x00f7, # DIVISION SIGN
        0x00e2: 0x0053, # LATIN CAPITAL LETTER S
        0x00e3: 0x0054, # LATIN CAPITAL LETTER T
        0x00e4: 0x0055, # LATIN CAPITAL LETTER U
        0x00e5: 0x0056, # LATIN CAPITAL LETTER V
        0x00e6: 0x0057, # LATIN CAPITAL LETTER W
        0x00e7: 0x0058, # LATIN CAPITAL LETTER X
        0x00e8: 0x0059, # LATIN CAPITAL LETTER Y
        0x00e9: 0x005a, # LATIN CAPITAL LETTER Z
        0x00ea: 0x00b2, # SUPERSCRIPT TWO
        0x00eb: None,   # UNDEFINED
        0x00ec: None,   # UNDEFINED
        0x00ed: None,   # UNDEFINED
        0x00ee: None,   # UNDEFINED
        0x00ef: None,   # UNDEFINED
        0x00f0: 0x0030, # DIGIT ZERO
        0x00f1: 0x0031, # DIGIT ONE
        0x00f2: 0x0032, # DIGIT TWO
        0x00f3: 0x0033, # DIGIT THREE
        0x00f4: 0x0034, # DIGIT FOUR
        0x00f5: 0x0035, # DIGIT FIVE
        0x00f6: 0x0036, # DIGIT SIX
        0x00f7: 0x0037, # DIGIT SEVEN
        0x00f8: 0x0038, # DIGIT EIGHT
        0x00f9: 0x0039, # DIGIT NINE
        0x00fa: 0x00b3, # SUPERSCRIPT THREE
        0x00fb: None,   # UNDEFINED
        0x00fc: None,   # UNDEFINED
        0x00fd: None,   # UNDEFINED
        0x00fe: None,   # UNDEFINED
        0x00ff: 0x009f, # EIGHT ONES
})

### Encoding Map

encoding_map = codecs.make_encoding_map(decoding_map)
