""" Python 'utf-16-be' Codec


Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.

"""
import codecs

### Codec APIs

class Codec(codecs.Codec):

    # Note: Binding these as C functions will result in the class not
    # converting them to methods. This is intended.
    encode = codecs.utf_16_be_encode
    decode = codecs.utf_16_be_decode

class StreamWriter(Codec,codecs.StreamWriter):
    pass

class StreamReader(Codec,codecs.StreamReader):

    def readline(self, size=None):
        raise NotImplementedError, '.readline() is not implemented for UTF-16-BE'

### encodings module API

def getregentry():

    return (Codec.encode,Codec.decode,StreamReader,StreamWriter)
