# Generated from 'flap:Metrowerks:Metrowerks CodeWarrior:MacOS Support:Headers:Universal Headers:Controls.h'

def FOUR_CHAR_CODE(x): return x
from TextEdit import *
from QuickDraw import *

_ControlDispatch = 0xAA73
kControlDefProcType = FOUR_CHAR_CODE('CDEF')
kControlTemplateResourceType = FOUR_CHAR_CODE('CNTL')
kControlColorTableResourceType = FOUR_CHAR_CODE('cctb')
kControlDefProcResourceType = FOUR_CHAR_CODE('CDEF')
kControlTabListResType = FOUR_CHAR_CODE('tab#')
kControlListDescResType = FOUR_CHAR_CODE('ldes')
cFrameColor = 0
cBodyColor = 1
cTextColor = 2
cThumbColor = 3
kNumberCtlCTabEntries = 4
errMessageNotSupported = -30580
errDataNotSupported = -30581
errControlDoesntSupportFocus = -30582
errWindowDoesntSupportFocus = -30583
errUnknownControl = -30584
errCouldntSetFocus = -30585
errNoRootControl = -30586
errRootAlreadyExists = -30587
errInvalidPartCode = -30588
errControlsAlreadyExist = -30589
errControlIsNotEmbedder = -30590
errDataSizeMismatch = -30591
errControlHiddenOrDisabled = -30592
errWindowRegionCodeInvalid = -30593
errCantEmbedIntoSelf = -30594
errCantEmbedRoot = -30595
errItemNotControl = -30596
pushButProc = 0
checkBoxProc = 1
radioButProc = 2
scrollBarProc = 16
popupMenuProc = 1008
kControlSupportsNewMessages = FOUR_CHAR_CODE(' ok ')
kControlFocusNoPart = 0
kControlFocusNextPart = -1
kControlFocusPrevPart = -2
kControlKeyFilterBlockKey = 0
kControlKeyFilterPassKey = 1
kControlFontBigSystemFont = -1
kControlFontSmallSystemFont = -2
kControlFontSmallBoldSystemFont = -3
kControlUseFontMask = 0x0001
kControlUseFaceMask = 0x0002
kControlUseSizeMask = 0x0004
kControlUseForeColorMask = 0x0008
kControlUseBackColorMask = 0x0010
kControlUseModeMask = 0x0020
kControlUseJustMask = 0x0040
kControlUseAllMask = 0x00FF
kControlAddFontSizeMask = 0x0100
kControlFontStyleTag = FOUR_CHAR_CODE('font')
kControlKeyFilterTag = FOUR_CHAR_CODE('fltr')
kControlSupportsGhosting = 1 << 0
kControlSupportsEmbedding = 1 << 1
kControlSupportsFocus = 1 << 2
kControlWantsIdle = 1 << 3
kControlWantsActivate = 1 << 4
kControlHandlesTracking = 1 << 5
kControlSupportsDataAccess = 1 << 6
kControlHasSpecialBackground = 1 << 7
kControlGetsFocusOnClick = 1 << 8
kControlSupportsCalcBestRect = 1 << 9
kControlSupportsLiveFeedback = 1 << 10
kControlHasRadioBehavior = 1 << 11
kControlMsgDrawGhost = 13
kControlMsgCalcBestRect = 14
kControlMsgHandleTracking = 15
kControlMsgFocus = 16
kControlMsgKeyDown = 17
kControlMsgIdle = 18
kControlMsgGetFeatures = 19
kControlMsgSetData = 20
kControlMsgGetData = 21
kControlMsgActivate = 22
kControlMsgSetUpBackground = 23
kControlMsgCalcValue = 24
kControlMsgSubControlHit = 25
kControlMsgCalcValueFromPos = 26
kControlMsgTestNewMsgSupport = 27
kControlMsgSubControlAdded = 28
kControlMsgSubControlRemoved = 29
kControlBevelButtonSmallBevelProc = 32
kControlBevelButtonNormalBevelProc = 33
kControlBevelButtonLargeBevelProc = 34
kControlBevelButtonAlignSysDirection = -1
kControlBevelButtonAlignCenter = 0
kControlBevelButtonAlignLeft = 1
kControlBevelButtonAlignRight = 2
kControlBevelButtonAlignTop = 3
kControlBevelButtonAlignBottom = 4
kControlBevelButtonAlignTopLeft = 5
kControlBevelButtonAlignBottomLeft = 6
kControlBevelButtonAlignTopRight = 7
kControlBevelButtonAlignBottomRight = 8
kControlBevelButtonAlignTextSysDirection = teFlushDefault
kControlBevelButtonAlignTextCenter = teCenter
kControlBevelButtonAlignTextFlushRight = teFlushRight
kControlBevelButtonAlignTextFlushLeft = teFlushLeft
kControlBevelButtonPlaceSysDirection = -1
kControlBevelButtonPlaceNormally = 0
kControlBevelButtonPlaceToRightOfGraphic = 1
kControlBevelButtonPlaceToLeftOfGraphic = 2
kControlBevelButtonPlaceBelowGraphic = 3
kControlBevelButtonPlaceAboveGraphic = 4
kControlBevelButtonSmallBevelVariant = 0
kControlBehaviorPushbutton = 0
kControlBehaviorToggles = 0x0100
kControlBehaviorSticky = 0x0200
kControlBehaviorMultiValueMenu = 0x4000
kControlBehaviorOffsetContents = 0x8000
kControlBehaviorCommandMenu = 0x2000
kControlContentTextOnly = 0
kControlContentIconSuiteRes = 1
kControlContentCIconRes = 2
kControlContentPictRes = 3
kControlContentIconSuiteHandle = 129
kControlContentCIconHandle = 130
kControlContentPictHandle = 131
kControlContentIconRef = 132
kControlBevelButtonContentTag = FOUR_CHAR_CODE('cont')
kControlBevelButtonTransformTag = FOUR_CHAR_CODE('tran')
kControlBevelButtonTextAlignTag = FOUR_CHAR_CODE('tali')
kControlBevelButtonTextOffsetTag = FOUR_CHAR_CODE('toff')
kControlBevelButtonGraphicAlignTag = FOUR_CHAR_CODE('gali')
kControlBevelButtonGraphicOffsetTag = FOUR_CHAR_CODE('goff')
kControlBevelButtonTextPlaceTag = FOUR_CHAR_CODE('tplc')
kControlBevelButtonMenuValueTag = FOUR_CHAR_CODE('mval')
kControlBevelButtonMenuHandleTag = FOUR_CHAR_CODE('mhnd')
kControlBevelButtonLastMenuTag = FOUR_CHAR_CODE('lmnu')
kControlBevelButtonMenuDelayTag = FOUR_CHAR_CODE('mdly')
kControlSliderProc = 48
kControlTriangleProc = 64
kControlTriangleLeftFacingProc = 65
kControlTriangleAutoToggleProc = 66
kControlTriangleLeftFacingAutoToggleProc = 67
kControlTriangleLastValueTag = FOUR_CHAR_CODE('last')
kControlProgressBarProc = 80
kControlLittleArrowsProc = 96
kControlChasingArrowsProc = 112
kControlTabLargeProc = 128
kControlTabSmallProc = 129
kControlTabContentRectTag = FOUR_CHAR_CODE('rect')
kControlTabEnabledFlagTag = FOUR_CHAR_CODE('enab')
kControlTabFontStyleTag = kControlFontStyleTag
kControlSeparatorLineProc = 144
kControlGroupBoxTextTitleProc = 160
kControlGroupBoxCheckBoxProc = 161
kControlGroupBoxPopupButtonProc = 162
kControlGroupBoxSecondaryTextTitleProc = 164
kControlGroupBoxSecondaryCheckBoxProc = 165
kControlGroupBoxSecondaryPopupButtonProc = 166
kControlGroupBoxMenuHandleTag = FOUR_CHAR_CODE('mhan')
kControlGroupBoxFontStyleTag = kControlFontStyleTag
kControlImageWellProc = 176
kControlImageWellContentTag = FOUR_CHAR_CODE('cont')
kControlImageWellTransformTag = FOUR_CHAR_CODE('tran')
kControlPopupArrowEastProc = 192
kControlPopupArrowWestProc = 193
kControlPopupArrowNorthProc = 194
kControlPopupArrowSouthProc = 195
kControlPopupArrowSmallEastProc = 196
kControlPopupArrowSmallWestProc = 197
kControlPopupArrowSmallNorthProc = 198
kControlPopupArrowSmallSouthProc = 199
kControlPlacardProc = 224
kControlClockTimeProc = 240
kControlClockTimeSecondsProc = 241
kControlClockDateProc = 242
kControlClockMonthYearProc = 243
kControlClockNoFlags = 0
kControlClockIsDisplayOnly = 1
kControlClockIsLive = 2
kControlClockLongDateTag = FOUR_CHAR_CODE('date')
kControlClockFontStyleTag = kControlFontStyleTag
kControlUserPaneProc = 256
kControlUserItemDrawProcTag = FOUR_CHAR_CODE('uidp')
kControlUserPaneDrawProcTag = FOUR_CHAR_CODE('draw')
kControlUserPaneHitTestProcTag = FOUR_CHAR_CODE('hitt')
kControlUserPaneTrackingProcTag = FOUR_CHAR_CODE('trak')
kControlUserPaneIdleProcTag = FOUR_CHAR_CODE('idle')
kControlUserPaneKeyDownProcTag = FOUR_CHAR_CODE('keyd')
kControlUserPaneActivateProcTag = FOUR_CHAR_CODE('acti')
kControlUserPaneFocusProcTag = FOUR_CHAR_CODE('foci')
kControlUserPaneBackgroundProcTag = FOUR_CHAR_CODE('back')
kControlEditTextProc = 272
kControlEditTextDialogProc = 273
kControlEditTextPasswordProc = 274
kControlEditTextStyleTag = kControlFontStyleTag
kControlEditTextTextTag = FOUR_CHAR_CODE('text')
kControlEditTextTEHandleTag = FOUR_CHAR_CODE('than')
kControlEditTextKeyFilterTag = kControlKeyFilterTag
kControlEditTextSelectionTag = FOUR_CHAR_CODE('sele')
kControlEditTextPasswordTag = FOUR_CHAR_CODE('pass')
kControlStaticTextProc = 288
kControlStaticTextStyleTag = kControlFontStyleTag
kControlStaticTextTextTag = FOUR_CHAR_CODE('text')
kControlStaticTextTextHeightTag = FOUR_CHAR_CODE('thei')
kControlPictureProc = 304
kControlPictureNoTrackProc = 305
kControlIconProc = 320
kControlIconNoTrackProc = 321
kControlIconSuiteProc = 322
kControlIconSuiteNoTrackProc = 323
kControlIconTransformTag = FOUR_CHAR_CODE('trfm')
kControlIconAlignmentTag = FOUR_CHAR_CODE('algn')
kControlWindowHeaderProc = 336
kControlWindowListViewHeaderProc = 337
kControlListBoxProc = 352
kControlListBoxAutoSizeProc = 353
kControlListBoxListHandleTag = FOUR_CHAR_CODE('lhan')
kControlListBoxKeyFilterTag = kControlKeyFilterTag
kControlListBoxFontStyleTag = kControlFontStyleTag
kControlListBoxDoubleClickTag = FOUR_CHAR_CODE('dblc')
kControlPushButtonProc = 368
kControlCheckBoxProc = 369
kControlRadioButtonProc = 370
kControlPushButLeftIconProc = 374
kControlPushButRightIconProc = 375
kControlPushButtonDefaultTag = FOUR_CHAR_CODE('dflt')
kControlScrollBarProc = 384
kControlScrollBarLiveProc = 386
kControlPopupButtonProc = 400
kControlPopupFixedWidthVariant = 1 << 0
kControlPopupVariableWidthVariant = 1 << 1
kControlPopupUseAddResMenuVariant = 1 << 2
kControlPopupUseWFontVariant = 1 << 3
kControlPopupButtonMenuHandleTag = FOUR_CHAR_CODE('mhan')
kControlPopupButtonMenuIDTag = FOUR_CHAR_CODE('mnid')
kControlRadioGroupProc = 416
kControlNoVariant = 0
kControlUsesOwningWindowsFontVariant = 1 << 3
kControlNoPart = 0
kControlLabelPart = 1
kControlMenuPart = 2
kControlTrianglePart = 4
kControlEditTextPart = 5
kControlPicturePart = 6
kControlIconPart = 7
kControlClockPart = 8
kControlListBoxPart = 24
kControlListBoxDoubleClickPart = 25
kControlImageWellPart = 26
kControlRadioGroupPart = 27
kControlButtonPart = 10
kControlCheckBoxPart = 11
kControlRadioButtonPart = 11
kControlUpButtonPart = 20
kControlDownButtonPart = 21
kControlPageUpPart = 22
kControlPageDownPart = 23
kControlIndicatorPart = 129
kControlDisabledPart = 254
kControlInactivePart = 255
kControlCheckBoxUncheckedValue = 0
kControlCheckBoxCheckedValue = 1
kControlCheckBoxMixedValue = 2
kControlRadioButtonUncheckedValue = 0
kControlRadioButtonCheckedValue = 1
kControlRadioButtonMixedValue = 2
popupFixedWidth = 1 << 0
popupVariableWidth = 1 << 1
popupUseAddResMenu = 1 << 2
popupUseWFont = 1 << 3
popupTitleBold = 1 << 8
popupTitleItalic = 1 << 9
popupTitleUnderline = 1 << 10
popupTitleOutline = 1 << 11
popupTitleShadow = 1 << 12
popupTitleCondense = 1 << 13
popupTitleExtend = 1 << 14
popupTitleNoStyle = 1 << 15
popupTitleLeftJust = 0x00000000
popupTitleCenterJust = 0x00000001
popupTitleRightJust = 0x000000FF
noConstraint = kNoConstraint
hAxisOnly = 1
vAxisOnly = 2
drawCntl = 0
testCntl = 1
calcCRgns = 2
initCntl = 3
dispCntl = 4
posCntl = 5
thumbCntl = 6
dragCntl = 7
autoTrack = 8
calcCntlRgn = 10
calcThumbRgn = 11
drawThumbOutline = 12
kDrawControlEntireControl = 0
kDrawControlIndicatorOnly = 129
kDragControlEntireControl = 0
kDragControlIndicator = 1
useWFont = popupUseWFont
kControlCheckboxUncheckedValue = kControlCheckBoxUncheckedValue
kControlCheckboxCheckedValue = kControlCheckBoxCheckedValue
kControlCheckboxMixedValue = kControlCheckBoxMixedValue
inLabel = kControlLabelPart
inMenu = kControlMenuPart
inTriangle = kControlTrianglePart
inButton = kControlButtonPart
inCheckBox = kControlCheckBoxPart
inUpButton = kControlUpButtonPart
inDownButton = kControlDownButtonPart
inPageUp = kControlPageUpPart
inPageDown = kControlPageDownPart
inThumb = kControlIndicatorPart
kNoHiliteControlPart = kControlNoPart
kInLabelControlPart = kControlLabelPart
kInMenuControlPart = kControlMenuPart
kInTriangleControlPart = kControlTrianglePart
kInButtonControlPart = kControlButtonPart
kInCheckBoxControlPart = kControlCheckBoxPart
kInUpButtonControlPart = kControlUpButtonPart
kInDownButtonControlPart = kControlDownButtonPart
kInPageUpControlPart = kControlPageUpPart
kInPageDownControlPart = kControlPageDownPart
kInIndicatorControlPart = kControlIndicatorPart
kReservedControlPart = kControlDisabledPart
kControlInactiveControlPart = kControlInactivePart
