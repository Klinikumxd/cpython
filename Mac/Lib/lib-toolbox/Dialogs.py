# Generated from 'flap:CodeWarrior Pro 5:Metrowerks CodeWarrior:MacOS Support:Universal:Interfaces:CIncludes:Dialogs.h'

def FOUR_CHAR_CODE(x): return x
gestaltDialogMgrAttr = FOUR_CHAR_CODE('dlog')
gestaltDialogMgrPresent = (1L << 0)
dialogNoTimeoutErr = -5640
kControlDialogItem = 4
kButtonDialogItem = kControlDialogItem | 0
kCheckBoxDialogItem = kControlDialogItem | 1
kRadioButtonDialogItem = kControlDialogItem | 2
kResourceControlDialogItem = kControlDialogItem | 3
kStaticTextDialogItem = 8
kEditTextDialogItem = 16
kIconDialogItem = 32
kPictureDialogItem = 64
kUserDialogItem = 0
kItemDisableBit = 128
ctrlItem = 4
btnCtrl = 0
chkCtrl = 1
radCtrl = 2
resCtrl = 3
statText = 8
editText = 16
iconItem = 32
picItem = 64
userItem = 0
itemDisable = 128
kStdOkItemIndex = 1
kStdCancelItemIndex = 2
ok = kStdOkItemIndex
cancel = kStdCancelItemIndex
kStopIcon = 0
kNoteIcon = 1
kCautionIcon = 2
stopIcon = kStopIcon
noteIcon = kNoteIcon
cautionIcon = kCautionIcon
kOkItemIndex = 1
kCancelItemIndex = 2
overlayDITL = 0
appendDITLRight = 1
appendDITLBottom = 2
kAlertStopAlert = 0
kAlertNoteAlert = 1
kAlertCautionAlert = 2
kAlertPlainAlert = 3
kAlertDefaultOKText = -1
kAlertDefaultCancelText = -1
kAlertDefaultOtherText = -1
kAlertStdAlertOKButton = 1
kAlertStdAlertCancelButton = 2
kAlertStdAlertOtherButton = 3
kAlertStdAlertHelpButton = 4
kDialogFlagsUseThemeBackground = (1 << 0)
kDialogFlagsUseControlHierarchy = (1 << 1)
kDialogFlagsHandleMovableModal = (1 << 2)
kDialogFlagsUseThemeControls = (1 << 3)
kAlertFlagsUseThemeBackground = (1 << 0)
kAlertFlagsUseControlHierarchy = (1 << 1)
kAlertFlagsAlertIsMovable = (1 << 2)
kAlertFlagsUseThemeControls = (1 << 3)
kDialogFontNoFontStyle = 0
kDialogFontUseFontMask = 0x0001
kDialogFontUseFaceMask = 0x0002
kDialogFontUseSizeMask = 0x0004
kDialogFontUseForeColorMask = 0x0008
kDialogFontUseBackColorMask = 0x0010
kDialogFontUseModeMask = 0x0020
kDialogFontUseJustMask = 0x0040
kDialogFontUseAllMask = 0x00FF
kDialogFontAddFontSizeMask = 0x0100
kDialogFontUseFontNameMask = 0x0200
kDialogFontAddToMetaFontMask = 0x0400
