# Module 'sunaudio' -- interpret sun audio headers

MAGIC = '.snd'

error = 'sunaudio sound header conversion error'


# convert a 4-char value to integer

def get_long_be(s):
	return (ord(s[0])<<24) | (ord(s[1])<<16) | (ord(s[2])<<8) | ord(s[3])


# read a sound header from an open file

def gethdr(fp):
	if fp.read(4) <> MAGIC:
		raise error, 'gethdr: bad magic word'
	hdr_size = get_long_be(fp.read(4))
	data_size = get_long_be(fp.read(4))
	encoding = get_long_be(fp.read(4))
	sample_rate = get_long_be(fp.read(4))
	channels = get_long_be(fp.read(4))
	excess = hdr_size - 24
	if excess < 0:
		raise error, 'gethdr: bad hdr_size'
	if excess > 0:
		info = fp.read(excess)
	else:
		info = ''
	return (data_size, encoding, sample_rate, channels, info)


# read and print the sound header of a named file

def printhdr(file):
	hdr = gethdr(open(file, 'r'))
	data_size, encoding, sample_rate, channels, info = hdr
	while info[-1:] == '\0':
		info = info[:-1]
	print 'File name:  ', file
	print 'Data size:  ', data_size
	print 'Encoding:   ', encoding
	print 'Sample rate:', sample_rate
	print 'Channels:   ', channels
	print 'Info:       ', `info`
