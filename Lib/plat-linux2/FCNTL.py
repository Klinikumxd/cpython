# Generated by h2py from /usr/include/sys/fcntl.h

# Included from fcntl.h

# Included from features.h
_FEATURES_H = 1
_GNU_SOURCE = 1
__USE_ANSI = 1
__FAVOR_BSD = 1
_BSD_SOURCE = 1
_SVID_SOURCE = 1
_POSIX_SOURCE = 1
_POSIX_C_SOURCE = 2
__USE_POSIX = 1
__USE_POSIX2 = 1
__USE_MISC = 1
__USE_BSD = 1
__USE_SVID = 1
__USE_GNU = 1
__GNU_LIBRARY__ = 1

# Included from sys/cdefs.h
_SYS_CDEFS_H = 1
def __P(args): return args	 

def __P(args): return args

def __P(args): return ()	 

def __STRING(x): return #x

def __STRING(x): return "x"


# Included from sys/types.h

# Included from linux/types.h
__FD_SETSIZE = 256

# Included from asm/types.h
def __FD_ZERO(fdsetp): return \


# Included from sys/bitypes.h

# Included from gnu/types.h
_GNU_TYPES_H = 1
__FDSET_LONGS = 8
def __FD_ZERO(fdsetp): return \

__FD_SETSIZE = 256
def __FDELT(d): return ((d) / __NFDBITS)

def __FDMASK(d): return (1 << ((d) % __NFDBITS))

def __FD_ZERO(set): return \


# Included from linux/fcntl.h

# Included from asm/fcntl.h
O_ACCMODE = 0003
O_RDONLY = 00
O_WRONLY = 01
O_RDWR = 02
O_CREAT = 0100
O_EXCL = 0200
O_NOCTTY = 0400
O_TRUNC = 01000
O_APPEND = 02000
O_NONBLOCK = 04000
O_NDELAY = O_NONBLOCK
O_SYNC = 010000
FASYNC = 020000
F_DUPFD = 0
F_GETFD = 1
F_SETFD = 2
F_GETFL = 3
F_SETFL = 4
F_GETLK = 5
F_SETLK = 6
F_SETLKW = 7
F_SETOWN = 8
F_GETOWN = 9
FD_CLOEXEC = 1
F_RDLCK = 0
F_WRLCK = 1
F_UNLCK = 2
F_EXLCK = 4
F_SHLCK = 8
LOCK_SH = 1
LOCK_EX = 2
LOCK_NB = 4
LOCK_UN = 8
F_POSIX = 1
F_FLOCK = 2
FNDELAY = O_NDELAY
F_ULOCK = 0
F_LOCK = 1
F_TLOCK = 2
F_TEST = 3
