# Generated from 'flap:CodeWarrior Pro 5:Metrowerks CodeWarrior:MacOS Support:Universal:Interfaces:CIncludes:AppleEvents.h'

def FOUR_CHAR_CODE(x): return x
from AEDataModel import *
keyDirectObject = FOUR_CHAR_CODE('----')
keyErrorNumber = FOUR_CHAR_CODE('errn')
keyErrorString = FOUR_CHAR_CODE('errs')
keyProcessSerialNumber = FOUR_CHAR_CODE('psn ')
keyPreDispatch = FOUR_CHAR_CODE('phac')
keySelectProc = FOUR_CHAR_CODE('selh')
keyAERecorderCount = FOUR_CHAR_CODE('recr')
keyAEVersion = FOUR_CHAR_CODE('vers')
kCoreEventClass = FOUR_CHAR_CODE('aevt')
kAEOpenApplication = FOUR_CHAR_CODE('oapp')
kAEOpenDocuments = FOUR_CHAR_CODE('odoc')
kAEPrintDocuments = FOUR_CHAR_CODE('pdoc')
kAEQuitApplication = FOUR_CHAR_CODE('quit')
kAEAnswer = FOUR_CHAR_CODE('ansr')
kAEApplicationDied = FOUR_CHAR_CODE('obit')
kAEStartRecording = FOUR_CHAR_CODE('reca')
kAEStopRecording = FOUR_CHAR_CODE('recc')
kAENotifyStartRecording = FOUR_CHAR_CODE('rec1')
kAENotifyStopRecording = FOUR_CHAR_CODE('rec0')
kAENotifyRecording = FOUR_CHAR_CODE('recr')
kAENeverInteract = 0x00000010
kAECanInteract = 0x00000020
kAEAlwaysInteract = 0x00000030
kAECanSwitchLayer = 0x00000040
kAEDontRecord = 0x00001000
kAEDontExecute = 0x00002000
kAEProcessNonReplyEvents = 0x00008000
kAENoReply = 0x00000001
kAEQueueReply = 0x00000002
kAEWaitReply = 0x00000003
kAEDontReconnect = 0x00000080
kAEWantReceipt = 0x00000200
kAEDefaultTimeout = -1
kNoTimeOut = -2
kAENormalPriority = 0x00000000
kAEHighPriority = 0x00000001
kAEUnknownSource = 0
kAEDirectCall = 1
kAESameProcess = 2
kAELocalProcess = 3
kAERemoteProcess = 4
kAEInteractWithSelf = 0
kAEInteractWithLocal = 1
kAEInteractWithAll = 2
kAEDoNotIgnoreHandler = 0x00000000
kAEIgnoreAppPhacHandler = 0x00000001
kAEIgnoreAppEventHandler = 0x00000002
kAEIgnoreSysPhacHandler = 0x00000004
kAEIgnoreSysEventHandler = 0x00000008
kAEIngoreBuiltInEventHandler = 0x00000010
# kAEDontDisposeOnResume = (long)0x80000000
kAENoDispatch = 0
# kAEUseStandardDispatch = (long)0xFFFFFFFF
