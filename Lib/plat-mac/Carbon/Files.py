# Generated from 'Files.h'

def FOUR_CHAR_CODE(x): return x
true = True
false = False
fsCurPerm = 0x00
fsRdPerm = 0x01
fsWrPerm = 0x02
fsRdWrPerm = 0x03
fsRdWrShPerm = 0x04
fsRdDenyPerm = 0x10
fsWrDenyPerm = 0x20  
fsRtParID = 1
fsRtDirID = 2
fsAtMark = 0
fsFromStart = 1
fsFromLEOF = 2
fsFromMark = 3
pleaseCacheBit = 4
pleaseCacheMask = 0x0010
noCacheBit = 5
noCacheMask = 0x0020
rdVerifyBit = 6
rdVerifyMask = 0x0040
rdVerify = 64
forceReadBit = 6
forceReadMask = 0x0040
newLineBit = 7
newLineMask = 0x0080
newLineCharMask = 0xFF00 
fsSBPartialName = 1
fsSBFullName = 2
fsSBFlAttrib = 4
fsSBFlFndrInfo = 8
fsSBFlLgLen = 32
fsSBFlPyLen = 64
fsSBFlRLgLen = 128
fsSBFlRPyLen = 256
fsSBFlCrDat = 512
fsSBFlMdDat = 1024
fsSBFlBkDat = 2048
fsSBFlXFndrInfo = 4096
fsSBFlParID = 8192
fsSBNegate = 16384
fsSBDrUsrWds = 8
fsSBDrNmFls = 16
fsSBDrCrDat = 512
fsSBDrMdDat = 1024
fsSBDrBkDat = 2048
fsSBDrFndrInfo = 4096
fsSBDrParID = 8192
fsSBPartialNameBit = 0
fsSBFullNameBit = 1
fsSBFlAttribBit = 2
fsSBFlFndrInfoBit = 3
fsSBFlLgLenBit = 5
fsSBFlPyLenBit = 6
fsSBFlRLgLenBit = 7
fsSBFlRPyLenBit = 8
fsSBFlCrDatBit = 9
fsSBFlMdDatBit = 10
fsSBFlBkDatBit = 11
fsSBFlXFndrInfoBit = 12
fsSBFlParIDBit = 13
fsSBNegateBit = 14
fsSBDrUsrWdsBit = 3
fsSBDrNmFlsBit = 4
fsSBDrCrDatBit = 9
fsSBDrMdDatBit = 10
fsSBDrBkDatBit = 11
fsSBDrFndrInfoBit = 12
fsSBDrParIDBit = 13    
bLimitFCBs = 31
bLocalWList = 30
bNoMiniFndr = 29
bNoVNEdit = 28
bNoLclSync = 27
bTrshOffLine = 26
bNoSwitchTo = 25
bDontShareIt = 21
bNoDeskItems = 20
bNoBootBlks = 19
bAccessCntl = 18
bNoSysDir = 17
bHasExtFSVol = 16
bHasOpenDeny = 15
bHasCopyFile = 14
bHasMoveRename = 13
bHasDesktopMgr = 12
bHasShortName = 11
bHasFolderLock = 10
bHasPersonalAccessPrivileges = 9
bHasUserGroupList = 8
bHasCatSearch = 7
bHasFileIDs = 6
bHasBTreeMgr = 5
bHasBlankAccessPrivileges = 4
bSupportsAsyncRequests = 3
bSupportsTrashVolumeCache = 2
bIsEjectable = 0
bSupportsHFSPlusAPIs = 1
bSupportsFSCatalogSearch = 2
bSupportsFSExchangeObjects = 3
bSupports2TBFiles = 4
bSupportsLongNames = 5
bSupportsMultiScriptNames = 6
bSupportsNamedForks = 7
bSupportsSubtreeIterators = 8
bL2PCanMapFileBlocks = 9     
bParentModDateChanges = 10
bAncestorModDateChanges = 11    
bSupportsSymbolicLinks = 13
bIsAutoMounted = 14
bAllowCDiDataHandler = 17    
kLargeIcon = 1
kLarge4BitIcon = 2
kLarge8BitIcon = 3
kSmallIcon = 4
kSmall4BitIcon = 5
kSmall8BitIcon = 6
kicnsIconFamily = 239   
kLargeIconSize = 256
kLarge4BitIconSize = 512
kLarge8BitIconSize = 1024
kSmallIconSize = 64
kSmall4BitIconSize = 128
kSmall8BitIconSize = 256
kWidePosOffsetBit = 8
kUseWidePositioning = (1 << kWidePosOffsetBit)
kMaximumBlocksIn4GB = 0x007FFFFF
fsUnixPriv = 1
kNoUserAuthentication = 1
kPassword = 2
kEncryptPassword = 3
kTwoWayEncryptPassword = 6
kOwnerID2Name = 1
kGroupID2Name = 2
kOwnerName2ID = 3
kGroupName2ID = 4
kReturnNextUser = 1
kReturnNextGroup = 2
kReturnNextUG = 3
kVCBFlagsIdleFlushBit = 3
kVCBFlagsIdleFlushMask = 0x0008
kVCBFlagsHFSPlusAPIsBit = 4
kVCBFlagsHFSPlusAPIsMask = 0x0010
kVCBFlagsHardwareGoneBit = 5
kVCBFlagsHardwareGoneMask = 0x0020
kVCBFlagsVolumeDirtyBit = 15
kVCBFlagsVolumeDirtyMask = 0x8000
kioVAtrbDefaultVolumeBit = 5
kioVAtrbDefaultVolumeMask = 0x0020
kioVAtrbFilesOpenBit = 6
kioVAtrbFilesOpenMask = 0x0040
kioVAtrbHardwareLockedBit = 7
kioVAtrbHardwareLockedMask = 0x0080
kioVAtrbSoftwareLockedBit = 15
kioVAtrbSoftwareLockedMask = 0x8000
kioFlAttribLockedBit = 0
kioFlAttribLockedMask = 0x01
kioFlAttribResOpenBit = 2
kioFlAttribResOpenMask = 0x04
kioFlAttribDataOpenBit = 3
kioFlAttribDataOpenMask = 0x08
kioFlAttribDirBit = 4
kioFlAttribDirMask = 0x10
ioDirFlg = 4
ioDirMask = 0x10
kioFlAttribCopyProtBit = 6
kioFlAttribCopyProtMask = 0x40
kioFlAttribFileOpenBit = 7
kioFlAttribFileOpenMask = 0x80
kioFlAttribInSharedBit = 2
kioFlAttribInSharedMask = 0x04
kioFlAttribMountedBit = 3
kioFlAttribMountedMask = 0x08
kioFlAttribSharePointBit = 5
kioFlAttribSharePointMask = 0x20
kioFCBWriteBit = 8
kioFCBWriteMask = 0x0100
kioFCBResourceBit = 9
kioFCBResourceMask = 0x0200
kioFCBWriteLockedBit = 10
kioFCBWriteLockedMask = 0x0400
kioFCBLargeFileBit = 11
kioFCBLargeFileMask = 0x0800
kioFCBSharedWriteBit = 12
kioFCBSharedWriteMask = 0x1000
kioFCBFileLockedBit = 13
kioFCBFileLockedMask = 0x2000
kioFCBOwnClumpBit = 14
kioFCBOwnClumpMask = 0x4000
kioFCBModifiedBit = 15
kioFCBModifiedMask = 0x8000
kioACUserNoSeeFolderBit = 0
kioACUserNoSeeFolderMask = 0x01
kioACUserNoSeeFilesBit = 1
kioACUserNoSeeFilesMask = 0x02
kioACUserNoMakeChangesBit = 2
kioACUserNoMakeChangesMask = 0x04
kioACUserNotOwnerBit = 7
kioACUserNotOwnerMask = 0x80
kioACAccessOwnerBit = 31
# kioACAccessOwnerMask = (long)0x80000000
kioACAccessBlankAccessBit = 28
kioACAccessBlankAccessMask = 0x10000000
kioACAccessUserWriteBit = 26
kioACAccessUserWriteMask = 0x04000000
kioACAccessUserReadBit = 25
kioACAccessUserReadMask = 0x02000000
kioACAccessUserSearchBit = 24
kioACAccessUserSearchMask = 0x01000000
kioACAccessEveryoneWriteBit = 18
kioACAccessEveryoneWriteMask = 0x00040000
kioACAccessEveryoneReadBit = 17
kioACAccessEveryoneReadMask = 0x00020000
kioACAccessEveryoneSearchBit = 16
kioACAccessEveryoneSearchMask = 0x00010000
kioACAccessGroupWriteBit = 10
kioACAccessGroupWriteMask = 0x00000400
kioACAccessGroupReadBit = 9
kioACAccessGroupReadMask = 0x00000200
kioACAccessGroupSearchBit = 8
kioACAccessGroupSearchMask = 0x00000100
kioACAccessOwnerWriteBit = 2
kioACAccessOwnerWriteMask = 0x00000004
kioACAccessOwnerReadBit = 1
kioACAccessOwnerReadMask = 0x00000002
kioACAccessOwnerSearchBit = 0
kioACAccessOwnerSearchMask = 0x00000001
kfullPrivileges = 0x00070007
kownerPrivileges = 0x00000007 
knoUser = 0
kadministratorUser = 1
knoGroup = 0
AppleShareMediaType = FOUR_CHAR_CODE('afpm')
volMountNoLoginMsgFlagBit = 0
volMountNoLoginMsgFlagMask = 0x0001
volMountExtendedFlagsBit = 7
volMountExtendedFlagsMask = 0x0080
volMountInteractBit = 15
volMountInteractMask = 0x8000
volMountChangedBit = 14
volMountChangedMask = 0x4000
volMountFSReservedMask = 0x00FF
volMountSysReservedMask = 0xFF00 
kAFPExtendedFlagsAlternateAddressMask = 1 
kAFPTagTypeIP = 0x01
kAFPTagTypeIPPort = 0x02
kAFPTagTypeDDP = 0x03
kAFPTagTypeDNS = 0x04  
kAFPTagLengthIP = 0x06
kAFPTagLengthIPPort = 0x08
kAFPTagLengthDDP = 0x06
kFSInvalidVolumeRefNum = 0
kFSCatInfoNone = 0x00000000
kFSCatInfoTextEncoding = 0x00000001
kFSCatInfoNodeFlags = 0x00000002
kFSCatInfoVolume = 0x00000004
kFSCatInfoParentDirID = 0x00000008
kFSCatInfoNodeID = 0x00000010
kFSCatInfoCreateDate = 0x00000020
kFSCatInfoContentMod = 0x00000040
kFSCatInfoAttrMod = 0x00000080
kFSCatInfoAccessDate = 0x00000100
kFSCatInfoBackupDate = 0x00000200
kFSCatInfoPermissions = 0x00000400
kFSCatInfoFinderInfo = 0x00000800
kFSCatInfoFinderXInfo = 0x00001000
kFSCatInfoValence = 0x00002000
kFSCatInfoDataSizes = 0x00004000
kFSCatInfoRsrcSizes = 0x00008000
kFSCatInfoSharingFlags = 0x00010000
kFSCatInfoUserPrivs = 0x00020000
kFSCatInfoUserAccess = 0x00080000
kFSCatInfoAllDates = 0x000003E0
kFSCatInfoGettableInfo = 0x0003FFFF
kFSCatInfoSettableInfo = 0x00001FE3
# kFSCatInfoReserved = (long)0xFFFC0000 
kFSNodeLockedBit = 0
kFSNodeLockedMask = 0x0001
kFSNodeResOpenBit = 2
kFSNodeResOpenMask = 0x0004
kFSNodeDataOpenBit = 3
kFSNodeDataOpenMask = 0x0008
kFSNodeIsDirectoryBit = 4
kFSNodeIsDirectoryMask = 0x0010
kFSNodeCopyProtectBit = 6
kFSNodeCopyProtectMask = 0x0040
kFSNodeForkOpenBit = 7
kFSNodeForkOpenMask = 0x0080
kFSNodeInSharedBit = 2
kFSNodeInSharedMask = 0x0004
kFSNodeIsMountedBit = 3
kFSNodeIsMountedMask = 0x0008
kFSNodeIsSharePointBit = 5
kFSNodeIsSharePointMask = 0x0020
kFSIterateFlat = 0
kFSIterateSubtree = 1
kFSIterateDelete = 2
# kFSIterateReserved = (long)0xFFFFFFFC
fsSBNodeID = 0x00008000
fsSBAttributeModDate = 0x00010000
fsSBAccessDate = 0x00020000
fsSBPermissions = 0x00040000
fsSBNodeIDBit = 15
fsSBAttributeModDateBit = 16
fsSBAccessDateBit = 17
fsSBPermissionsBit = 18
kFSAllocDefaultFlags = 0x0000
kFSAllocAllOrNothingMask = 0x0001
kFSAllocContiguousMask = 0x0002
kFSAllocNoRoundUpMask = 0x0004
kFSAllocReservedMask = 0xFFF8 
kFSVolInfoNone = 0x0000
kFSVolInfoCreateDate = 0x0001
kFSVolInfoModDate = 0x0002
kFSVolInfoBackupDate = 0x0004
kFSVolInfoCheckedDate = 0x0008
kFSVolInfoFileCount = 0x0010
kFSVolInfoDirCount = 0x0020
kFSVolInfoSizes = 0x0040
kFSVolInfoBlocks = 0x0080
kFSVolInfoNextAlloc = 0x0100
kFSVolInfoRsrcClump = 0x0200
kFSVolInfoDataClump = 0x0400
kFSVolInfoNextID = 0x0800
kFSVolInfoFinderInfo = 0x1000
kFSVolInfoFlags = 0x2000
kFSVolInfoFSInfo = 0x4000
kFSVolInfoDriveInfo = 0x8000
kFSVolInfoGettableInfo = 0xFFFF
kFSVolInfoSettableInfo = 0x3004 
kFSVolFlagDefaultVolumeBit = 5
kFSVolFlagDefaultVolumeMask = 0x0020
kFSVolFlagFilesOpenBit = 6
kFSVolFlagFilesOpenMask = 0x0040
kFSVolFlagHardwareLockedBit = 7
kFSVolFlagHardwareLockedMask = 0x0080
kFSVolFlagSoftwareLockedBit = 15
kFSVolFlagSoftwareLockedMask = 0x8000
kFNDirectoryModifiedMessage = 1
kFNNoImplicitAllSubscription = (1 << 0)
rAliasType = FOUR_CHAR_CODE('alis') 
kARMMountVol = 0x00000001
kARMNoUI = 0x00000002
kARMMultVols = 0x00000008
kARMSearch = 0x00000100
kARMSearchMore = 0x00000200
kARMSearchRelFirst = 0x00000400 
asiZoneName = -3
asiServerName = -2
asiVolumeName = -1
asiAliasName = 0
asiParentName = 1     
kResolveAliasFileNoUI = 0x00000001 
kClippingCreator = FOUR_CHAR_CODE('drag')
kClippingPictureType = FOUR_CHAR_CODE('clpp')
kClippingTextType = FOUR_CHAR_CODE('clpt')
kClippingSoundType = FOUR_CHAR_CODE('clps')
kClippingUnknownType = FOUR_CHAR_CODE('clpu')
kInternetLocationCreator = FOUR_CHAR_CODE('drag')
kInternetLocationHTTP = FOUR_CHAR_CODE('ilht')
kInternetLocationFTP = FOUR_CHAR_CODE('ilft')
kInternetLocationFile = FOUR_CHAR_CODE('ilfi')
kInternetLocationMail = FOUR_CHAR_CODE('ilma')
kInternetLocationNNTP = FOUR_CHAR_CODE('ilnw')
kInternetLocationAFP = FOUR_CHAR_CODE('ilaf')
kInternetLocationAppleTalk = FOUR_CHAR_CODE('ilat')
kInternetLocationNSL = FOUR_CHAR_CODE('ilns')
kInternetLocationGeneric = FOUR_CHAR_CODE('ilge')
kCustomIconResource = -16455 
kCustomBadgeResourceType = FOUR_CHAR_CODE('badg')
kCustomBadgeResourceID = kCustomIconResource
kCustomBadgeResourceVersion = 0
# kSystemFolderType = 'macs'.
kRoutingResourceType = FOUR_CHAR_CODE('rout')
kRoutingResourceID = 0
kContainerFolderAliasType = FOUR_CHAR_CODE('fdrp')
kContainerTrashAliasType = FOUR_CHAR_CODE('trsh')
kContainerHardDiskAliasType = FOUR_CHAR_CODE('hdsk')
kContainerFloppyAliasType = FOUR_CHAR_CODE('flpy')
kContainerServerAliasType = FOUR_CHAR_CODE('srvr')
kApplicationAliasType = FOUR_CHAR_CODE('adrp')
kContainerAliasType = FOUR_CHAR_CODE('drop')
kDesktopPrinterAliasType = FOUR_CHAR_CODE('dtpa')
kContainerCDROMAliasType = FOUR_CHAR_CODE('cddr')
kApplicationCPAliasType = FOUR_CHAR_CODE('acdp')
kApplicationDAAliasType = FOUR_CHAR_CODE('addp')
kPackageAliasType = FOUR_CHAR_CODE('fpka')
kAppPackageAliasType = FOUR_CHAR_CODE('fapa') 
kSystemFolderAliasType = FOUR_CHAR_CODE('fasy')
kAppleMenuFolderAliasType = FOUR_CHAR_CODE('faam')
kStartupFolderAliasType = FOUR_CHAR_CODE('fast')
kPrintMonitorDocsFolderAliasType = FOUR_CHAR_CODE('fapn')
kPreferencesFolderAliasType = FOUR_CHAR_CODE('fapf')
kControlPanelFolderAliasType = FOUR_CHAR_CODE('fact')
kExtensionFolderAliasType = FOUR_CHAR_CODE('faex')
kExportedFolderAliasType = FOUR_CHAR_CODE('faet')
kDropFolderAliasType = FOUR_CHAR_CODE('fadr')
kSharedFolderAliasType = FOUR_CHAR_CODE('fash')
kMountedFolderAliasType = FOUR_CHAR_CODE('famn')
kIsOnDesk = 0x0001
kColor = 0x000E
kIsShared = 0x0040
kHasNoINITs = 0x0080
kHasBeenInited = 0x0100
kHasCustomIcon = 0x0400
kIsStationery = 0x0800
kNameLocked = 0x1000
kHasBundle = 0x2000
kIsInvisible = 0x4000
kIsAlias = 0x8000 
fOnDesk = kIsOnDesk
fHasBundle = kHasBundle
fInvisible = kIsInvisible
fTrash = -3
fDesktop = -2
fDisk = 0
kIsStationary = kIsStationery
kExtendedFlagsAreInvalid = 0x8000
kExtendedFlagHasCustomBadge = 0x0100
kExtendedFlagHasRoutingInfo = 0x0004 
kFirstMagicBusyFiletype = FOUR_CHAR_CODE('bzy ')
kLastMagicBusyFiletype = FOUR_CHAR_CODE('bzy?')
kMagicBusyCreationDate = 0x4F3AFDB0
