# Generated from 'Macintosh HD:SWDev:Codewarrior Pro 5:Metrowerks CodeWarrior:MacOS Support:Universal:Interfaces:CIncludes:Components.h'

def FOUR_CHAR_CODE(x): return x
kAppleManufacturer = FOUR_CHAR_CODE('appl')
kComponentResourceType = FOUR_CHAR_CODE('thng')
kComponentAliasResourceType = FOUR_CHAR_CODE('thga')
kAnyComponentType = 0
kAnyComponentSubType = 0
kAnyComponentManufacturer = 0
kAnyComponentFlagsMask = 0
cmpWantsRegisterMessage = 1L << 31
kComponentOpenSelect = -1
kComponentCloseSelect = -2
kComponentCanDoSelect = -3
kComponentVersionSelect = -4
kComponentRegisterSelect = -5
kComponentTargetSelect = -6
kComponentUnregisterSelect = -7
kComponentGetMPWorkFunctionSelect = -8
componentDoAutoVersion = (1 << 0)
componentWantsUnregister = (1 << 1)
componentAutoVersionIncludeFlags = (1 << 2)
componentHasMultiplePlatforms = (1 << 3)
componentLoadResident = (1 << 4)
defaultComponentIdentical = 0
defaultComponentAnyFlags = 1
defaultComponentAnyManufacturer = 2
defaultComponentAnySubType = 4
defaultComponentAnyFlagsAnyManufacturer = (defaultComponentAnyFlags + defaultComponentAnyManufacturer)
defaultComponentAnyFlagsAnyManufacturerAnySubType = (defaultComponentAnyFlags + defaultComponentAnyManufacturer + defaultComponentAnySubType)
registerComponentGlobal = 1
registerComponentNoDuplicates = 2
registerComponentAfterExisting = 4
registerComponentAliasesOnly = 8
platform68k = 1
platformPowerPC = 2
platformInterpreted = 3
platformWin32 = 4
mpWorkFlagDoWork = (1 << 0)
mpWorkFlagDoCompletion = (1 << 1)
mpWorkFlagCopyWorkBlock = (1 << 2)
mpWorkFlagDontBlock = (1 << 3)
mpWorkFlagGetProcessorCount = (1 << 4)
mpWorkFlagGetIsRunning = (1 << 6)
uppComponentFunctionImplementedProcInfo = 0x000002F0
uppGetComponentVersionProcInfo = 0x000000F0
uppComponentSetTargetProcInfo = 0x000003F0
uppCallComponentOpenProcInfo = 0x000003F0
uppCallComponentCloseProcInfo = 0x000003F0
uppCallComponentCanDoProcInfo = 0x000002F0
uppCallComponentVersionProcInfo = 0x000000F0
uppCallComponentRegisterProcInfo = 0x000000F0
uppCallComponentTargetProcInfo = 0x000003F0
uppCallComponentUnregisterProcInfo = 0x000000F0
uppCallComponentGetMPWorkFunctionProcInfo = 0x00000FF0
