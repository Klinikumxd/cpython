""" Python Character Mapping Codec generated from 'VENDORS/MICSFT/PC/CP864.TXT' with gencodec.py.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)

    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_table)
    
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
    0x0025: 0x066a,	#  ARABIC PERCENT SIGN
    0x0080: 0x00b0,	#  DEGREE SIGN
    0x0081: 0x00b7,	#  MIDDLE DOT
    0x0082: 0x2219,	#  BULLET OPERATOR
    0x0083: 0x221a,	#  SQUARE ROOT
    0x0084: 0x2592,	#  MEDIUM SHADE
    0x0085: 0x2500,	#  FORMS LIGHT HORIZONTAL
    0x0086: 0x2502,	#  FORMS LIGHT VERTICAL
    0x0087: 0x253c,	#  FORMS LIGHT VERTICAL AND HORIZONTAL
    0x0088: 0x2524,	#  FORMS LIGHT VERTICAL AND LEFT
    0x0089: 0x252c,	#  FORMS LIGHT DOWN AND HORIZONTAL
    0x008a: 0x251c,	#  FORMS LIGHT VERTICAL AND RIGHT
    0x008b: 0x2534,	#  FORMS LIGHT UP AND HORIZONTAL
    0x008c: 0x2510,	#  FORMS LIGHT DOWN AND LEFT
    0x008d: 0x250c,	#  FORMS LIGHT DOWN AND RIGHT
    0x008e: 0x2514,	#  FORMS LIGHT UP AND RIGHT
    0x008f: 0x2518,	#  FORMS LIGHT UP AND LEFT
    0x0090: 0x03b2,	#  GREEK SMALL BETA
    0x0091: 0x221e,	#  INFINITY
    0x0092: 0x03c6,	#  GREEK SMALL PHI
    0x0093: 0x00b1,	#  PLUS-OR-MINUS SIGN
    0x0094: 0x00bd,	#  FRACTION 1/2
    0x0095: 0x00bc,	#  FRACTION 1/4
    0x0096: 0x2248,	#  ALMOST EQUAL TO
    0x0097: 0x00ab,	#  LEFT POINTING GUILLEMET
    0x0098: 0x00bb,	#  RIGHT POINTING GUILLEMET
    0x0099: 0xfef7,	#  ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE ISOLATED FORM
    0x009a: 0xfef8,	#  ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE FINAL FORM
    0x009b: None,	#  UNDEFINED
    0x009c: None,	#  UNDEFINED
    0x009d: 0xfefb,	#  ARABIC LIGATURE LAM WITH ALEF ISOLATED FORM
    0x009e: 0xfefc,	#  ARABIC LIGATURE LAM WITH ALEF FINAL FORM
    0x009f: None,	#  UNDEFINED
    0x00a1: 0x00ad,	#  SOFT HYPHEN
    0x00a2: 0xfe82,	#  ARABIC LETTER ALEF WITH MADDA ABOVE FINAL FORM
    0x00a5: 0xfe84,	#  ARABIC LETTER ALEF WITH HAMZA ABOVE FINAL FORM
    0x00a6: None,	#  UNDEFINED
    0x00a7: None,	#  UNDEFINED
    0x00a8: 0xfe8e,	#  ARABIC LETTER ALEF FINAL FORM
    0x00a9: 0xfe8f,	#  ARABIC LETTER BEH ISOLATED FORM
    0x00aa: 0xfe95,	#  ARABIC LETTER TEH ISOLATED FORM
    0x00ab: 0xfe99,	#  ARABIC LETTER THEH ISOLATED FORM
    0x00ac: 0x060c,	#  ARABIC COMMA
    0x00ad: 0xfe9d,	#  ARABIC LETTER JEEM ISOLATED FORM
    0x00ae: 0xfea1,	#  ARABIC LETTER HAH ISOLATED FORM
    0x00af: 0xfea5,	#  ARABIC LETTER KHAH ISOLATED FORM
    0x00b0: 0x0660,	#  ARABIC-INDIC DIGIT ZERO
    0x00b1: 0x0661,	#  ARABIC-INDIC DIGIT ONE
    0x00b2: 0x0662,	#  ARABIC-INDIC DIGIT TWO
    0x00b3: 0x0663,	#  ARABIC-INDIC DIGIT THREE
    0x00b4: 0x0664,	#  ARABIC-INDIC DIGIT FOUR
    0x00b5: 0x0665,	#  ARABIC-INDIC DIGIT FIVE
    0x00b6: 0x0666,	#  ARABIC-INDIC DIGIT SIX
    0x00b7: 0x0667,	#  ARABIC-INDIC DIGIT SEVEN
    0x00b8: 0x0668,	#  ARABIC-INDIC DIGIT EIGHT
    0x00b9: 0x0669,	#  ARABIC-INDIC DIGIT NINE
    0x00ba: 0xfed1,	#  ARABIC LETTER FEH ISOLATED FORM
    0x00bb: 0x061b,	#  ARABIC SEMICOLON
    0x00bc: 0xfeb1,	#  ARABIC LETTER SEEN ISOLATED FORM
    0x00bd: 0xfeb5,	#  ARABIC LETTER SHEEN ISOLATED FORM
    0x00be: 0xfeb9,	#  ARABIC LETTER SAD ISOLATED FORM
    0x00bf: 0x061f,	#  ARABIC QUESTION MARK
    0x00c0: 0x00a2,	#  CENT SIGN
    0x00c1: 0xfe80,	#  ARABIC LETTER HAMZA ISOLATED FORM
    0x00c2: 0xfe81,	#  ARABIC LETTER ALEF WITH MADDA ABOVE ISOLATED FORM
    0x00c3: 0xfe83,	#  ARABIC LETTER ALEF WITH HAMZA ABOVE ISOLATED FORM
    0x00c4: 0xfe85,	#  ARABIC LETTER WAW WITH HAMZA ABOVE ISOLATED FORM
    0x00c5: 0xfeca,	#  ARABIC LETTER AIN FINAL FORM
    0x00c6: 0xfe8b,	#  ARABIC LETTER YEH WITH HAMZA ABOVE INITIAL FORM
    0x00c7: 0xfe8d,	#  ARABIC LETTER ALEF ISOLATED FORM
    0x00c8: 0xfe91,	#  ARABIC LETTER BEH INITIAL FORM
    0x00c9: 0xfe93,	#  ARABIC LETTER TEH MARBUTA ISOLATED FORM
    0x00ca: 0xfe97,	#  ARABIC LETTER TEH INITIAL FORM
    0x00cb: 0xfe9b,	#  ARABIC LETTER THEH INITIAL FORM
    0x00cc: 0xfe9f,	#  ARABIC LETTER JEEM INITIAL FORM
    0x00cd: 0xfea3,	#  ARABIC LETTER HAH INITIAL FORM
    0x00ce: 0xfea7,	#  ARABIC LETTER KHAH INITIAL FORM
    0x00cf: 0xfea9,	#  ARABIC LETTER DAL ISOLATED FORM
    0x00d0: 0xfeab,	#  ARABIC LETTER THAL ISOLATED FORM
    0x00d1: 0xfead,	#  ARABIC LETTER REH ISOLATED FORM
    0x00d2: 0xfeaf,	#  ARABIC LETTER ZAIN ISOLATED FORM
    0x00d3: 0xfeb3,	#  ARABIC LETTER SEEN INITIAL FORM
    0x00d4: 0xfeb7,	#  ARABIC LETTER SHEEN INITIAL FORM
    0x00d5: 0xfebb,	#  ARABIC LETTER SAD INITIAL FORM
    0x00d6: 0xfebf,	#  ARABIC LETTER DAD INITIAL FORM
    0x00d7: 0xfec1,	#  ARABIC LETTER TAH ISOLATED FORM
    0x00d8: 0xfec5,	#  ARABIC LETTER ZAH ISOLATED FORM
    0x00d9: 0xfecb,	#  ARABIC LETTER AIN INITIAL FORM
    0x00da: 0xfecf,	#  ARABIC LETTER GHAIN INITIAL FORM
    0x00db: 0x00a6,	#  BROKEN VERTICAL BAR
    0x00dc: 0x00ac,	#  NOT SIGN
    0x00dd: 0x00f7,	#  DIVISION SIGN
    0x00de: 0x00d7,	#  MULTIPLICATION SIGN
    0x00df: 0xfec9,	#  ARABIC LETTER AIN ISOLATED FORM
    0x00e0: 0x0640,	#  ARABIC TATWEEL
    0x00e1: 0xfed3,	#  ARABIC LETTER FEH INITIAL FORM
    0x00e2: 0xfed7,	#  ARABIC LETTER QAF INITIAL FORM
    0x00e3: 0xfedb,	#  ARABIC LETTER KAF INITIAL FORM
    0x00e4: 0xfedf,	#  ARABIC LETTER LAM INITIAL FORM
    0x00e5: 0xfee3,	#  ARABIC LETTER MEEM INITIAL FORM
    0x00e6: 0xfee7,	#  ARABIC LETTER NOON INITIAL FORM
    0x00e7: 0xfeeb,	#  ARABIC LETTER HEH INITIAL FORM
    0x00e8: 0xfeed,	#  ARABIC LETTER WAW ISOLATED FORM
    0x00e9: 0xfeef,	#  ARABIC LETTER ALEF MAKSURA ISOLATED FORM
    0x00ea: 0xfef3,	#  ARABIC LETTER YEH INITIAL FORM
    0x00eb: 0xfebd,	#  ARABIC LETTER DAD ISOLATED FORM
    0x00ec: 0xfecc,	#  ARABIC LETTER AIN MEDIAL FORM
    0x00ed: 0xfece,	#  ARABIC LETTER GHAIN FINAL FORM
    0x00ee: 0xfecd,	#  ARABIC LETTER GHAIN ISOLATED FORM
    0x00ef: 0xfee1,	#  ARABIC LETTER MEEM ISOLATED FORM
    0x00f0: 0xfe7d,	#  ARABIC SHADDA MEDIAL FORM
    0x00f1: 0x0651,	#  ARABIC SHADDAH
    0x00f2: 0xfee5,	#  ARABIC LETTER NOON ISOLATED FORM
    0x00f3: 0xfee9,	#  ARABIC LETTER HEH ISOLATED FORM
    0x00f4: 0xfeec,	#  ARABIC LETTER HEH MEDIAL FORM
    0x00f5: 0xfef0,	#  ARABIC LETTER ALEF MAKSURA FINAL FORM
    0x00f6: 0xfef2,	#  ARABIC LETTER YEH FINAL FORM
    0x00f7: 0xfed0,	#  ARABIC LETTER GHAIN MEDIAL FORM
    0x00f8: 0xfed5,	#  ARABIC LETTER QAF ISOLATED FORM
    0x00f9: 0xfef5,	#  ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE ISOLATED FORM
    0x00fa: 0xfef6,	#  ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE FINAL FORM
    0x00fb: 0xfedd,	#  ARABIC LETTER LAM ISOLATED FORM
    0x00fc: 0xfed9,	#  ARABIC LETTER KAF ISOLATED FORM
    0x00fd: 0xfef1,	#  ARABIC LETTER YEH ISOLATED FORM
    0x00fe: 0x25a0,	#  BLACK SQUARE
    0x00ff: None,	#  UNDEFINED
})

### Decoding Table

decoding_table = (
    u'\x00'	#  0x0000 -> NULL
    u'\x01'	#  0x0001 -> START OF HEADING
    u'\x02'	#  0x0002 -> START OF TEXT
    u'\x03'	#  0x0003 -> END OF TEXT
    u'\x04'	#  0x0004 -> END OF TRANSMISSION
    u'\x05'	#  0x0005 -> ENQUIRY
    u'\x06'	#  0x0006 -> ACKNOWLEDGE
    u'\x07'	#  0x0007 -> BELL
    u'\x08'	#  0x0008 -> BACKSPACE
    u'\t'	#  0x0009 -> HORIZONTAL TABULATION
    u'\n'	#  0x000a -> LINE FEED
    u'\x0b'	#  0x000b -> VERTICAL TABULATION
    u'\x0c'	#  0x000c -> FORM FEED
    u'\r'	#  0x000d -> CARRIAGE RETURN
    u'\x0e'	#  0x000e -> SHIFT OUT
    u'\x0f'	#  0x000f -> SHIFT IN
    u'\x10'	#  0x0010 -> DATA LINK ESCAPE
    u'\x11'	#  0x0011 -> DEVICE CONTROL ONE
    u'\x12'	#  0x0012 -> DEVICE CONTROL TWO
    u'\x13'	#  0x0013 -> DEVICE CONTROL THREE
    u'\x14'	#  0x0014 -> DEVICE CONTROL FOUR
    u'\x15'	#  0x0015 -> NEGATIVE ACKNOWLEDGE
    u'\x16'	#  0x0016 -> SYNCHRONOUS IDLE
    u'\x17'	#  0x0017 -> END OF TRANSMISSION BLOCK
    u'\x18'	#  0x0018 -> CANCEL
    u'\x19'	#  0x0019 -> END OF MEDIUM
    u'\x1a'	#  0x001a -> SUBSTITUTE
    u'\x1b'	#  0x001b -> ESCAPE
    u'\x1c'	#  0x001c -> FILE SEPARATOR
    u'\x1d'	#  0x001d -> GROUP SEPARATOR
    u'\x1e'	#  0x001e -> RECORD SEPARATOR
    u'\x1f'	#  0x001f -> UNIT SEPARATOR
    u' '	#  0x0020 -> SPACE
    u'!'	#  0x0021 -> EXCLAMATION MARK
    u'"'	#  0x0022 -> QUOTATION MARK
    u'#'	#  0x0023 -> NUMBER SIGN
    u'$'	#  0x0024 -> DOLLAR SIGN
    u'\u066a'	#  0x0025 -> ARABIC PERCENT SIGN
    u'&'	#  0x0026 -> AMPERSAND
    u"'"	#  0x0027 -> APOSTROPHE
    u'('	#  0x0028 -> LEFT PARENTHESIS
    u')'	#  0x0029 -> RIGHT PARENTHESIS
    u'*'	#  0x002a -> ASTERISK
    u'+'	#  0x002b -> PLUS SIGN
    u','	#  0x002c -> COMMA
    u'-'	#  0x002d -> HYPHEN-MINUS
    u'.'	#  0x002e -> FULL STOP
    u'/'	#  0x002f -> SOLIDUS
    u'0'	#  0x0030 -> DIGIT ZERO
    u'1'	#  0x0031 -> DIGIT ONE
    u'2'	#  0x0032 -> DIGIT TWO
    u'3'	#  0x0033 -> DIGIT THREE
    u'4'	#  0x0034 -> DIGIT FOUR
    u'5'	#  0x0035 -> DIGIT FIVE
    u'6'	#  0x0036 -> DIGIT SIX
    u'7'	#  0x0037 -> DIGIT SEVEN
    u'8'	#  0x0038 -> DIGIT EIGHT
    u'9'	#  0x0039 -> DIGIT NINE
    u':'	#  0x003a -> COLON
    u';'	#  0x003b -> SEMICOLON
    u'<'	#  0x003c -> LESS-THAN SIGN
    u'='	#  0x003d -> EQUALS SIGN
    u'>'	#  0x003e -> GREATER-THAN SIGN
    u'?'	#  0x003f -> QUESTION MARK
    u'@'	#  0x0040 -> COMMERCIAL AT
    u'A'	#  0x0041 -> LATIN CAPITAL LETTER A
    u'B'	#  0x0042 -> LATIN CAPITAL LETTER B
    u'C'	#  0x0043 -> LATIN CAPITAL LETTER C
    u'D'	#  0x0044 -> LATIN CAPITAL LETTER D
    u'E'	#  0x0045 -> LATIN CAPITAL LETTER E
    u'F'	#  0x0046 -> LATIN CAPITAL LETTER F
    u'G'	#  0x0047 -> LATIN CAPITAL LETTER G
    u'H'	#  0x0048 -> LATIN CAPITAL LETTER H
    u'I'	#  0x0049 -> LATIN CAPITAL LETTER I
    u'J'	#  0x004a -> LATIN CAPITAL LETTER J
    u'K'	#  0x004b -> LATIN CAPITAL LETTER K
    u'L'	#  0x004c -> LATIN CAPITAL LETTER L
    u'M'	#  0x004d -> LATIN CAPITAL LETTER M
    u'N'	#  0x004e -> LATIN CAPITAL LETTER N
    u'O'	#  0x004f -> LATIN CAPITAL LETTER O
    u'P'	#  0x0050 -> LATIN CAPITAL LETTER P
    u'Q'	#  0x0051 -> LATIN CAPITAL LETTER Q
    u'R'	#  0x0052 -> LATIN CAPITAL LETTER R
    u'S'	#  0x0053 -> LATIN CAPITAL LETTER S
    u'T'	#  0x0054 -> LATIN CAPITAL LETTER T
    u'U'	#  0x0055 -> LATIN CAPITAL LETTER U
    u'V'	#  0x0056 -> LATIN CAPITAL LETTER V
    u'W'	#  0x0057 -> LATIN CAPITAL LETTER W
    u'X'	#  0x0058 -> LATIN CAPITAL LETTER X
    u'Y'	#  0x0059 -> LATIN CAPITAL LETTER Y
    u'Z'	#  0x005a -> LATIN CAPITAL LETTER Z
    u'['	#  0x005b -> LEFT SQUARE BRACKET
    u'\\'	#  0x005c -> REVERSE SOLIDUS
    u']'	#  0x005d -> RIGHT SQUARE BRACKET
    u'^'	#  0x005e -> CIRCUMFLEX ACCENT
    u'_'	#  0x005f -> LOW LINE
    u'`'	#  0x0060 -> GRAVE ACCENT
    u'a'	#  0x0061 -> LATIN SMALL LETTER A
    u'b'	#  0x0062 -> LATIN SMALL LETTER B
    u'c'	#  0x0063 -> LATIN SMALL LETTER C
    u'd'	#  0x0064 -> LATIN SMALL LETTER D
    u'e'	#  0x0065 -> LATIN SMALL LETTER E
    u'f'	#  0x0066 -> LATIN SMALL LETTER F
    u'g'	#  0x0067 -> LATIN SMALL LETTER G
    u'h'	#  0x0068 -> LATIN SMALL LETTER H
    u'i'	#  0x0069 -> LATIN SMALL LETTER I
    u'j'	#  0x006a -> LATIN SMALL LETTER J
    u'k'	#  0x006b -> LATIN SMALL LETTER K
    u'l'	#  0x006c -> LATIN SMALL LETTER L
    u'm'	#  0x006d -> LATIN SMALL LETTER M
    u'n'	#  0x006e -> LATIN SMALL LETTER N
    u'o'	#  0x006f -> LATIN SMALL LETTER O
    u'p'	#  0x0070 -> LATIN SMALL LETTER P
    u'q'	#  0x0071 -> LATIN SMALL LETTER Q
    u'r'	#  0x0072 -> LATIN SMALL LETTER R
    u's'	#  0x0073 -> LATIN SMALL LETTER S
    u't'	#  0x0074 -> LATIN SMALL LETTER T
    u'u'	#  0x0075 -> LATIN SMALL LETTER U
    u'v'	#  0x0076 -> LATIN SMALL LETTER V
    u'w'	#  0x0077 -> LATIN SMALL LETTER W
    u'x'	#  0x0078 -> LATIN SMALL LETTER X
    u'y'	#  0x0079 -> LATIN SMALL LETTER Y
    u'z'	#  0x007a -> LATIN SMALL LETTER Z
    u'{'	#  0x007b -> LEFT CURLY BRACKET
    u'|'	#  0x007c -> VERTICAL LINE
    u'}'	#  0x007d -> RIGHT CURLY BRACKET
    u'~'	#  0x007e -> TILDE
    u'\x7f'	#  0x007f -> DELETE
    u'\xb0'	#  0x0080 -> DEGREE SIGN
    u'\xb7'	#  0x0081 -> MIDDLE DOT
    u'\u2219'	#  0x0082 -> BULLET OPERATOR
    u'\u221a'	#  0x0083 -> SQUARE ROOT
    u'\u2592'	#  0x0084 -> MEDIUM SHADE
    u'\u2500'	#  0x0085 -> FORMS LIGHT HORIZONTAL
    u'\u2502'	#  0x0086 -> FORMS LIGHT VERTICAL
    u'\u253c'	#  0x0087 -> FORMS LIGHT VERTICAL AND HORIZONTAL
    u'\u2524'	#  0x0088 -> FORMS LIGHT VERTICAL AND LEFT
    u'\u252c'	#  0x0089 -> FORMS LIGHT DOWN AND HORIZONTAL
    u'\u251c'	#  0x008a -> FORMS LIGHT VERTICAL AND RIGHT
    u'\u2534'	#  0x008b -> FORMS LIGHT UP AND HORIZONTAL
    u'\u2510'	#  0x008c -> FORMS LIGHT DOWN AND LEFT
    u'\u250c'	#  0x008d -> FORMS LIGHT DOWN AND RIGHT
    u'\u2514'	#  0x008e -> FORMS LIGHT UP AND RIGHT
    u'\u2518'	#  0x008f -> FORMS LIGHT UP AND LEFT
    u'\u03b2'	#  0x0090 -> GREEK SMALL BETA
    u'\u221e'	#  0x0091 -> INFINITY
    u'\u03c6'	#  0x0092 -> GREEK SMALL PHI
    u'\xb1'	#  0x0093 -> PLUS-OR-MINUS SIGN
    u'\xbd'	#  0x0094 -> FRACTION 1/2
    u'\xbc'	#  0x0095 -> FRACTION 1/4
    u'\u2248'	#  0x0096 -> ALMOST EQUAL TO
    u'\xab'	#  0x0097 -> LEFT POINTING GUILLEMET
    u'\xbb'	#  0x0098 -> RIGHT POINTING GUILLEMET
    u'\ufef7'	#  0x0099 -> ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE ISOLATED FORM
    u'\ufef8'	#  0x009a -> ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE FINAL FORM
    u'\ufffe'	#  0x009b -> UNDEFINED
    u'\ufffe'	#  0x009c -> UNDEFINED
    u'\ufefb'	#  0x009d -> ARABIC LIGATURE LAM WITH ALEF ISOLATED FORM
    u'\ufefc'	#  0x009e -> ARABIC LIGATURE LAM WITH ALEF FINAL FORM
    u'\ufffe'	#  0x009f -> UNDEFINED
    u'\xa0'	#  0x00a0 -> NON-BREAKING SPACE
    u'\xad'	#  0x00a1 -> SOFT HYPHEN
    u'\ufe82'	#  0x00a2 -> ARABIC LETTER ALEF WITH MADDA ABOVE FINAL FORM
    u'\xa3'	#  0x00a3 -> POUND SIGN
    u'\xa4'	#  0x00a4 -> CURRENCY SIGN
    u'\ufe84'	#  0x00a5 -> ARABIC LETTER ALEF WITH HAMZA ABOVE FINAL FORM
    u'\ufffe'	#  0x00a6 -> UNDEFINED
    u'\ufffe'	#  0x00a7 -> UNDEFINED
    u'\ufe8e'	#  0x00a8 -> ARABIC LETTER ALEF FINAL FORM
    u'\ufe8f'	#  0x00a9 -> ARABIC LETTER BEH ISOLATED FORM
    u'\ufe95'	#  0x00aa -> ARABIC LETTER TEH ISOLATED FORM
    u'\ufe99'	#  0x00ab -> ARABIC LETTER THEH ISOLATED FORM
    u'\u060c'	#  0x00ac -> ARABIC COMMA
    u'\ufe9d'	#  0x00ad -> ARABIC LETTER JEEM ISOLATED FORM
    u'\ufea1'	#  0x00ae -> ARABIC LETTER HAH ISOLATED FORM
    u'\ufea5'	#  0x00af -> ARABIC LETTER KHAH ISOLATED FORM
    u'\u0660'	#  0x00b0 -> ARABIC-INDIC DIGIT ZERO
    u'\u0661'	#  0x00b1 -> ARABIC-INDIC DIGIT ONE
    u'\u0662'	#  0x00b2 -> ARABIC-INDIC DIGIT TWO
    u'\u0663'	#  0x00b3 -> ARABIC-INDIC DIGIT THREE
    u'\u0664'	#  0x00b4 -> ARABIC-INDIC DIGIT FOUR
    u'\u0665'	#  0x00b5 -> ARABIC-INDIC DIGIT FIVE
    u'\u0666'	#  0x00b6 -> ARABIC-INDIC DIGIT SIX
    u'\u0667'	#  0x00b7 -> ARABIC-INDIC DIGIT SEVEN
    u'\u0668'	#  0x00b8 -> ARABIC-INDIC DIGIT EIGHT
    u'\u0669'	#  0x00b9 -> ARABIC-INDIC DIGIT NINE
    u'\ufed1'	#  0x00ba -> ARABIC LETTER FEH ISOLATED FORM
    u'\u061b'	#  0x00bb -> ARABIC SEMICOLON
    u'\ufeb1'	#  0x00bc -> ARABIC LETTER SEEN ISOLATED FORM
    u'\ufeb5'	#  0x00bd -> ARABIC LETTER SHEEN ISOLATED FORM
    u'\ufeb9'	#  0x00be -> ARABIC LETTER SAD ISOLATED FORM
    u'\u061f'	#  0x00bf -> ARABIC QUESTION MARK
    u'\xa2'	#  0x00c0 -> CENT SIGN
    u'\ufe80'	#  0x00c1 -> ARABIC LETTER HAMZA ISOLATED FORM
    u'\ufe81'	#  0x00c2 -> ARABIC LETTER ALEF WITH MADDA ABOVE ISOLATED FORM
    u'\ufe83'	#  0x00c3 -> ARABIC LETTER ALEF WITH HAMZA ABOVE ISOLATED FORM
    u'\ufe85'	#  0x00c4 -> ARABIC LETTER WAW WITH HAMZA ABOVE ISOLATED FORM
    u'\ufeca'	#  0x00c5 -> ARABIC LETTER AIN FINAL FORM
    u'\ufe8b'	#  0x00c6 -> ARABIC LETTER YEH WITH HAMZA ABOVE INITIAL FORM
    u'\ufe8d'	#  0x00c7 -> ARABIC LETTER ALEF ISOLATED FORM
    u'\ufe91'	#  0x00c8 -> ARABIC LETTER BEH INITIAL FORM
    u'\ufe93'	#  0x00c9 -> ARABIC LETTER TEH MARBUTA ISOLATED FORM
    u'\ufe97'	#  0x00ca -> ARABIC LETTER TEH INITIAL FORM
    u'\ufe9b'	#  0x00cb -> ARABIC LETTER THEH INITIAL FORM
    u'\ufe9f'	#  0x00cc -> ARABIC LETTER JEEM INITIAL FORM
    u'\ufea3'	#  0x00cd -> ARABIC LETTER HAH INITIAL FORM
    u'\ufea7'	#  0x00ce -> ARABIC LETTER KHAH INITIAL FORM
    u'\ufea9'	#  0x00cf -> ARABIC LETTER DAL ISOLATED FORM
    u'\ufeab'	#  0x00d0 -> ARABIC LETTER THAL ISOLATED FORM
    u'\ufead'	#  0x00d1 -> ARABIC LETTER REH ISOLATED FORM
    u'\ufeaf'	#  0x00d2 -> ARABIC LETTER ZAIN ISOLATED FORM
    u'\ufeb3'	#  0x00d3 -> ARABIC LETTER SEEN INITIAL FORM
    u'\ufeb7'	#  0x00d4 -> ARABIC LETTER SHEEN INITIAL FORM
    u'\ufebb'	#  0x00d5 -> ARABIC LETTER SAD INITIAL FORM
    u'\ufebf'	#  0x00d6 -> ARABIC LETTER DAD INITIAL FORM
    u'\ufec1'	#  0x00d7 -> ARABIC LETTER TAH ISOLATED FORM
    u'\ufec5'	#  0x00d8 -> ARABIC LETTER ZAH ISOLATED FORM
    u'\ufecb'	#  0x00d9 -> ARABIC LETTER AIN INITIAL FORM
    u'\ufecf'	#  0x00da -> ARABIC LETTER GHAIN INITIAL FORM
    u'\xa6'	#  0x00db -> BROKEN VERTICAL BAR
    u'\xac'	#  0x00dc -> NOT SIGN
    u'\xf7'	#  0x00dd -> DIVISION SIGN
    u'\xd7'	#  0x00de -> MULTIPLICATION SIGN
    u'\ufec9'	#  0x00df -> ARABIC LETTER AIN ISOLATED FORM
    u'\u0640'	#  0x00e0 -> ARABIC TATWEEL
    u'\ufed3'	#  0x00e1 -> ARABIC LETTER FEH INITIAL FORM
    u'\ufed7'	#  0x00e2 -> ARABIC LETTER QAF INITIAL FORM
    u'\ufedb'	#  0x00e3 -> ARABIC LETTER KAF INITIAL FORM
    u'\ufedf'	#  0x00e4 -> ARABIC LETTER LAM INITIAL FORM
    u'\ufee3'	#  0x00e5 -> ARABIC LETTER MEEM INITIAL FORM
    u'\ufee7'	#  0x00e6 -> ARABIC LETTER NOON INITIAL FORM
    u'\ufeeb'	#  0x00e7 -> ARABIC LETTER HEH INITIAL FORM
    u'\ufeed'	#  0x00e8 -> ARABIC LETTER WAW ISOLATED FORM
    u'\ufeef'	#  0x00e9 -> ARABIC LETTER ALEF MAKSURA ISOLATED FORM
    u'\ufef3'	#  0x00ea -> ARABIC LETTER YEH INITIAL FORM
    u'\ufebd'	#  0x00eb -> ARABIC LETTER DAD ISOLATED FORM
    u'\ufecc'	#  0x00ec -> ARABIC LETTER AIN MEDIAL FORM
    u'\ufece'	#  0x00ed -> ARABIC LETTER GHAIN FINAL FORM
    u'\ufecd'	#  0x00ee -> ARABIC LETTER GHAIN ISOLATED FORM
    u'\ufee1'	#  0x00ef -> ARABIC LETTER MEEM ISOLATED FORM
    u'\ufe7d'	#  0x00f0 -> ARABIC SHADDA MEDIAL FORM
    u'\u0651'	#  0x00f1 -> ARABIC SHADDAH
    u'\ufee5'	#  0x00f2 -> ARABIC LETTER NOON ISOLATED FORM
    u'\ufee9'	#  0x00f3 -> ARABIC LETTER HEH ISOLATED FORM
    u'\ufeec'	#  0x00f4 -> ARABIC LETTER HEH MEDIAL FORM
    u'\ufef0'	#  0x00f5 -> ARABIC LETTER ALEF MAKSURA FINAL FORM
    u'\ufef2'	#  0x00f6 -> ARABIC LETTER YEH FINAL FORM
    u'\ufed0'	#  0x00f7 -> ARABIC LETTER GHAIN MEDIAL FORM
    u'\ufed5'	#  0x00f8 -> ARABIC LETTER QAF ISOLATED FORM
    u'\ufef5'	#  0x00f9 -> ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE ISOLATED FORM
    u'\ufef6'	#  0x00fa -> ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE FINAL FORM
    u'\ufedd'	#  0x00fb -> ARABIC LETTER LAM ISOLATED FORM
    u'\ufed9'	#  0x00fc -> ARABIC LETTER KAF ISOLATED FORM
    u'\ufef1'	#  0x00fd -> ARABIC LETTER YEH ISOLATED FORM
    u'\u25a0'	#  0x00fe -> BLACK SQUARE
    u'\ufffe'	#  0x00ff -> UNDEFINED
)

### Encoding Map

encoding_map = {
    0x0000: 0x0000,	#  NULL
    0x0001: 0x0001,	#  START OF HEADING
    0x0002: 0x0002,	#  START OF TEXT
    0x0003: 0x0003,	#  END OF TEXT
    0x0004: 0x0004,	#  END OF TRANSMISSION
    0x0005: 0x0005,	#  ENQUIRY
    0x0006: 0x0006,	#  ACKNOWLEDGE
    0x0007: 0x0007,	#  BELL
    0x0008: 0x0008,	#  BACKSPACE
    0x0009: 0x0009,	#  HORIZONTAL TABULATION
    0x000a: 0x000a,	#  LINE FEED
    0x000b: 0x000b,	#  VERTICAL TABULATION
    0x000c: 0x000c,	#  FORM FEED
    0x000d: 0x000d,	#  CARRIAGE RETURN
    0x000e: 0x000e,	#  SHIFT OUT
    0x000f: 0x000f,	#  SHIFT IN
    0x0010: 0x0010,	#  DATA LINK ESCAPE
    0x0011: 0x0011,	#  DEVICE CONTROL ONE
    0x0012: 0x0012,	#  DEVICE CONTROL TWO
    0x0013: 0x0013,	#  DEVICE CONTROL THREE
    0x0014: 0x0014,	#  DEVICE CONTROL FOUR
    0x0015: 0x0015,	#  NEGATIVE ACKNOWLEDGE
    0x0016: 0x0016,	#  SYNCHRONOUS IDLE
    0x0017: 0x0017,	#  END OF TRANSMISSION BLOCK
    0x0018: 0x0018,	#  CANCEL
    0x0019: 0x0019,	#  END OF MEDIUM
    0x001a: 0x001a,	#  SUBSTITUTE
    0x001b: 0x001b,	#  ESCAPE
    0x001c: 0x001c,	#  FILE SEPARATOR
    0x001d: 0x001d,	#  GROUP SEPARATOR
    0x001e: 0x001e,	#  RECORD SEPARATOR
    0x001f: 0x001f,	#  UNIT SEPARATOR
    0x0020: 0x0020,	#  SPACE
    0x0021: 0x0021,	#  EXCLAMATION MARK
    0x0022: 0x0022,	#  QUOTATION MARK
    0x0023: 0x0023,	#  NUMBER SIGN
    0x0024: 0x0024,	#  DOLLAR SIGN
    0x0026: 0x0026,	#  AMPERSAND
    0x0027: 0x0027,	#  APOSTROPHE
    0x0028: 0x0028,	#  LEFT PARENTHESIS
    0x0029: 0x0029,	#  RIGHT PARENTHESIS
    0x002a: 0x002a,	#  ASTERISK
    0x002b: 0x002b,	#  PLUS SIGN
    0x002c: 0x002c,	#  COMMA
    0x002d: 0x002d,	#  HYPHEN-MINUS
    0x002e: 0x002e,	#  FULL STOP
    0x002f: 0x002f,	#  SOLIDUS
    0x0030: 0x0030,	#  DIGIT ZERO
    0x0031: 0x0031,	#  DIGIT ONE
    0x0032: 0x0032,	#  DIGIT TWO
    0x0033: 0x0033,	#  DIGIT THREE
    0x0034: 0x0034,	#  DIGIT FOUR
    0x0035: 0x0035,	#  DIGIT FIVE
    0x0036: 0x0036,	#  DIGIT SIX
    0x0037: 0x0037,	#  DIGIT SEVEN
    0x0038: 0x0038,	#  DIGIT EIGHT
    0x0039: 0x0039,	#  DIGIT NINE
    0x003a: 0x003a,	#  COLON
    0x003b: 0x003b,	#  SEMICOLON
    0x003c: 0x003c,	#  LESS-THAN SIGN
    0x003d: 0x003d,	#  EQUALS SIGN
    0x003e: 0x003e,	#  GREATER-THAN SIGN
    0x003f: 0x003f,	#  QUESTION MARK
    0x0040: 0x0040,	#  COMMERCIAL AT
    0x0041: 0x0041,	#  LATIN CAPITAL LETTER A
    0x0042: 0x0042,	#  LATIN CAPITAL LETTER B
    0x0043: 0x0043,	#  LATIN CAPITAL LETTER C
    0x0044: 0x0044,	#  LATIN CAPITAL LETTER D
    0x0045: 0x0045,	#  LATIN CAPITAL LETTER E
    0x0046: 0x0046,	#  LATIN CAPITAL LETTER F
    0x0047: 0x0047,	#  LATIN CAPITAL LETTER G
    0x0048: 0x0048,	#  LATIN CAPITAL LETTER H
    0x0049: 0x0049,	#  LATIN CAPITAL LETTER I
    0x004a: 0x004a,	#  LATIN CAPITAL LETTER J
    0x004b: 0x004b,	#  LATIN CAPITAL LETTER K
    0x004c: 0x004c,	#  LATIN CAPITAL LETTER L
    0x004d: 0x004d,	#  LATIN CAPITAL LETTER M
    0x004e: 0x004e,	#  LATIN CAPITAL LETTER N
    0x004f: 0x004f,	#  LATIN CAPITAL LETTER O
    0x0050: 0x0050,	#  LATIN CAPITAL LETTER P
    0x0051: 0x0051,	#  LATIN CAPITAL LETTER Q
    0x0052: 0x0052,	#  LATIN CAPITAL LETTER R
    0x0053: 0x0053,	#  LATIN CAPITAL LETTER S
    0x0054: 0x0054,	#  LATIN CAPITAL LETTER T
    0x0055: 0x0055,	#  LATIN CAPITAL LETTER U
    0x0056: 0x0056,	#  LATIN CAPITAL LETTER V
    0x0057: 0x0057,	#  LATIN CAPITAL LETTER W
    0x0058: 0x0058,	#  LATIN CAPITAL LETTER X
    0x0059: 0x0059,	#  LATIN CAPITAL LETTER Y
    0x005a: 0x005a,	#  LATIN CAPITAL LETTER Z
    0x005b: 0x005b,	#  LEFT SQUARE BRACKET
    0x005c: 0x005c,	#  REVERSE SOLIDUS
    0x005d: 0x005d,	#  RIGHT SQUARE BRACKET
    0x005e: 0x005e,	#  CIRCUMFLEX ACCENT
    0x005f: 0x005f,	#  LOW LINE
    0x0060: 0x0060,	#  GRAVE ACCENT
    0x0061: 0x0061,	#  LATIN SMALL LETTER A
    0x0062: 0x0062,	#  LATIN SMALL LETTER B
    0x0063: 0x0063,	#  LATIN SMALL LETTER C
    0x0064: 0x0064,	#  LATIN SMALL LETTER D
    0x0065: 0x0065,	#  LATIN SMALL LETTER E
    0x0066: 0x0066,	#  LATIN SMALL LETTER F
    0x0067: 0x0067,	#  LATIN SMALL LETTER G
    0x0068: 0x0068,	#  LATIN SMALL LETTER H
    0x0069: 0x0069,	#  LATIN SMALL LETTER I
    0x006a: 0x006a,	#  LATIN SMALL LETTER J
    0x006b: 0x006b,	#  LATIN SMALL LETTER K
    0x006c: 0x006c,	#  LATIN SMALL LETTER L
    0x006d: 0x006d,	#  LATIN SMALL LETTER M
    0x006e: 0x006e,	#  LATIN SMALL LETTER N
    0x006f: 0x006f,	#  LATIN SMALL LETTER O
    0x0070: 0x0070,	#  LATIN SMALL LETTER P
    0x0071: 0x0071,	#  LATIN SMALL LETTER Q
    0x0072: 0x0072,	#  LATIN SMALL LETTER R
    0x0073: 0x0073,	#  LATIN SMALL LETTER S
    0x0074: 0x0074,	#  LATIN SMALL LETTER T
    0x0075: 0x0075,	#  LATIN SMALL LETTER U
    0x0076: 0x0076,	#  LATIN SMALL LETTER V
    0x0077: 0x0077,	#  LATIN SMALL LETTER W
    0x0078: 0x0078,	#  LATIN SMALL LETTER X
    0x0079: 0x0079,	#  LATIN SMALL LETTER Y
    0x007a: 0x007a,	#  LATIN SMALL LETTER Z
    0x007b: 0x007b,	#  LEFT CURLY BRACKET
    0x007c: 0x007c,	#  VERTICAL LINE
    0x007d: 0x007d,	#  RIGHT CURLY BRACKET
    0x007e: 0x007e,	#  TILDE
    0x007f: 0x007f,	#  DELETE
    0x00a0: 0x00a0,	#  NON-BREAKING SPACE
    0x00a2: 0x00c0,	#  CENT SIGN
    0x00a3: 0x00a3,	#  POUND SIGN
    0x00a4: 0x00a4,	#  CURRENCY SIGN
    0x00a6: 0x00db,	#  BROKEN VERTICAL BAR
    0x00ab: 0x0097,	#  LEFT POINTING GUILLEMET
    0x00ac: 0x00dc,	#  NOT SIGN
    0x00ad: 0x00a1,	#  SOFT HYPHEN
    0x00b0: 0x0080,	#  DEGREE SIGN
    0x00b1: 0x0093,	#  PLUS-OR-MINUS SIGN
    0x00b7: 0x0081,	#  MIDDLE DOT
    0x00bb: 0x0098,	#  RIGHT POINTING GUILLEMET
    0x00bc: 0x0095,	#  FRACTION 1/4
    0x00bd: 0x0094,	#  FRACTION 1/2
    0x00d7: 0x00de,	#  MULTIPLICATION SIGN
    0x00f7: 0x00dd,	#  DIVISION SIGN
    0x03b2: 0x0090,	#  GREEK SMALL BETA
    0x03c6: 0x0092,	#  GREEK SMALL PHI
    0x060c: 0x00ac,	#  ARABIC COMMA
    0x061b: 0x00bb,	#  ARABIC SEMICOLON
    0x061f: 0x00bf,	#  ARABIC QUESTION MARK
    0x0640: 0x00e0,	#  ARABIC TATWEEL
    0x0651: 0x00f1,	#  ARABIC SHADDAH
    0x0660: 0x00b0,	#  ARABIC-INDIC DIGIT ZERO
    0x0661: 0x00b1,	#  ARABIC-INDIC DIGIT ONE
    0x0662: 0x00b2,	#  ARABIC-INDIC DIGIT TWO
    0x0663: 0x00b3,	#  ARABIC-INDIC DIGIT THREE
    0x0664: 0x00b4,	#  ARABIC-INDIC DIGIT FOUR
    0x0665: 0x00b5,	#  ARABIC-INDIC DIGIT FIVE
    0x0666: 0x00b6,	#  ARABIC-INDIC DIGIT SIX
    0x0667: 0x00b7,	#  ARABIC-INDIC DIGIT SEVEN
    0x0668: 0x00b8,	#  ARABIC-INDIC DIGIT EIGHT
    0x0669: 0x00b9,	#  ARABIC-INDIC DIGIT NINE
    0x066a: 0x0025,	#  ARABIC PERCENT SIGN
    0x2219: 0x0082,	#  BULLET OPERATOR
    0x221a: 0x0083,	#  SQUARE ROOT
    0x221e: 0x0091,	#  INFINITY
    0x2248: 0x0096,	#  ALMOST EQUAL TO
    0x2500: 0x0085,	#  FORMS LIGHT HORIZONTAL
    0x2502: 0x0086,	#  FORMS LIGHT VERTICAL
    0x250c: 0x008d,	#  FORMS LIGHT DOWN AND RIGHT
    0x2510: 0x008c,	#  FORMS LIGHT DOWN AND LEFT
    0x2514: 0x008e,	#  FORMS LIGHT UP AND RIGHT
    0x2518: 0x008f,	#  FORMS LIGHT UP AND LEFT
    0x251c: 0x008a,	#  FORMS LIGHT VERTICAL AND RIGHT
    0x2524: 0x0088,	#  FORMS LIGHT VERTICAL AND LEFT
    0x252c: 0x0089,	#  FORMS LIGHT DOWN AND HORIZONTAL
    0x2534: 0x008b,	#  FORMS LIGHT UP AND HORIZONTAL
    0x253c: 0x0087,	#  FORMS LIGHT VERTICAL AND HORIZONTAL
    0x2592: 0x0084,	#  MEDIUM SHADE
    0x25a0: 0x00fe,	#  BLACK SQUARE
    0xfe7d: 0x00f0,	#  ARABIC SHADDA MEDIAL FORM
    0xfe80: 0x00c1,	#  ARABIC LETTER HAMZA ISOLATED FORM
    0xfe81: 0x00c2,	#  ARABIC LETTER ALEF WITH MADDA ABOVE ISOLATED FORM
    0xfe82: 0x00a2,	#  ARABIC LETTER ALEF WITH MADDA ABOVE FINAL FORM
    0xfe83: 0x00c3,	#  ARABIC LETTER ALEF WITH HAMZA ABOVE ISOLATED FORM
    0xfe84: 0x00a5,	#  ARABIC LETTER ALEF WITH HAMZA ABOVE FINAL FORM
    0xfe85: 0x00c4,	#  ARABIC LETTER WAW WITH HAMZA ABOVE ISOLATED FORM
    0xfe8b: 0x00c6,	#  ARABIC LETTER YEH WITH HAMZA ABOVE INITIAL FORM
    0xfe8d: 0x00c7,	#  ARABIC LETTER ALEF ISOLATED FORM
    0xfe8e: 0x00a8,	#  ARABIC LETTER ALEF FINAL FORM
    0xfe8f: 0x00a9,	#  ARABIC LETTER BEH ISOLATED FORM
    0xfe91: 0x00c8,	#  ARABIC LETTER BEH INITIAL FORM
    0xfe93: 0x00c9,	#  ARABIC LETTER TEH MARBUTA ISOLATED FORM
    0xfe95: 0x00aa,	#  ARABIC LETTER TEH ISOLATED FORM
    0xfe97: 0x00ca,	#  ARABIC LETTER TEH INITIAL FORM
    0xfe99: 0x00ab,	#  ARABIC LETTER THEH ISOLATED FORM
    0xfe9b: 0x00cb,	#  ARABIC LETTER THEH INITIAL FORM
    0xfe9d: 0x00ad,	#  ARABIC LETTER JEEM ISOLATED FORM
    0xfe9f: 0x00cc,	#  ARABIC LETTER JEEM INITIAL FORM
    0xfea1: 0x00ae,	#  ARABIC LETTER HAH ISOLATED FORM
    0xfea3: 0x00cd,	#  ARABIC LETTER HAH INITIAL FORM
    0xfea5: 0x00af,	#  ARABIC LETTER KHAH ISOLATED FORM
    0xfea7: 0x00ce,	#  ARABIC LETTER KHAH INITIAL FORM
    0xfea9: 0x00cf,	#  ARABIC LETTER DAL ISOLATED FORM
    0xfeab: 0x00d0,	#  ARABIC LETTER THAL ISOLATED FORM
    0xfead: 0x00d1,	#  ARABIC LETTER REH ISOLATED FORM
    0xfeaf: 0x00d2,	#  ARABIC LETTER ZAIN ISOLATED FORM
    0xfeb1: 0x00bc,	#  ARABIC LETTER SEEN ISOLATED FORM
    0xfeb3: 0x00d3,	#  ARABIC LETTER SEEN INITIAL FORM
    0xfeb5: 0x00bd,	#  ARABIC LETTER SHEEN ISOLATED FORM
    0xfeb7: 0x00d4,	#  ARABIC LETTER SHEEN INITIAL FORM
    0xfeb9: 0x00be,	#  ARABIC LETTER SAD ISOLATED FORM
    0xfebb: 0x00d5,	#  ARABIC LETTER SAD INITIAL FORM
    0xfebd: 0x00eb,	#  ARABIC LETTER DAD ISOLATED FORM
    0xfebf: 0x00d6,	#  ARABIC LETTER DAD INITIAL FORM
    0xfec1: 0x00d7,	#  ARABIC LETTER TAH ISOLATED FORM
    0xfec5: 0x00d8,	#  ARABIC LETTER ZAH ISOLATED FORM
    0xfec9: 0x00df,	#  ARABIC LETTER AIN ISOLATED FORM
    0xfeca: 0x00c5,	#  ARABIC LETTER AIN FINAL FORM
    0xfecb: 0x00d9,	#  ARABIC LETTER AIN INITIAL FORM
    0xfecc: 0x00ec,	#  ARABIC LETTER AIN MEDIAL FORM
    0xfecd: 0x00ee,	#  ARABIC LETTER GHAIN ISOLATED FORM
    0xfece: 0x00ed,	#  ARABIC LETTER GHAIN FINAL FORM
    0xfecf: 0x00da,	#  ARABIC LETTER GHAIN INITIAL FORM
    0xfed0: 0x00f7,	#  ARABIC LETTER GHAIN MEDIAL FORM
    0xfed1: 0x00ba,	#  ARABIC LETTER FEH ISOLATED FORM
    0xfed3: 0x00e1,	#  ARABIC LETTER FEH INITIAL FORM
    0xfed5: 0x00f8,	#  ARABIC LETTER QAF ISOLATED FORM
    0xfed7: 0x00e2,	#  ARABIC LETTER QAF INITIAL FORM
    0xfed9: 0x00fc,	#  ARABIC LETTER KAF ISOLATED FORM
    0xfedb: 0x00e3,	#  ARABIC LETTER KAF INITIAL FORM
    0xfedd: 0x00fb,	#  ARABIC LETTER LAM ISOLATED FORM
    0xfedf: 0x00e4,	#  ARABIC LETTER LAM INITIAL FORM
    0xfee1: 0x00ef,	#  ARABIC LETTER MEEM ISOLATED FORM
    0xfee3: 0x00e5,	#  ARABIC LETTER MEEM INITIAL FORM
    0xfee5: 0x00f2,	#  ARABIC LETTER NOON ISOLATED FORM
    0xfee7: 0x00e6,	#  ARABIC LETTER NOON INITIAL FORM
    0xfee9: 0x00f3,	#  ARABIC LETTER HEH ISOLATED FORM
    0xfeeb: 0x00e7,	#  ARABIC LETTER HEH INITIAL FORM
    0xfeec: 0x00f4,	#  ARABIC LETTER HEH MEDIAL FORM
    0xfeed: 0x00e8,	#  ARABIC LETTER WAW ISOLATED FORM
    0xfeef: 0x00e9,	#  ARABIC LETTER ALEF MAKSURA ISOLATED FORM
    0xfef0: 0x00f5,	#  ARABIC LETTER ALEF MAKSURA FINAL FORM
    0xfef1: 0x00fd,	#  ARABIC LETTER YEH ISOLATED FORM
    0xfef2: 0x00f6,	#  ARABIC LETTER YEH FINAL FORM
    0xfef3: 0x00ea,	#  ARABIC LETTER YEH INITIAL FORM
    0xfef5: 0x00f9,	#  ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE ISOLATED FORM
    0xfef6: 0x00fa,	#  ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE FINAL FORM
    0xfef7: 0x0099,	#  ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE ISOLATED FORM
    0xfef8: 0x009a,	#  ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE FINAL FORM
    0xfefb: 0x009d,	#  ARABIC LIGATURE LAM WITH ALEF ISOLATED FORM
    0xfefc: 0x009e,	#  ARABIC LIGATURE LAM WITH ALEF FINAL FORM
}