# Generated by h2py from /usr/include/sys/socket.h
SOCK_STREAM = 1
SOCK_DGRAM = 2
SOCK_RAW = 3
SOCK_RDM = 4
SOCK_SEQPACKET = 5
SO_DEBUG = 0x0001
SO_ACCEPTCONN = 0x0002
SO_REUSEADDR = 0x0004
SO_KEEPALIVE = 0x0008
SO_DONTROUTE = 0x0010
SO_BROADCAST = 0x0020
SO_USELOOPBACK = 0x0040
SO_LINGER = 0x0080
SO_OOBINLINE = 0x0100
SO_REUSEPORT = 0x0200
SO_TIMESTAMP = 0x0400
SO_SNDBUF = 0x1001
SO_RCVBUF = 0x1002
SO_SNDLOWAT = 0x1003
SO_RCVLOWAT = 0x1004
SO_SNDTIMEO = 0x1005
SO_RCVTIMEO = 0x1006
SO_ERROR = 0x1007
SO_TYPE = 0x1008
SOL_SOCKET = 0xffff
AF_UNSPEC = 0
AF_LOCAL = 1
AF_UNIX = AF_LOCAL
AF_INET = 2
AF_IMPLINK = 3
AF_PUP = 4
AF_CHAOS = 5
AF_NS = 6
AF_ISO = 7
AF_OSI = AF_ISO
AF_ECMA = 8
AF_DATAKIT = 9
AF_CCITT = 10
AF_SNA = 11
AF_DECnet = 12
AF_DLI = 13
AF_LAT = 14
AF_HYLINK = 15
AF_APPLETALK = 16
AF_ROUTE = 17
AF_LINK = 18
pseudo_AF_XTP = 19
AF_COIP = 20
AF_CNT = 21
pseudo_AF_RTIP = 22
AF_IPX = 23
AF_SIP = 24
pseudo_AF_PIP = 25
AF_ISDN = 26
AF_E164 = AF_ISDN
pseudo_AF_KEY = 27
AF_INET6 = 28
AF_NATM = 29
AF_MAX = 30
SOCK_MAXADDRLEN = 255
PF_UNSPEC = AF_UNSPEC
PF_LOCAL = AF_LOCAL
PF_UNIX = PF_LOCAL
PF_INET = AF_INET
PF_IMPLINK = AF_IMPLINK
PF_PUP = AF_PUP
PF_CHAOS = AF_CHAOS
PF_NS = AF_NS
PF_ISO = AF_ISO
PF_OSI = AF_ISO
PF_ECMA = AF_ECMA
PF_DATAKIT = AF_DATAKIT
PF_CCITT = AF_CCITT
PF_SNA = AF_SNA
PF_DECnet = AF_DECnet
PF_DLI = AF_DLI
PF_LAT = AF_LAT
PF_HYLINK = AF_HYLINK
PF_APPLETALK = AF_APPLETALK
PF_ROUTE = AF_ROUTE
PF_LINK = AF_LINK
PF_XTP = pseudo_AF_XTP
PF_COIP = AF_COIP
PF_CNT = AF_CNT
PF_SIP = AF_SIP
PF_IPX = AF_IPX
PF_RTIP = pseudo_AF_RTIP
PF_PIP = pseudo_AF_PIP
PF_ISDN = AF_ISDN
PF_KEY = pseudo_AF_KEY
PF_INET6 = AF_INET6
PF_NATM = AF_NATM
PF_MAX = AF_MAX
NET_MAXID = AF_MAX
NET_RT_DUMP = 1
NET_RT_FLAGS = 2
NET_RT_IFLIST = 3
NET_RT_MAXID = 4
SOMAXCONN = 128
MSG_OOB = 0x1
MSG_PEEK = 0x2
MSG_DONTROUTE = 0x4
MSG_EOR = 0x8
MSG_TRUNC = 0x10
MSG_CTRUNC = 0x20
MSG_WAITALL = 0x40
MSG_DONTWAIT = 0x80
MSG_EOF = 0x100
MSG_COMPAT = 0x8000
CMGROUP_MAX = 16
SCM_RIGHTS = 0x01
SCM_TIMESTAMP = 0x02
SCM_CREDS = 0x03

# Included from sys/cdefs.h
def __P(protos): return protos		 

def __STRING(x): return #x		 

def __XSTRING(x): return __STRING(x)	 

def __P(protos): return ()		 

def __STRING(x): return "x"

def __RCSID(s): return __IDSTRING(rcsid,s)

def __RCSID_SOURCE(s): return __IDSTRING(rcsid_source,s)

def __COPYRIGHT(s): return __IDSTRING(copyright,s)

