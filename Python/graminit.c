#include "pgenheaders.h"
#include "grammar.h"
static arc arcs_0_0[3] = {
	{2, 1},
	{3, 1},
	{4, 2},
};
static arc arcs_0_1[1] = {
	{0, 1},
};
static arc arcs_0_2[1] = {
	{2, 1},
};
static state states_0[3] = {
	{3, arcs_0_0},
	{1, arcs_0_1},
	{1, arcs_0_2},
};
static arc arcs_1_0[3] = {
	{2, 0},
	{6, 0},
	{7, 1},
};
static arc arcs_1_1[1] = {
	{0, 1},
};
static state states_1[2] = {
	{3, arcs_1_0},
	{1, arcs_1_1},
};
static arc arcs_2_0[1] = {
	{9, 1},
};
static arc arcs_2_1[2] = {
	{2, 1},
	{7, 2},
};
static arc arcs_2_2[1] = {
	{0, 2},
};
static state states_2[3] = {
	{1, arcs_2_0},
	{2, arcs_2_1},
	{1, arcs_2_2},
};
static arc arcs_3_0[1] = {
	{11, 1},
};
static arc arcs_3_1[1] = {
	{12, 2},
};
static arc arcs_3_2[1] = {
	{13, 3},
};
static arc arcs_3_3[1] = {
	{14, 4},
};
static arc arcs_3_4[1] = {
	{15, 5},
};
static arc arcs_3_5[1] = {
	{0, 5},
};
static state states_3[6] = {
	{1, arcs_3_0},
	{1, arcs_3_1},
	{1, arcs_3_2},
	{1, arcs_3_3},
	{1, arcs_3_4},
	{1, arcs_3_5},
};
static arc arcs_4_0[1] = {
	{16, 1},
};
static arc arcs_4_1[2] = {
	{17, 2},
	{18, 3},
};
static arc arcs_4_2[1] = {
	{18, 3},
};
static arc arcs_4_3[1] = {
	{0, 3},
};
static state states_4[4] = {
	{1, arcs_4_0},
	{2, arcs_4_1},
	{1, arcs_4_2},
	{1, arcs_4_3},
};
static arc arcs_5_0[3] = {
	{19, 1},
	{23, 2},
	{24, 3},
};
static arc arcs_5_1[3] = {
	{20, 4},
	{22, 5},
	{0, 1},
};
static arc arcs_5_2[1] = {
	{12, 6},
};
static arc arcs_5_3[1] = {
	{12, 7},
};
static arc arcs_5_4[1] = {
	{21, 8},
};
static arc arcs_5_5[4] = {
	{19, 1},
	{23, 2},
	{24, 3},
	{0, 5},
};
static arc arcs_5_6[2] = {
	{22, 9},
	{0, 6},
};
static arc arcs_5_7[1] = {
	{0, 7},
};
static arc arcs_5_8[2] = {
	{22, 5},
	{0, 8},
};
static arc arcs_5_9[1] = {
	{24, 3},
};
static state states_5[10] = {
	{3, arcs_5_0},
	{3, arcs_5_1},
	{1, arcs_5_2},
	{1, arcs_5_3},
	{1, arcs_5_4},
	{4, arcs_5_5},
	{2, arcs_5_6},
	{1, arcs_5_7},
	{2, arcs_5_8},
	{1, arcs_5_9},
};
static arc arcs_6_0[2] = {
	{12, 1},
	{16, 2},
};
static arc arcs_6_1[1] = {
	{0, 1},
};
static arc arcs_6_2[1] = {
	{25, 3},
};
static arc arcs_6_3[1] = {
	{18, 1},
};
static state states_6[4] = {
	{2, arcs_6_0},
	{1, arcs_6_1},
	{1, arcs_6_2},
	{1, arcs_6_3},
};
static arc arcs_7_0[1] = {
	{19, 1},
};
static arc arcs_7_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_7_2[2] = {
	{19, 1},
	{0, 2},
};
static state states_7[3] = {
	{1, arcs_7_0},
	{2, arcs_7_1},
	{2, arcs_7_2},
};
static arc arcs_8_0[2] = {
	{3, 1},
	{4, 1},
};
static arc arcs_8_1[1] = {
	{0, 1},
};
static state states_8[2] = {
	{2, arcs_8_0},
	{1, arcs_8_1},
};
static arc arcs_9_0[1] = {
	{26, 1},
};
static arc arcs_9_1[2] = {
	{27, 2},
	{2, 3},
};
static arc arcs_9_2[2] = {
	{26, 1},
	{2, 3},
};
static arc arcs_9_3[1] = {
	{0, 3},
};
static state states_9[4] = {
	{1, arcs_9_0},
	{2, arcs_9_1},
	{2, arcs_9_2},
	{1, arcs_9_3},
};
static arc arcs_10_0[9] = {
	{28, 1},
	{29, 1},
	{30, 1},
	{31, 1},
	{32, 1},
	{33, 1},
	{34, 1},
	{35, 1},
	{36, 1},
};
static arc arcs_10_1[1] = {
	{0, 1},
};
static state states_10[2] = {
	{9, arcs_10_0},
	{1, arcs_10_1},
};
static arc arcs_11_0[1] = {
	{9, 1},
};
static arc arcs_11_1[3] = {
	{37, 2},
	{20, 3},
	{0, 1},
};
static arc arcs_11_2[1] = {
	{9, 4},
};
static arc arcs_11_3[1] = {
	{9, 5},
};
static arc arcs_11_4[1] = {
	{0, 4},
};
static arc arcs_11_5[2] = {
	{20, 3},
	{0, 5},
};
static state states_11[6] = {
	{1, arcs_11_0},
	{3, arcs_11_1},
	{1, arcs_11_2},
	{1, arcs_11_3},
	{1, arcs_11_4},
	{2, arcs_11_5},
};
static arc arcs_12_0[11] = {
	{38, 1},
	{39, 1},
	{40, 1},
	{41, 1},
	{42, 1},
	{43, 1},
	{44, 1},
	{45, 1},
	{46, 1},
	{47, 1},
	{48, 1},
};
static arc arcs_12_1[1] = {
	{0, 1},
};
static state states_12[2] = {
	{11, arcs_12_0},
	{1, arcs_12_1},
};
static arc arcs_13_0[1] = {
	{49, 1},
};
static arc arcs_13_1[3] = {
	{21, 2},
	{50, 3},
	{0, 1},
};
static arc arcs_13_2[2] = {
	{22, 4},
	{0, 2},
};
static arc arcs_13_3[1] = {
	{21, 5},
};
static arc arcs_13_4[2] = {
	{21, 2},
	{0, 4},
};
static arc arcs_13_5[2] = {
	{22, 6},
	{0, 5},
};
static arc arcs_13_6[1] = {
	{21, 7},
};
static arc arcs_13_7[2] = {
	{22, 8},
	{0, 7},
};
static arc arcs_13_8[2] = {
	{21, 7},
	{0, 8},
};
static state states_13[9] = {
	{1, arcs_13_0},
	{3, arcs_13_1},
	{2, arcs_13_2},
	{1, arcs_13_3},
	{2, arcs_13_4},
	{2, arcs_13_5},
	{1, arcs_13_6},
	{2, arcs_13_7},
	{2, arcs_13_8},
};
static arc arcs_14_0[1] = {
	{51, 1},
};
static arc arcs_14_1[1] = {
	{52, 2},
};
static arc arcs_14_2[1] = {
	{0, 2},
};
static state states_14[3] = {
	{1, arcs_14_0},
	{1, arcs_14_1},
	{1, arcs_14_2},
};
static arc arcs_15_0[1] = {
	{53, 1},
};
static arc arcs_15_1[1] = {
	{0, 1},
};
static state states_15[2] = {
	{1, arcs_15_0},
	{1, arcs_15_1},
};
static arc arcs_16_0[5] = {
	{54, 1},
	{55, 1},
	{56, 1},
	{57, 1},
	{58, 1},
};
static arc arcs_16_1[1] = {
	{0, 1},
};
static state states_16[2] = {
	{5, arcs_16_0},
	{1, arcs_16_1},
};
static arc arcs_17_0[1] = {
	{59, 1},
};
static arc arcs_17_1[1] = {
	{0, 1},
};
static state states_17[2] = {
	{1, arcs_17_0},
	{1, arcs_17_1},
};
static arc arcs_18_0[1] = {
	{60, 1},
};
static arc arcs_18_1[1] = {
	{0, 1},
};
static state states_18[2] = {
	{1, arcs_18_0},
	{1, arcs_18_1},
};
static arc arcs_19_0[1] = {
	{61, 1},
};
static arc arcs_19_1[2] = {
	{9, 2},
	{0, 1},
};
static arc arcs_19_2[1] = {
	{0, 2},
};
static state states_19[3] = {
	{1, arcs_19_0},
	{2, arcs_19_1},
	{1, arcs_19_2},
};
static arc arcs_20_0[1] = {
	{62, 1},
};
static arc arcs_20_1[1] = {
	{9, 2},
};
static arc arcs_20_2[1] = {
	{0, 2},
};
static state states_20[3] = {
	{1, arcs_20_0},
	{1, arcs_20_1},
	{1, arcs_20_2},
};
static arc arcs_21_0[1] = {
	{63, 1},
};
static arc arcs_21_1[2] = {
	{21, 2},
	{0, 1},
};
static arc arcs_21_2[2] = {
	{22, 3},
	{0, 2},
};
static arc arcs_21_3[1] = {
	{21, 4},
};
static arc arcs_21_4[2] = {
	{22, 5},
	{0, 4},
};
static arc arcs_21_5[1] = {
	{21, 6},
};
static arc arcs_21_6[1] = {
	{0, 6},
};
static state states_21[7] = {
	{1, arcs_21_0},
	{2, arcs_21_1},
	{2, arcs_21_2},
	{1, arcs_21_3},
	{2, arcs_21_4},
	{1, arcs_21_5},
	{1, arcs_21_6},
};
static arc arcs_22_0[2] = {
	{64, 1},
	{66, 2},
};
static arc arcs_22_1[1] = {
	{65, 3},
};
static arc arcs_22_2[1] = {
	{67, 4},
};
static arc arcs_22_3[2] = {
	{22, 1},
	{0, 3},
};
static arc arcs_22_4[1] = {
	{64, 5},
};
static arc arcs_22_5[2] = {
	{23, 6},
	{68, 7},
};
static arc arcs_22_6[1] = {
	{0, 6},
};
static arc arcs_22_7[2] = {
	{22, 8},
	{0, 7},
};
static arc arcs_22_8[1] = {
	{68, 7},
};
static state states_22[9] = {
	{2, arcs_22_0},
	{1, arcs_22_1},
	{1, arcs_22_2},
	{2, arcs_22_3},
	{1, arcs_22_4},
	{2, arcs_22_5},
	{1, arcs_22_6},
	{2, arcs_22_7},
	{1, arcs_22_8},
};
static arc arcs_23_0[1] = {
	{12, 1},
};
static arc arcs_23_1[2] = {
	{12, 2},
	{0, 1},
};
static arc arcs_23_2[1] = {
	{12, 3},
};
static arc arcs_23_3[1] = {
	{0, 3},
};
static state states_23[4] = {
	{1, arcs_23_0},
	{2, arcs_23_1},
	{1, arcs_23_2},
	{1, arcs_23_3},
};
static arc arcs_24_0[1] = {
	{67, 1},
};
static arc arcs_24_1[2] = {
	{12, 2},
	{0, 1},
};
static arc arcs_24_2[1] = {
	{12, 3},
};
static arc arcs_24_3[1] = {
	{0, 3},
};
static state states_24[4] = {
	{1, arcs_24_0},
	{2, arcs_24_1},
	{1, arcs_24_2},
	{1, arcs_24_3},
};
static arc arcs_25_0[1] = {
	{12, 1},
};
static arc arcs_25_1[2] = {
	{69, 0},
	{0, 1},
};
static state states_25[2] = {
	{1, arcs_25_0},
	{2, arcs_25_1},
};
static arc arcs_26_0[1] = {
	{70, 1},
};
static arc arcs_26_1[1] = {
	{12, 2},
};
static arc arcs_26_2[2] = {
	{22, 1},
	{0, 2},
};
static state states_26[3] = {
	{1, arcs_26_0},
	{1, arcs_26_1},
	{2, arcs_26_2},
};
static arc arcs_27_0[1] = {
	{71, 1},
};
static arc arcs_27_1[1] = {
	{72, 2},
};
static arc arcs_27_2[2] = {
	{73, 3},
	{0, 2},
};
static arc arcs_27_3[1] = {
	{21, 4},
};
static arc arcs_27_4[2] = {
	{22, 5},
	{0, 4},
};
static arc arcs_27_5[1] = {
	{21, 6},
};
static arc arcs_27_6[1] = {
	{0, 6},
};
static state states_27[7] = {
	{1, arcs_27_0},
	{1, arcs_27_1},
	{2, arcs_27_2},
	{1, arcs_27_3},
	{2, arcs_27_4},
	{1, arcs_27_5},
	{1, arcs_27_6},
};
static arc arcs_28_0[1] = {
	{74, 1},
};
static arc arcs_28_1[1] = {
	{21, 2},
};
static arc arcs_28_2[2] = {
	{22, 3},
	{0, 2},
};
static arc arcs_28_3[1] = {
	{21, 4},
};
static arc arcs_28_4[1] = {
	{0, 4},
};
static state states_28[5] = {
	{1, arcs_28_0},
	{1, arcs_28_1},
	{2, arcs_28_2},
	{1, arcs_28_3},
	{1, arcs_28_4},
};
static arc arcs_29_0[6] = {
	{75, 1},
	{76, 1},
	{77, 1},
	{78, 1},
	{10, 1},
	{79, 1},
};
static arc arcs_29_1[1] = {
	{0, 1},
};
static state states_29[2] = {
	{6, arcs_29_0},
	{1, arcs_29_1},
};
static arc arcs_30_0[1] = {
	{80, 1},
};
static arc arcs_30_1[1] = {
	{21, 2},
};
static arc arcs_30_2[1] = {
	{14, 3},
};
static arc arcs_30_3[1] = {
	{15, 4},
};
static arc arcs_30_4[3] = {
	{81, 1},
	{82, 5},
	{0, 4},
};
static arc arcs_30_5[1] = {
	{14, 6},
};
static arc arcs_30_6[1] = {
	{15, 7},
};
static arc arcs_30_7[1] = {
	{0, 7},
};
static state states_30[8] = {
	{1, arcs_30_0},
	{1, arcs_30_1},
	{1, arcs_30_2},
	{1, arcs_30_3},
	{3, arcs_30_4},
	{1, arcs_30_5},
	{1, arcs_30_6},
	{1, arcs_30_7},
};
static arc arcs_31_0[1] = {
	{83, 1},
};
static arc arcs_31_1[1] = {
	{21, 2},
};
static arc arcs_31_2[1] = {
	{14, 3},
};
static arc arcs_31_3[1] = {
	{15, 4},
};
static arc arcs_31_4[2] = {
	{82, 5},
	{0, 4},
};
static arc arcs_31_5[1] = {
	{14, 6},
};
static arc arcs_31_6[1] = {
	{15, 7},
};
static arc arcs_31_7[1] = {
	{0, 7},
};
static state states_31[8] = {
	{1, arcs_31_0},
	{1, arcs_31_1},
	{1, arcs_31_2},
	{1, arcs_31_3},
	{2, arcs_31_4},
	{1, arcs_31_5},
	{1, arcs_31_6},
	{1, arcs_31_7},
};
static arc arcs_32_0[1] = {
	{84, 1},
};
static arc arcs_32_1[1] = {
	{52, 2},
};
static arc arcs_32_2[1] = {
	{73, 3},
};
static arc arcs_32_3[1] = {
	{9, 4},
};
static arc arcs_32_4[1] = {
	{14, 5},
};
static arc arcs_32_5[1] = {
	{15, 6},
};
static arc arcs_32_6[2] = {
	{82, 7},
	{0, 6},
};
static arc arcs_32_7[1] = {
	{14, 8},
};
static arc arcs_32_8[1] = {
	{15, 9},
};
static arc arcs_32_9[1] = {
	{0, 9},
};
static state states_32[10] = {
	{1, arcs_32_0},
	{1, arcs_32_1},
	{1, arcs_32_2},
	{1, arcs_32_3},
	{1, arcs_32_4},
	{1, arcs_32_5},
	{2, arcs_32_6},
	{1, arcs_32_7},
	{1, arcs_32_8},
	{1, arcs_32_9},
};
static arc arcs_33_0[1] = {
	{85, 1},
};
static arc arcs_33_1[1] = {
	{14, 2},
};
static arc arcs_33_2[1] = {
	{15, 3},
};
static arc arcs_33_3[2] = {
	{86, 4},
	{87, 5},
};
static arc arcs_33_4[1] = {
	{14, 6},
};
static arc arcs_33_5[1] = {
	{14, 7},
};
static arc arcs_33_6[1] = {
	{15, 8},
};
static arc arcs_33_7[1] = {
	{15, 9},
};
static arc arcs_33_8[3] = {
	{86, 4},
	{82, 5},
	{0, 8},
};
static arc arcs_33_9[1] = {
	{0, 9},
};
static state states_33[10] = {
	{1, arcs_33_0},
	{1, arcs_33_1},
	{1, arcs_33_2},
	{2, arcs_33_3},
	{1, arcs_33_4},
	{1, arcs_33_5},
	{1, arcs_33_6},
	{1, arcs_33_7},
	{3, arcs_33_8},
	{1, arcs_33_9},
};
static arc arcs_34_0[1] = {
	{88, 1},
};
static arc arcs_34_1[2] = {
	{21, 2},
	{0, 1},
};
static arc arcs_34_2[2] = {
	{22, 3},
	{0, 2},
};
static arc arcs_34_3[1] = {
	{21, 4},
};
static arc arcs_34_4[1] = {
	{0, 4},
};
static state states_34[5] = {
	{1, arcs_34_0},
	{2, arcs_34_1},
	{2, arcs_34_2},
	{1, arcs_34_3},
	{1, arcs_34_4},
};
static arc arcs_35_0[2] = {
	{3, 1},
	{2, 2},
};
static arc arcs_35_1[1] = {
	{0, 1},
};
static arc arcs_35_2[1] = {
	{89, 3},
};
static arc arcs_35_3[1] = {
	{6, 4},
};
static arc arcs_35_4[2] = {
	{6, 4},
	{90, 1},
};
static state states_35[5] = {
	{2, arcs_35_0},
	{1, arcs_35_1},
	{1, arcs_35_2},
	{1, arcs_35_3},
	{2, arcs_35_4},
};
static arc arcs_36_0[2] = {
	{91, 1},
	{93, 2},
};
static arc arcs_36_1[2] = {
	{92, 3},
	{0, 1},
};
static arc arcs_36_2[1] = {
	{0, 2},
};
static arc arcs_36_3[1] = {
	{91, 1},
};
static state states_36[4] = {
	{2, arcs_36_0},
	{2, arcs_36_1},
	{1, arcs_36_2},
	{1, arcs_36_3},
};
static arc arcs_37_0[1] = {
	{94, 1},
};
static arc arcs_37_1[2] = {
	{95, 0},
	{0, 1},
};
static state states_37[2] = {
	{1, arcs_37_0},
	{2, arcs_37_1},
};
static arc arcs_38_0[2] = {
	{96, 1},
	{97, 2},
};
static arc arcs_38_1[1] = {
	{94, 2},
};
static arc arcs_38_2[1] = {
	{0, 2},
};
static state states_38[3] = {
	{2, arcs_38_0},
	{1, arcs_38_1},
	{1, arcs_38_2},
};
static arc arcs_39_0[1] = {
	{72, 1},
};
static arc arcs_39_1[2] = {
	{98, 0},
	{0, 1},
};
static state states_39[2] = {
	{1, arcs_39_0},
	{2, arcs_39_1},
};
static arc arcs_40_0[10] = {
	{99, 1},
	{100, 1},
	{101, 1},
	{102, 1},
	{103, 1},
	{104, 1},
	{105, 1},
	{73, 1},
	{96, 2},
	{106, 3},
};
static arc arcs_40_1[1] = {
	{0, 1},
};
static arc arcs_40_2[1] = {
	{73, 1},
};
static arc arcs_40_3[2] = {
	{96, 1},
	{0, 3},
};
static state states_40[4] = {
	{10, arcs_40_0},
	{1, arcs_40_1},
	{1, arcs_40_2},
	{2, arcs_40_3},
};
static arc arcs_41_0[1] = {
	{107, 1},
};
static arc arcs_41_1[2] = {
	{108, 0},
	{0, 1},
};
static state states_41[2] = {
	{1, arcs_41_0},
	{2, arcs_41_1},
};
static arc arcs_42_0[1] = {
	{109, 1},
};
static arc arcs_42_1[2] = {
	{110, 0},
	{0, 1},
};
static state states_42[2] = {
	{1, arcs_42_0},
	{2, arcs_42_1},
};
static arc arcs_43_0[1] = {
	{111, 1},
};
static arc arcs_43_1[2] = {
	{112, 0},
	{0, 1},
};
static state states_43[2] = {
	{1, arcs_43_0},
	{2, arcs_43_1},
};
static arc arcs_44_0[1] = {
	{113, 1},
};
static arc arcs_44_1[3] = {
	{114, 0},
	{50, 0},
	{0, 1},
};
static state states_44[2] = {
	{1, arcs_44_0},
	{3, arcs_44_1},
};
static arc arcs_45_0[1] = {
	{115, 1},
};
static arc arcs_45_1[3] = {
	{116, 0},
	{117, 0},
	{0, 1},
};
static state states_45[2] = {
	{1, arcs_45_0},
	{3, arcs_45_1},
};
static arc arcs_46_0[1] = {
	{118, 1},
};
static arc arcs_46_1[4] = {
	{23, 0},
	{119, 0},
	{120, 0},
	{0, 1},
};
static state states_46[2] = {
	{1, arcs_46_0},
	{4, arcs_46_1},
};
static arc arcs_47_0[4] = {
	{116, 1},
	{117, 1},
	{121, 1},
	{122, 2},
};
static arc arcs_47_1[1] = {
	{118, 2},
};
static arc arcs_47_2[1] = {
	{0, 2},
};
static state states_47[3] = {
	{4, arcs_47_0},
	{1, arcs_47_1},
	{1, arcs_47_2},
};
static arc arcs_48_0[1] = {
	{123, 1},
};
static arc arcs_48_1[3] = {
	{124, 1},
	{24, 2},
	{0, 1},
};
static arc arcs_48_2[1] = {
	{118, 3},
};
static arc arcs_48_3[2] = {
	{24, 2},
	{0, 3},
};
static state states_48[4] = {
	{1, arcs_48_0},
	{3, arcs_48_1},
	{1, arcs_48_2},
	{2, arcs_48_3},
};
static arc arcs_49_0[7] = {
	{16, 1},
	{125, 2},
	{128, 3},
	{131, 4},
	{12, 5},
	{132, 5},
	{133, 6},
};
static arc arcs_49_1[2] = {
	{9, 7},
	{18, 5},
};
static arc arcs_49_2[2] = {
	{126, 8},
	{127, 5},
};
static arc arcs_49_3[2] = {
	{129, 9},
	{130, 5},
};
static arc arcs_49_4[1] = {
	{9, 10},
};
static arc arcs_49_5[1] = {
	{0, 5},
};
static arc arcs_49_6[2] = {
	{133, 6},
	{0, 6},
};
static arc arcs_49_7[1] = {
	{18, 5},
};
static arc arcs_49_8[1] = {
	{127, 5},
};
static arc arcs_49_9[1] = {
	{130, 5},
};
static arc arcs_49_10[1] = {
	{131, 5},
};
static state states_49[11] = {
	{7, arcs_49_0},
	{2, arcs_49_1},
	{2, arcs_49_2},
	{2, arcs_49_3},
	{1, arcs_49_4},
	{1, arcs_49_5},
	{2, arcs_49_6},
	{1, arcs_49_7},
	{1, arcs_49_8},
	{1, arcs_49_9},
	{1, arcs_49_10},
};
static arc arcs_50_0[1] = {
	{21, 1},
};
static arc arcs_50_1[3] = {
	{134, 2},
	{22, 3},
	{0, 1},
};
static arc arcs_50_2[1] = {
	{0, 2},
};
static arc arcs_50_3[2] = {
	{21, 4},
	{0, 3},
};
static arc arcs_50_4[2] = {
	{22, 3},
	{0, 4},
};
static state states_50[5] = {
	{1, arcs_50_0},
	{3, arcs_50_1},
	{1, arcs_50_2},
	{2, arcs_50_3},
	{2, arcs_50_4},
};
static arc arcs_51_0[1] = {
	{135, 1},
};
static arc arcs_51_1[2] = {
	{17, 2},
	{14, 3},
};
static arc arcs_51_2[1] = {
	{14, 3},
};
static arc arcs_51_3[1] = {
	{21, 4},
};
static arc arcs_51_4[1] = {
	{0, 4},
};
static state states_51[5] = {
	{1, arcs_51_0},
	{2, arcs_51_1},
	{1, arcs_51_2},
	{1, arcs_51_3},
	{1, arcs_51_4},
};
static arc arcs_52_0[3] = {
	{16, 1},
	{125, 2},
	{69, 3},
};
static arc arcs_52_1[2] = {
	{136, 4},
	{18, 5},
};
static arc arcs_52_2[1] = {
	{137, 6},
};
static arc arcs_52_3[1] = {
	{12, 5},
};
static arc arcs_52_4[1] = {
	{18, 5},
};
static arc arcs_52_5[1] = {
	{0, 5},
};
static arc arcs_52_6[1] = {
	{127, 5},
};
static state states_52[7] = {
	{3, arcs_52_0},
	{2, arcs_52_1},
	{1, arcs_52_2},
	{1, arcs_52_3},
	{1, arcs_52_4},
	{1, arcs_52_5},
	{1, arcs_52_6},
};
static arc arcs_53_0[1] = {
	{138, 1},
};
static arc arcs_53_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_53_2[2] = {
	{138, 1},
	{0, 2},
};
static state states_53[3] = {
	{1, arcs_53_0},
	{2, arcs_53_1},
	{2, arcs_53_2},
};
static arc arcs_54_0[3] = {
	{69, 1},
	{21, 2},
	{14, 3},
};
static arc arcs_54_1[1] = {
	{69, 4},
};
static arc arcs_54_2[2] = {
	{14, 3},
	{0, 2},
};
static arc arcs_54_3[3] = {
	{21, 5},
	{139, 6},
	{0, 3},
};
static arc arcs_54_4[1] = {
	{69, 6},
};
static arc arcs_54_5[2] = {
	{139, 6},
	{0, 5},
};
static arc arcs_54_6[1] = {
	{0, 6},
};
static state states_54[7] = {
	{3, arcs_54_0},
	{1, arcs_54_1},
	{2, arcs_54_2},
	{3, arcs_54_3},
	{1, arcs_54_4},
	{2, arcs_54_5},
	{1, arcs_54_6},
};
static arc arcs_55_0[1] = {
	{14, 1},
};
static arc arcs_55_1[2] = {
	{21, 2},
	{0, 1},
};
static arc arcs_55_2[1] = {
	{0, 2},
};
static state states_55[3] = {
	{1, arcs_55_0},
	{2, arcs_55_1},
	{1, arcs_55_2},
};
static arc arcs_56_0[1] = {
	{72, 1},
};
static arc arcs_56_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_56_2[2] = {
	{72, 1},
	{0, 2},
};
static state states_56[3] = {
	{1, arcs_56_0},
	{2, arcs_56_1},
	{2, arcs_56_2},
};
static arc arcs_57_0[1] = {
	{21, 1},
};
static arc arcs_57_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_57_2[2] = {
	{21, 1},
	{0, 2},
};
static state states_57[3] = {
	{1, arcs_57_0},
	{2, arcs_57_1},
	{2, arcs_57_2},
};
static arc arcs_58_0[1] = {
	{21, 1},
};
static arc arcs_58_1[1] = {
	{14, 2},
};
static arc arcs_58_2[1] = {
	{21, 3},
};
static arc arcs_58_3[2] = {
	{22, 4},
	{0, 3},
};
static arc arcs_58_4[2] = {
	{21, 1},
	{0, 4},
};
static state states_58[5] = {
	{1, arcs_58_0},
	{1, arcs_58_1},
	{1, arcs_58_2},
	{2, arcs_58_3},
	{2, arcs_58_4},
};
static arc arcs_59_0[1] = {
	{140, 1},
};
static arc arcs_59_1[1] = {
	{12, 2},
};
static arc arcs_59_2[2] = {
	{16, 3},
	{14, 4},
};
static arc arcs_59_3[1] = {
	{9, 5},
};
static arc arcs_59_4[1] = {
	{15, 6},
};
static arc arcs_59_5[1] = {
	{18, 7},
};
static arc arcs_59_6[1] = {
	{0, 6},
};
static arc arcs_59_7[1] = {
	{14, 4},
};
static state states_59[8] = {
	{1, arcs_59_0},
	{1, arcs_59_1},
	{2, arcs_59_2},
	{1, arcs_59_3},
	{1, arcs_59_4},
	{1, arcs_59_5},
	{1, arcs_59_6},
	{1, arcs_59_7},
};
static arc arcs_60_0[3] = {
	{141, 1},
	{23, 2},
	{24, 3},
};
static arc arcs_60_1[2] = {
	{22, 4},
	{0, 1},
};
static arc arcs_60_2[1] = {
	{21, 5},
};
static arc arcs_60_3[1] = {
	{21, 6},
};
static arc arcs_60_4[4] = {
	{141, 1},
	{23, 2},
	{24, 3},
	{0, 4},
};
static arc arcs_60_5[2] = {
	{22, 7},
	{0, 5},
};
static arc arcs_60_6[1] = {
	{0, 6},
};
static arc arcs_60_7[1] = {
	{24, 3},
};
static state states_60[8] = {
	{3, arcs_60_0},
	{2, arcs_60_1},
	{1, arcs_60_2},
	{1, arcs_60_3},
	{4, arcs_60_4},
	{2, arcs_60_5},
	{1, arcs_60_6},
	{1, arcs_60_7},
};
static arc arcs_61_0[1] = {
	{21, 1},
};
static arc arcs_61_1[2] = {
	{20, 2},
	{0, 1},
};
static arc arcs_61_2[1] = {
	{21, 3},
};
static arc arcs_61_3[1] = {
	{0, 3},
};
static state states_61[4] = {
	{1, arcs_61_0},
	{2, arcs_61_1},
	{1, arcs_61_2},
	{1, arcs_61_3},
};
static arc arcs_62_0[2] = {
	{134, 1},
	{143, 1},
};
static arc arcs_62_1[1] = {
	{0, 1},
};
static state states_62[2] = {
	{2, arcs_62_0},
	{1, arcs_62_1},
};
static arc arcs_63_0[1] = {
	{84, 1},
};
static arc arcs_63_1[1] = {
	{52, 2},
};
static arc arcs_63_2[1] = {
	{73, 3},
};
static arc arcs_63_3[1] = {
	{9, 4},
};
static arc arcs_63_4[2] = {
	{142, 5},
	{0, 4},
};
static arc arcs_63_5[1] = {
	{0, 5},
};
static state states_63[6] = {
	{1, arcs_63_0},
	{1, arcs_63_1},
	{1, arcs_63_2},
	{1, arcs_63_3},
	{2, arcs_63_4},
	{1, arcs_63_5},
};
static arc arcs_64_0[1] = {
	{80, 1},
};
static arc arcs_64_1[1] = {
	{21, 2},
};
static arc arcs_64_2[2] = {
	{142, 3},
	{0, 2},
};
static arc arcs_64_3[1] = {
	{0, 3},
};
static state states_64[4] = {
	{1, arcs_64_0},
	{1, arcs_64_1},
	{2, arcs_64_2},
	{1, arcs_64_3},
};
static dfa dfas[65] = {
	{256, "single_input", 0, 3, states_0,
	 "\004\030\001\000\000\000\052\370\305\004\071\000\001\000\060\042\271\020"},
	{257, "file_input", 0, 2, states_1,
	 "\204\030\001\000\000\000\052\370\305\004\071\000\001\000\060\042\271\020"},
	{258, "eval_input", 0, 3, states_2,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{259, "funcdef", 0, 6, states_3,
	 "\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{260, "parameters", 0, 4, states_4,
	 "\000\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{261, "varargslist", 0, 10, states_5,
	 "\000\020\201\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{262, "fpdef", 0, 4, states_6,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{263, "fplist", 0, 3, states_7,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{264, "stmt", 0, 2, states_8,
	 "\000\030\001\000\000\000\052\370\305\004\071\000\001\000\060\042\271\020"},
	{265, "simple_stmt", 0, 4, states_9,
	 "\000\020\001\000\000\000\052\370\305\004\000\000\001\000\060\042\271\000"},
	{266, "small_stmt", 0, 2, states_10,
	 "\000\020\001\000\000\000\052\370\305\004\000\000\001\000\060\042\271\000"},
	{267, "expr_stmt", 0, 6, states_11,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{268, "augassign", 0, 2, states_12,
	 "\000\000\000\000\300\377\001\000\000\000\000\000\000\000\000\000\000\000"},
	{269, "print_stmt", 0, 9, states_13,
	 "\000\000\000\000\000\000\002\000\000\000\000\000\000\000\000\000\000\000"},
	{270, "del_stmt", 0, 3, states_14,
	 "\000\000\000\000\000\000\010\000\000\000\000\000\000\000\000\000\000\000"},
	{271, "pass_stmt", 0, 2, states_15,
	 "\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\000\000\000"},
	{272, "flow_stmt", 0, 2, states_16,
	 "\000\000\000\000\000\000\000\370\000\000\000\000\000\000\000\000\000\000"},
	{273, "break_stmt", 0, 2, states_17,
	 "\000\000\000\000\000\000\000\010\000\000\000\000\000\000\000\000\000\000"},
	{274, "continue_stmt", 0, 2, states_18,
	 "\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000\000\000\000"},
	{275, "return_stmt", 0, 3, states_19,
	 "\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\000\000"},
	{276, "yield_stmt", 0, 3, states_20,
	 "\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000\000"},
	{277, "raise_stmt", 0, 7, states_21,
	 "\000\000\000\000\000\000\000\200\000\000\000\000\000\000\000\000\000\000"},
	{278, "import_stmt", 0, 9, states_22,
	 "\000\000\000\000\000\000\000\000\005\000\000\000\000\000\000\000\000\000"},
	{279, "import_as_name", 0, 4, states_23,
	 "\000\020\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{280, "dotted_as_name", 0, 4, states_24,
	 "\000\020\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{281, "dotted_name", 0, 2, states_25,
	 "\000\020\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{282, "global_stmt", 0, 3, states_26,
	 "\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000"},
	{283, "exec_stmt", 0, 7, states_27,
	 "\000\000\000\000\000\000\000\000\200\000\000\000\000\000\000\000\000\000"},
	{284, "assert_stmt", 0, 5, states_28,
	 "\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000"},
	{285, "compound_stmt", 0, 2, states_29,
	 "\000\010\000\000\000\000\000\000\000\000\071\000\000\000\000\000\000\020"},
	{286, "if_stmt", 0, 8, states_30,
	 "\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\000\000"},
	{287, "while_stmt", 0, 8, states_31,
	 "\000\000\000\000\000\000\000\000\000\000\010\000\000\000\000\000\000\000"},
	{288, "for_stmt", 0, 10, states_32,
	 "\000\000\000\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000"},
	{289, "try_stmt", 0, 10, states_33,
	 "\000\000\000\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000"},
	{290, "except_clause", 0, 5, states_34,
	 "\000\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\000"},
	{291, "suite", 0, 5, states_35,
	 "\004\020\001\000\000\000\052\370\305\004\000\000\001\000\060\042\271\000"},
	{292, "test", 0, 4, states_36,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{293, "and_test", 0, 2, states_37,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\071\000"},
	{294, "not_test", 0, 3, states_38,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\071\000"},
	{295, "comparison", 0, 2, states_39,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{296, "comp_op", 0, 4, states_40,
	 "\000\000\000\000\000\000\000\000\000\002\000\000\371\007\000\000\000\000"},
	{297, "expr", 0, 2, states_41,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{298, "xor_expr", 0, 2, states_42,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{299, "and_expr", 0, 2, states_43,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{300, "shift_expr", 0, 2, states_44,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{301, "arith_expr", 0, 2, states_45,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{302, "term", 0, 2, states_46,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{303, "factor", 0, 3, states_47,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{304, "power", 0, 4, states_48,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\000\040\071\000"},
	{305, "atom", 0, 11, states_49,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\000\040\071\000"},
	{306, "listmaker", 0, 5, states_50,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{307, "lambdef", 0, 5, states_51,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\200\000"},
	{308, "trailer", 0, 7, states_52,
	 "\000\000\001\000\000\000\000\000\040\000\000\000\000\000\000\040\000\000"},
	{309, "subscriptlist", 0, 3, states_53,
	 "\000\120\001\000\000\000\000\000\040\000\000\000\001\000\060\042\271\000"},
	{310, "subscript", 0, 7, states_54,
	 "\000\120\001\000\000\000\000\000\040\000\000\000\001\000\060\042\271\000"},
	{311, "sliceop", 0, 3, states_55,
	 "\000\100\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{312, "exprlist", 0, 3, states_56,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\000\000\060\042\071\000"},
	{313, "testlist", 0, 3, states_57,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{314, "dictmaker", 0, 5, states_58,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{315, "classdef", 0, 8, states_59,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\020"},
	{316, "arglist", 0, 8, states_60,
	 "\000\020\201\001\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{317, "argument", 0, 4, states_61,
	 "\000\020\001\000\000\000\000\000\000\000\000\000\001\000\060\042\271\000"},
	{318, "list_iter", 0, 2, states_62,
	 "\000\000\000\000\000\000\000\000\000\000\021\000\000\000\000\000\000\000"},
	{319, "list_for", 0, 6, states_63,
	 "\000\000\000\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000"},
	{320, "list_if", 0, 4, states_64,
	 "\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\000\000"},
};
static label labels[144] = {
	{0, "EMPTY"},
	{256, 0},
	{4, 0},
	{265, 0},
	{285, 0},
	{257, 0},
	{264, 0},
	{0, 0},
	{258, 0},
	{313, 0},
	{259, 0},
	{1, "def"},
	{1, 0},
	{260, 0},
	{11, 0},
	{291, 0},
	{7, 0},
	{261, 0},
	{8, 0},
	{262, 0},
	{22, 0},
	{292, 0},
	{12, 0},
	{16, 0},
	{36, 0},
	{263, 0},
	{266, 0},
	{13, 0},
	{267, 0},
	{269, 0},
	{270, 0},
	{271, 0},
	{272, 0},
	{278, 0},
	{282, 0},
	{283, 0},
	{284, 0},
	{268, 0},
	{37, 0},
	{38, 0},
	{39, 0},
	{40, 0},
	{41, 0},
	{42, 0},
	{43, 0},
	{44, 0},
	{45, 0},
	{46, 0},
	{47, 0},
	{1, "print"},
	{35, 0},
	{1, "del"},
	{312, 0},
	{1, "pass"},
	{273, 0},
	{274, 0},
	{275, 0},
	{277, 0},
	{276, 0},
	{1, "break"},
	{1, "continue"},
	{1, "return"},
	{1, "yield"},
	{1, "raise"},
	{1, "import"},
	{280, 0},
	{1, "from"},
	{281, 0},
	{279, 0},
	{23, 0},
	{1, "global"},
	{1, "exec"},
	{297, 0},
	{1, "in"},
	{1, "assert"},
	{286, 0},
	{287, 0},
	{288, 0},
	{289, 0},
	{315, 0},
	{1, "if"},
	{1, "elif"},
	{1, "else"},
	{1, "while"},
	{1, "for"},
	{1, "try"},
	{290, 0},
	{1, "finally"},
	{1, "except"},
	{5, 0},
	{6, 0},
	{293, 0},
	{1, "or"},
	{307, 0},
	{294, 0},
	{1, "and"},
	{1, "not"},
	{295, 0},
	{296, 0},
	{20, 0},
	{21, 0},
	{28, 0},
	{31, 0},
	{30, 0},
	{29, 0},
	{29, 0},
	{1, "is"},
	{298, 0},
	{18, 0},
	{299, 0},
	{33, 0},
	{300, 0},
	{19, 0},
	{301, 0},
	{34, 0},
	{302, 0},
	{14, 0},
	{15, 0},
	{303, 0},
	{17, 0},
	{24, 0},
	{32, 0},
	{304, 0},
	{305, 0},
	{308, 0},
	{9, 0},
	{306, 0},
	{10, 0},
	{26, 0},
	{314, 0},
	{27, 0},
	{25, 0},
	{2, 0},
	{3, 0},
	{319, 0},
	{1, "lambda"},
	{316, 0},
	{309, 0},
	{310, 0},
	{311, 0},
	{1, "class"},
	{317, 0},
	{318, 0},
	{320, 0},
};
grammar _PyParser_Grammar = {
	65,
	dfas,
	{144, labels},
	256
};
