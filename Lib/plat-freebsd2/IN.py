# Generated by h2py from /usr/include/netinet/in.h
IPPROTO_IP = 0
IPPROTO_ICMP = 1
IPPROTO_IGMP = 2
IPPROTO_GGP = 3
IPPROTO_IPIP = 4
IPPROTO_TCP = 6
IPPROTO_ST = 7
IPPROTO_EGP = 8
IPPROTO_PIGP = 9
IPPROTO_RCCMON = 10
IPPROTO_NVPII = 11
IPPROTO_PUP = 12
IPPROTO_ARGUS = 13
IPPROTO_EMCON = 14
IPPROTO_XNET = 15
IPPROTO_CHAOS = 16
IPPROTO_UDP = 17
IPPROTO_MUX = 18
IPPROTO_MEAS = 19
IPPROTO_HMP = 20
IPPROTO_PRM = 21
IPPROTO_IDP = 22
IPPROTO_TRUNK1 = 23
IPPROTO_TRUNK2 = 24
IPPROTO_LEAF1 = 25
IPPROTO_LEAF2 = 26
IPPROTO_RDP = 27
IPPROTO_IRTP = 28
IPPROTO_TP = 29
IPPROTO_BLT = 30
IPPROTO_NSP = 31
IPPROTO_INP = 32
IPPROTO_SEP = 33
IPPROTO_3PC = 34
IPPROTO_IDPR = 35
IPPROTO_XTP = 36
IPPROTO_DDP = 37
IPPROTO_CMTP = 38
IPPROTO_TPXX = 39
IPPROTO_IL = 40
IPPROTO_SIP = 41
IPPROTO_SDRP = 42
IPPROTO_SIPSR = 43
IPPROTO_SIPFRAG = 44
IPPROTO_IDRP = 45
IPPROTO_RSVP = 46
IPPROTO_GRE = 47
IPPROTO_MHRP = 48
IPPROTO_BHA = 49
IPPROTO_ESP = 50
IPPROTO_AH = 51
IPPROTO_INLSP = 52
IPPROTO_SWIPE = 53
IPPROTO_NHRP = 54
IPPROTO_AHIP = 61
IPPROTO_CFTP = 62
IPPROTO_HELLO = 63
IPPROTO_SATEXPAK = 64
IPPROTO_KRYPTOLAN = 65
IPPROTO_RVD = 66
IPPROTO_IPPC = 67
IPPROTO_ADFS = 68
IPPROTO_SATMON = 69
IPPROTO_VISA = 70
IPPROTO_IPCV = 71
IPPROTO_CPNX = 72
IPPROTO_CPHB = 73
IPPROTO_WSN = 74
IPPROTO_PVP = 75
IPPROTO_BRSATMON = 76
IPPROTO_ND = 77
IPPROTO_WBMON = 78
IPPROTO_WBEXPAK = 79
IPPROTO_EON = 80
IPPROTO_VMTP = 81
IPPROTO_SVMTP = 82
IPPROTO_VINES = 83
IPPROTO_TTP = 84
IPPROTO_IGP = 85
IPPROTO_DGP = 86
IPPROTO_TCF = 87
IPPROTO_IGRP = 88
IPPROTO_OSPFIGP = 89
IPPROTO_SRPC = 90
IPPROTO_LARP = 91
IPPROTO_MTP = 92
IPPROTO_AX25 = 93
IPPROTO_IPEIP = 94
IPPROTO_MICP = 95
IPPROTO_SCCSP = 96
IPPROTO_ETHERIP = 97
IPPROTO_ENCAP = 98
IPPROTO_APES = 99
IPPROTO_GMTP = 100
IPPROTO_DIVERT = 254
IPPROTO_RAW = 255
IPPROTO_MAX = 256
IPPORT_RESERVED = 1024
IPPORT_USERRESERVED = 5000
IPPORT_HIFIRSTAUTO = 40000
IPPORT_HILASTAUTO = 44999
IPPORT_RESERVEDSTART = 600
def IN_CLASSA(i): return (((long)(i) & 0x80000000) == 0)

IN_CLASSA_NET = 0xff000000
IN_CLASSA_NSHIFT = 24
IN_CLASSA_HOST = 0x00ffffff
IN_CLASSA_MAX = 128
def IN_CLASSB(i): return (((long)(i) & 0xc0000000) == 0x80000000)

IN_CLASSB_NET = 0xffff0000
IN_CLASSB_NSHIFT = 16
IN_CLASSB_HOST = 0x0000ffff
IN_CLASSB_MAX = 65536
def IN_CLASSC(i): return (((long)(i) & 0xe0000000) == 0xc0000000)

IN_CLASSC_NET = 0xffffff00
IN_CLASSC_NSHIFT = 8
IN_CLASSC_HOST = 0x000000ff
def IN_CLASSD(i): return (((long)(i) & 0xf0000000) == 0xe0000000)

IN_CLASSD_NET = 0xf0000000
IN_CLASSD_NSHIFT = 28
IN_CLASSD_HOST = 0x0fffffff
def IN_MULTICAST(i): return IN_CLASSD(i)

def IN_EXPERIMENTAL(i): return (((long)(i) & 0xf0000000) == 0xf0000000)

def IN_BADCLASS(i): return (((long)(i) & 0xf0000000) == 0xf0000000)

INADDR_ANY = 0x00000000
INADDR_BROADCAST = 0xffffffff
INADDR_NONE = 0xffffffff
INADDR_UNSPEC_GROUP = 0xe0000000
INADDR_ALLHOSTS_GROUP = 0xe0000001
INADDR_ALLRTRS_GROUP = 0xe0000002
INADDR_MAX_LOCAL_GROUP = 0xe00000ff
IN_LOOPBACKNET = 127
IP_OPTIONS = 1
IP_HDRINCL = 2
IP_TOS = 3
IP_TTL = 4
IP_RECVOPTS = 5
IP_RECVRETOPTS = 6
IP_RECVDSTADDR = 7
IP_RETOPTS = 8
IP_MULTICAST_IF = 9
IP_MULTICAST_TTL = 10
IP_MULTICAST_LOOP = 11
IP_ADD_MEMBERSHIP = 12
IP_DROP_MEMBERSHIP = 13
IP_MULTICAST_VIF = 14
IP_RSVP_ON = 15
IP_RSVP_OFF = 16
IP_RSVP_VIF_ON = 17
IP_RSVP_VIF_OFF = 18
IP_PORTRANGE = 19
IP_RECVIF = 20
IP_FW_ADD = 50
IP_FW_DEL = 51
IP_FW_FLUSH = 52
IP_FW_ZERO = 53
IP_FW_GET = 54
IP_NAT = 55
IP_DEFAULT_MULTICAST_TTL = 1
IP_DEFAULT_MULTICAST_LOOP = 1
IP_MAX_MEMBERSHIPS = 20
IP_PORTRANGE_DEFAULT = 0
IP_PORTRANGE_HIGH = 1
IP_PORTRANGE_LOW = 2
IPPROTO_MAXID = (IPPROTO_IDP + 1)
IPCTL_FORWARDING = 1
IPCTL_SENDREDIRECTS = 2
IPCTL_DEFTTL = 3
IPCTL_DEFMTU = 4
IPCTL_RTEXPIRE = 5
IPCTL_RTMINEXPIRE = 6
IPCTL_RTMAXCACHE = 7
IPCTL_SOURCEROUTE = 8
IPCTL_DIRECTEDBROADCAST = 9
IPCTL_INTRQMAXLEN = 10
IPCTL_INTRQDROPS = 11
IPCTL_ACCEPTSOURCEROUTE = 13
IPCTL_MAXID = 13
IP_NAT_IN = 0x00000001
IP_NAT_OUT = 0x00000002
