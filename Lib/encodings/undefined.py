""" Python 'undefined' Codec

    This codec will always raise a ValueError exception when being
    used. It is intended for use by the site.py file to switch off
    automatic string to Unicode coercion.

Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.

"""
import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):
        raise UnicodeError, "undefined encoding"

    def decode(self,input,errors='strict'):
        raise UnicodeError, "undefined encoding"

class StreamWriter(Codec,codecs.StreamWriter):
    pass
        
class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():

    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)
