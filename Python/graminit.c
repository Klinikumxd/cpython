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
static arc arcs_3_2[2] = {
	{13, 3},
	{2, 4},
};
static arc arcs_3_3[2] = {
	{14, 5},
	{15, 6},
};
static arc arcs_3_4[1] = {
	{0, 4},
};
static arc arcs_3_5[1] = {
	{15, 6},
};
static arc arcs_3_6[1] = {
	{2, 4},
};
static state states_3[7] = {
	{1, arcs_3_0},
	{1, arcs_3_1},
	{2, arcs_3_2},
	{2, arcs_3_3},
	{1, arcs_3_4},
	{1, arcs_3_5},
	{1, arcs_3_6},
};
static arc arcs_4_0[1] = {
	{10, 1},
};
static arc arcs_4_1[2] = {
	{10, 1},
	{0, 1},
};
static state states_4[2] = {
	{1, arcs_4_0},
	{2, arcs_4_1},
};
static arc arcs_5_0[2] = {
	{16, 1},
	{18, 2},
};
static arc arcs_5_1[1] = {
	{18, 2},
};
static arc arcs_5_2[1] = {
	{19, 3},
};
static arc arcs_5_3[1] = {
	{20, 4},
};
static arc arcs_5_4[2] = {
	{21, 5},
	{23, 6},
};
static arc arcs_5_5[1] = {
	{22, 7},
};
static arc arcs_5_6[1] = {
	{24, 8},
};
static arc arcs_5_7[1] = {
	{23, 6},
};
static arc arcs_5_8[1] = {
	{0, 8},
};
static state states_5[9] = {
	{2, arcs_5_0},
	{1, arcs_5_1},
	{1, arcs_5_2},
	{1, arcs_5_3},
	{2, arcs_5_4},
	{1, arcs_5_5},
	{1, arcs_5_6},
	{1, arcs_5_7},
	{1, arcs_5_8},
};
static arc arcs_6_0[1] = {
	{13, 1},
};
static arc arcs_6_1[2] = {
	{25, 2},
	{15, 3},
};
static arc arcs_6_2[1] = {
	{15, 3},
};
static arc arcs_6_3[1] = {
	{0, 3},
};
static state states_6[4] = {
	{1, arcs_6_0},
	{2, arcs_6_1},
	{1, arcs_6_2},
	{1, arcs_6_3},
};
static arc arcs_7_0[3] = {
	{26, 1},
	{29, 2},
	{31, 3},
};
static arc arcs_7_1[3] = {
	{27, 4},
	{28, 5},
	{0, 1},
};
static arc arcs_7_2[3] = {
	{30, 6},
	{28, 7},
	{0, 2},
};
static arc arcs_7_3[1] = {
	{30, 8},
};
static arc arcs_7_4[1] = {
	{22, 9},
};
static arc arcs_7_5[4] = {
	{26, 1},
	{29, 2},
	{31, 3},
	{0, 5},
};
static arc arcs_7_6[2] = {
	{28, 7},
	{0, 6},
};
static arc arcs_7_7[2] = {
	{30, 10},
	{31, 3},
};
static arc arcs_7_8[1] = {
	{0, 8},
};
static arc arcs_7_9[2] = {
	{28, 5},
	{0, 9},
};
static arc arcs_7_10[3] = {
	{28, 7},
	{27, 11},
	{0, 10},
};
static arc arcs_7_11[1] = {
	{22, 6},
};
static state states_7[12] = {
	{3, arcs_7_0},
	{3, arcs_7_1},
	{3, arcs_7_2},
	{1, arcs_7_3},
	{1, arcs_7_4},
	{4, arcs_7_5},
	{2, arcs_7_6},
	{2, arcs_7_7},
	{1, arcs_7_8},
	{2, arcs_7_9},
	{3, arcs_7_10},
	{1, arcs_7_11},
};
static arc arcs_8_0[1] = {
	{19, 1},
};
static arc arcs_8_1[2] = {
	{23, 2},
	{0, 1},
};
static arc arcs_8_2[1] = {
	{22, 3},
};
static arc arcs_8_3[1] = {
	{0, 3},
};
static state states_8[4] = {
	{1, arcs_8_0},
	{2, arcs_8_1},
	{1, arcs_8_2},
	{1, arcs_8_3},
};
static arc arcs_9_0[2] = {
	{30, 1},
	{13, 2},
};
static arc arcs_9_1[1] = {
	{0, 1},
};
static arc arcs_9_2[1] = {
	{32, 3},
};
static arc arcs_9_3[1] = {
	{15, 1},
};
static state states_9[4] = {
	{2, arcs_9_0},
	{1, arcs_9_1},
	{1, arcs_9_2},
	{1, arcs_9_3},
};
static arc arcs_10_0[1] = {
	{26, 1},
};
static arc arcs_10_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_10_2[2] = {
	{26, 1},
	{0, 2},
};
static state states_10[3] = {
	{1, arcs_10_0},
	{2, arcs_10_1},
	{2, arcs_10_2},
};
static arc arcs_11_0[3] = {
	{34, 1},
	{29, 2},
	{31, 3},
};
static arc arcs_11_1[3] = {
	{27, 4},
	{28, 5},
	{0, 1},
};
static arc arcs_11_2[3] = {
	{35, 6},
	{28, 7},
	{0, 2},
};
static arc arcs_11_3[1] = {
	{35, 8},
};
static arc arcs_11_4[1] = {
	{22, 9},
};
static arc arcs_11_5[4] = {
	{34, 1},
	{29, 2},
	{31, 3},
	{0, 5},
};
static arc arcs_11_6[2] = {
	{28, 7},
	{0, 6},
};
static arc arcs_11_7[2] = {
	{35, 10},
	{31, 3},
};
static arc arcs_11_8[1] = {
	{0, 8},
};
static arc arcs_11_9[2] = {
	{28, 5},
	{0, 9},
};
static arc arcs_11_10[3] = {
	{28, 7},
	{27, 11},
	{0, 10},
};
static arc arcs_11_11[1] = {
	{22, 6},
};
static state states_11[12] = {
	{3, arcs_11_0},
	{3, arcs_11_1},
	{3, arcs_11_2},
	{1, arcs_11_3},
	{1, arcs_11_4},
	{4, arcs_11_5},
	{2, arcs_11_6},
	{2, arcs_11_7},
	{1, arcs_11_8},
	{2, arcs_11_9},
	{3, arcs_11_10},
	{1, arcs_11_11},
};
static arc arcs_12_0[1] = {
	{19, 1},
};
static arc arcs_12_1[1] = {
	{0, 1},
};
static state states_12[2] = {
	{1, arcs_12_0},
	{1, arcs_12_1},
};
static arc arcs_13_0[2] = {
	{35, 1},
	{13, 2},
};
static arc arcs_13_1[1] = {
	{0, 1},
};
static arc arcs_13_2[1] = {
	{36, 3},
};
static arc arcs_13_3[1] = {
	{15, 1},
};
static state states_13[4] = {
	{2, arcs_13_0},
	{1, arcs_13_1},
	{1, arcs_13_2},
	{1, arcs_13_3},
};
static arc arcs_14_0[1] = {
	{34, 1},
};
static arc arcs_14_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_14_2[2] = {
	{34, 1},
	{0, 2},
};
static state states_14[3] = {
	{1, arcs_14_0},
	{2, arcs_14_1},
	{2, arcs_14_2},
};
static arc arcs_15_0[2] = {
	{3, 1},
	{4, 1},
};
static arc arcs_15_1[1] = {
	{0, 1},
};
static state states_15[2] = {
	{2, arcs_15_0},
	{1, arcs_15_1},
};
static arc arcs_16_0[1] = {
	{37, 1},
};
static arc arcs_16_1[2] = {
	{38, 2},
	{2, 3},
};
static arc arcs_16_2[2] = {
	{37, 1},
	{2, 3},
};
static arc arcs_16_3[1] = {
	{0, 3},
};
static state states_16[4] = {
	{1, arcs_16_0},
	{2, arcs_16_1},
	{2, arcs_16_2},
	{1, arcs_16_3},
};
static arc arcs_17_0[7] = {
	{39, 1},
	{40, 1},
	{41, 1},
	{42, 1},
	{43, 1},
	{44, 1},
	{45, 1},
};
static arc arcs_17_1[1] = {
	{0, 1},
};
static state states_17[2] = {
	{7, arcs_17_0},
	{1, arcs_17_1},
};
static arc arcs_18_0[1] = {
	{9, 1},
};
static arc arcs_18_1[3] = {
	{46, 2},
	{27, 3},
	{0, 1},
};
static arc arcs_18_2[2] = {
	{47, 4},
	{9, 4},
};
static arc arcs_18_3[2] = {
	{47, 5},
	{9, 5},
};
static arc arcs_18_4[1] = {
	{0, 4},
};
static arc arcs_18_5[2] = {
	{27, 3},
	{0, 5},
};
static state states_18[6] = {
	{1, arcs_18_0},
	{3, arcs_18_1},
	{2, arcs_18_2},
	{2, arcs_18_3},
	{1, arcs_18_4},
	{2, arcs_18_5},
};
static arc arcs_19_0[12] = {
	{48, 1},
	{49, 1},
	{50, 1},
	{51, 1},
	{52, 1},
	{53, 1},
	{54, 1},
	{55, 1},
	{56, 1},
	{57, 1},
	{58, 1},
	{59, 1},
};
static arc arcs_19_1[1] = {
	{0, 1},
};
static state states_19[2] = {
	{12, arcs_19_0},
	{1, arcs_19_1},
};
static arc arcs_20_0[1] = {
	{60, 1},
};
static arc arcs_20_1[1] = {
	{61, 2},
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
	{62, 1},
};
static arc arcs_21_1[1] = {
	{0, 1},
};
static state states_21[2] = {
	{1, arcs_21_0},
	{1, arcs_21_1},
};
static arc arcs_22_0[5] = {
	{63, 1},
	{64, 1},
	{65, 1},
	{66, 1},
	{67, 1},
};
static arc arcs_22_1[1] = {
	{0, 1},
};
static state states_22[2] = {
	{5, arcs_22_0},
	{1, arcs_22_1},
};
static arc arcs_23_0[1] = {
	{68, 1},
};
static arc arcs_23_1[1] = {
	{0, 1},
};
static state states_23[2] = {
	{1, arcs_23_0},
	{1, arcs_23_1},
};
static arc arcs_24_0[1] = {
	{69, 1},
};
static arc arcs_24_1[1] = {
	{0, 1},
};
static state states_24[2] = {
	{1, arcs_24_0},
	{1, arcs_24_1},
};
static arc arcs_25_0[1] = {
	{70, 1},
};
static arc arcs_25_1[2] = {
	{9, 2},
	{0, 1},
};
static arc arcs_25_2[1] = {
	{0, 2},
};
static state states_25[3] = {
	{1, arcs_25_0},
	{2, arcs_25_1},
	{1, arcs_25_2},
};
static arc arcs_26_0[1] = {
	{47, 1},
};
static arc arcs_26_1[1] = {
	{0, 1},
};
static state states_26[2] = {
	{1, arcs_26_0},
	{1, arcs_26_1},
};
static arc arcs_27_0[1] = {
	{71, 1},
};
static arc arcs_27_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_27_2[2] = {
	{28, 3},
	{0, 2},
};
static arc arcs_27_3[1] = {
	{22, 4},
};
static arc arcs_27_4[2] = {
	{28, 5},
	{0, 4},
};
static arc arcs_27_5[1] = {
	{22, 6},
};
static arc arcs_27_6[1] = {
	{0, 6},
};
static state states_27[7] = {
	{1, arcs_27_0},
	{2, arcs_27_1},
	{2, arcs_27_2},
	{1, arcs_27_3},
	{2, arcs_27_4},
	{1, arcs_27_5},
	{1, arcs_27_6},
};
static arc arcs_28_0[2] = {
	{72, 1},
	{73, 1},
};
static arc arcs_28_1[1] = {
	{0, 1},
};
static state states_28[2] = {
	{2, arcs_28_0},
	{1, arcs_28_1},
};
static arc arcs_29_0[1] = {
	{74, 1},
};
static arc arcs_29_1[1] = {
	{75, 2},
};
static arc arcs_29_2[1] = {
	{0, 2},
};
static state states_29[3] = {
	{1, arcs_29_0},
	{1, arcs_29_1},
	{1, arcs_29_2},
};
static arc arcs_30_0[1] = {
	{76, 1},
};
static arc arcs_30_1[2] = {
	{77, 2},
	{12, 3},
};
static arc arcs_30_2[3] = {
	{77, 2},
	{12, 3},
	{74, 4},
};
static arc arcs_30_3[1] = {
	{74, 4},
};
static arc arcs_30_4[3] = {
	{29, 5},
	{13, 6},
	{78, 5},
};
static arc arcs_30_5[1] = {
	{0, 5},
};
static arc arcs_30_6[1] = {
	{78, 7},
};
static arc arcs_30_7[1] = {
	{15, 5},
};
static state states_30[8] = {
	{1, arcs_30_0},
	{2, arcs_30_1},
	{3, arcs_30_2},
	{1, arcs_30_3},
	{3, arcs_30_4},
	{1, arcs_30_5},
	{1, arcs_30_6},
	{1, arcs_30_7},
};
static arc arcs_31_0[1] = {
	{19, 1},
};
static arc arcs_31_1[2] = {
	{80, 2},
	{0, 1},
};
static arc arcs_31_2[1] = {
	{19, 3},
};
static arc arcs_31_3[1] = {
	{0, 3},
};
static state states_31[4] = {
	{1, arcs_31_0},
	{2, arcs_31_1},
	{1, arcs_31_2},
	{1, arcs_31_3},
};
static arc arcs_32_0[1] = {
	{12, 1},
};
static arc arcs_32_1[2] = {
	{80, 2},
	{0, 1},
};
static arc arcs_32_2[1] = {
	{19, 3},
};
static arc arcs_32_3[1] = {
	{0, 3},
};
static state states_32[4] = {
	{1, arcs_32_0},
	{2, arcs_32_1},
	{1, arcs_32_2},
	{1, arcs_32_3},
};
static arc arcs_33_0[1] = {
	{79, 1},
};
static arc arcs_33_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_33_2[2] = {
	{79, 1},
	{0, 2},
};
static state states_33[3] = {
	{1, arcs_33_0},
	{2, arcs_33_1},
	{2, arcs_33_2},
};
static arc arcs_34_0[1] = {
	{81, 1},
};
static arc arcs_34_1[2] = {
	{28, 0},
	{0, 1},
};
static state states_34[2] = {
	{1, arcs_34_0},
	{2, arcs_34_1},
};
static arc arcs_35_0[1] = {
	{19, 1},
};
static arc arcs_35_1[2] = {
	{77, 0},
	{0, 1},
};
static state states_35[2] = {
	{1, arcs_35_0},
	{2, arcs_35_1},
};
static arc arcs_36_0[1] = {
	{82, 1},
};
static arc arcs_36_1[1] = {
	{19, 2},
};
static arc arcs_36_2[2] = {
	{28, 1},
	{0, 2},
};
static state states_36[3] = {
	{1, arcs_36_0},
	{1, arcs_36_1},
	{2, arcs_36_2},
};
static arc arcs_37_0[1] = {
	{83, 1},
};
static arc arcs_37_1[1] = {
	{22, 2},
};
static arc arcs_37_2[2] = {
	{28, 3},
	{0, 2},
};
static arc arcs_37_3[1] = {
	{22, 4},
};
static arc arcs_37_4[1] = {
	{0, 4},
};
static state states_37[5] = {
	{1, arcs_37_0},
	{1, arcs_37_1},
	{2, arcs_37_2},
	{1, arcs_37_3},
	{1, arcs_37_4},
};
static arc arcs_38_0[7] = {
	{84, 1},
	{85, 1},
	{86, 1},
	{87, 1},
	{88, 1},
	{17, 1},
	{89, 1},
};
static arc arcs_38_1[1] = {
	{0, 1},
};
static state states_38[2] = {
	{7, arcs_38_0},
	{1, arcs_38_1},
};
static arc arcs_39_0[1] = {
	{90, 1},
};
static arc arcs_39_1[1] = {
	{22, 2},
};
static arc arcs_39_2[1] = {
	{23, 3},
};
static arc arcs_39_3[1] = {
	{24, 4},
};
static arc arcs_39_4[3] = {
	{91, 1},
	{92, 5},
	{0, 4},
};
static arc arcs_39_5[1] = {
	{23, 6},
};
static arc arcs_39_6[1] = {
	{24, 7},
};
static arc arcs_39_7[1] = {
	{0, 7},
};
static state states_39[8] = {
	{1, arcs_39_0},
	{1, arcs_39_1},
	{1, arcs_39_2},
	{1, arcs_39_3},
	{3, arcs_39_4},
	{1, arcs_39_5},
	{1, arcs_39_6},
	{1, arcs_39_7},
};
static arc arcs_40_0[1] = {
	{93, 1},
};
static arc arcs_40_1[1] = {
	{22, 2},
};
static arc arcs_40_2[1] = {
	{23, 3},
};
static arc arcs_40_3[1] = {
	{24, 4},
};
static arc arcs_40_4[2] = {
	{92, 5},
	{0, 4},
};
static arc arcs_40_5[1] = {
	{23, 6},
};
static arc arcs_40_6[1] = {
	{24, 7},
};
static arc arcs_40_7[1] = {
	{0, 7},
};
static state states_40[8] = {
	{1, arcs_40_0},
	{1, arcs_40_1},
	{1, arcs_40_2},
	{1, arcs_40_3},
	{2, arcs_40_4},
	{1, arcs_40_5},
	{1, arcs_40_6},
	{1, arcs_40_7},
};
static arc arcs_41_0[1] = {
	{94, 1},
};
static arc arcs_41_1[1] = {
	{61, 2},
};
static arc arcs_41_2[1] = {
	{95, 3},
};
static arc arcs_41_3[1] = {
	{9, 4},
};
static arc arcs_41_4[1] = {
	{23, 5},
};
static arc arcs_41_5[1] = {
	{24, 6},
};
static arc arcs_41_6[2] = {
	{92, 7},
	{0, 6},
};
static arc arcs_41_7[1] = {
	{23, 8},
};
static arc arcs_41_8[1] = {
	{24, 9},
};
static arc arcs_41_9[1] = {
	{0, 9},
};
static state states_41[10] = {
	{1, arcs_41_0},
	{1, arcs_41_1},
	{1, arcs_41_2},
	{1, arcs_41_3},
	{1, arcs_41_4},
	{1, arcs_41_5},
	{2, arcs_41_6},
	{1, arcs_41_7},
	{1, arcs_41_8},
	{1, arcs_41_9},
};
static arc arcs_42_0[1] = {
	{96, 1},
};
static arc arcs_42_1[1] = {
	{23, 2},
};
static arc arcs_42_2[1] = {
	{24, 3},
};
static arc arcs_42_3[2] = {
	{97, 4},
	{98, 5},
};
static arc arcs_42_4[1] = {
	{23, 6},
};
static arc arcs_42_5[1] = {
	{23, 7},
};
static arc arcs_42_6[1] = {
	{24, 8},
};
static arc arcs_42_7[1] = {
	{24, 9},
};
static arc arcs_42_8[4] = {
	{97, 4},
	{92, 10},
	{98, 5},
	{0, 8},
};
static arc arcs_42_9[1] = {
	{0, 9},
};
static arc arcs_42_10[1] = {
	{23, 11},
};
static arc arcs_42_11[1] = {
	{24, 12},
};
static arc arcs_42_12[2] = {
	{98, 5},
	{0, 12},
};
static state states_42[13] = {
	{1, arcs_42_0},
	{1, arcs_42_1},
	{1, arcs_42_2},
	{2, arcs_42_3},
	{1, arcs_42_4},
	{1, arcs_42_5},
	{1, arcs_42_6},
	{1, arcs_42_7},
	{4, arcs_42_8},
	{1, arcs_42_9},
	{1, arcs_42_10},
	{1, arcs_42_11},
	{2, arcs_42_12},
};
static arc arcs_43_0[1] = {
	{99, 1},
};
static arc arcs_43_1[1] = {
	{22, 2},
};
static arc arcs_43_2[2] = {
	{100, 3},
	{23, 4},
};
static arc arcs_43_3[1] = {
	{23, 4},
};
static arc arcs_43_4[1] = {
	{24, 5},
};
static arc arcs_43_5[1] = {
	{0, 5},
};
static state states_43[6] = {
	{1, arcs_43_0},
	{1, arcs_43_1},
	{2, arcs_43_2},
	{1, arcs_43_3},
	{1, arcs_43_4},
	{1, arcs_43_5},
};
static arc arcs_44_0[1] = {
	{80, 1},
};
static arc arcs_44_1[1] = {
	{101, 2},
};
static arc arcs_44_2[1] = {
	{0, 2},
};
static state states_44[3] = {
	{1, arcs_44_0},
	{1, arcs_44_1},
	{1, arcs_44_2},
};
static arc arcs_45_0[1] = {
	{102, 1},
};
static arc arcs_45_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_45_2[2] = {
	{80, 3},
	{0, 2},
};
static arc arcs_45_3[1] = {
	{19, 4},
};
static arc arcs_45_4[1] = {
	{0, 4},
};
static state states_45[5] = {
	{1, arcs_45_0},
	{2, arcs_45_1},
	{2, arcs_45_2},
	{1, arcs_45_3},
	{1, arcs_45_4},
};
static arc arcs_46_0[2] = {
	{3, 1},
	{2, 2},
};
static arc arcs_46_1[1] = {
	{0, 1},
};
static arc arcs_46_2[1] = {
	{103, 3},
};
static arc arcs_46_3[1] = {
	{6, 4},
};
static arc arcs_46_4[2] = {
	{6, 4},
	{104, 1},
};
static state states_46[5] = {
	{2, arcs_46_0},
	{1, arcs_46_1},
	{1, arcs_46_2},
	{1, arcs_46_3},
	{2, arcs_46_4},
};
static arc arcs_47_0[1] = {
	{106, 1},
};
static arc arcs_47_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_47_2[1] = {
	{106, 3},
};
static arc arcs_47_3[2] = {
	{28, 4},
	{0, 3},
};
static arc arcs_47_4[2] = {
	{106, 3},
	{0, 4},
};
static state states_47[5] = {
	{1, arcs_47_0},
	{2, arcs_47_1},
	{1, arcs_47_2},
	{2, arcs_47_3},
	{2, arcs_47_4},
};
static arc arcs_48_0[2] = {
	{107, 1},
	{108, 1},
};
static arc arcs_48_1[1] = {
	{0, 1},
};
static state states_48[2] = {
	{2, arcs_48_0},
	{1, arcs_48_1},
};
static arc arcs_49_0[1] = {
	{109, 1},
};
static arc arcs_49_1[2] = {
	{33, 2},
	{23, 3},
};
static arc arcs_49_2[1] = {
	{23, 3},
};
static arc arcs_49_3[1] = {
	{106, 4},
};
static arc arcs_49_4[1] = {
	{0, 4},
};
static state states_49[5] = {
	{1, arcs_49_0},
	{2, arcs_49_1},
	{1, arcs_49_2},
	{1, arcs_49_3},
	{1, arcs_49_4},
};
static arc arcs_50_0[2] = {
	{107, 1},
	{110, 2},
};
static arc arcs_50_1[2] = {
	{90, 3},
	{0, 1},
};
static arc arcs_50_2[1] = {
	{0, 2},
};
static arc arcs_50_3[1] = {
	{107, 4},
};
static arc arcs_50_4[1] = {
	{92, 5},
};
static arc arcs_50_5[1] = {
	{22, 2},
};
static state states_50[6] = {
	{2, arcs_50_0},
	{2, arcs_50_1},
	{1, arcs_50_2},
	{1, arcs_50_3},
	{1, arcs_50_4},
	{1, arcs_50_5},
};
static arc arcs_51_0[1] = {
	{111, 1},
};
static arc arcs_51_1[2] = {
	{112, 0},
	{0, 1},
};
static state states_51[2] = {
	{1, arcs_51_0},
	{2, arcs_51_1},
};
static arc arcs_52_0[1] = {
	{113, 1},
};
static arc arcs_52_1[2] = {
	{114, 0},
	{0, 1},
};
static state states_52[2] = {
	{1, arcs_52_0},
	{2, arcs_52_1},
};
static arc arcs_53_0[2] = {
	{115, 1},
	{116, 2},
};
static arc arcs_53_1[1] = {
	{113, 2},
};
static arc arcs_53_2[1] = {
	{0, 2},
};
static state states_53[3] = {
	{2, arcs_53_0},
	{1, arcs_53_1},
	{1, arcs_53_2},
};
static arc arcs_54_0[1] = {
	{101, 1},
};
static arc arcs_54_1[2] = {
	{117, 0},
	{0, 1},
};
static state states_54[2] = {
	{1, arcs_54_0},
	{2, arcs_54_1},
};
static arc arcs_55_0[9] = {
	{118, 1},
	{119, 1},
	{120, 1},
	{121, 1},
	{122, 1},
	{123, 1},
	{95, 1},
	{115, 2},
	{124, 3},
};
static arc arcs_55_1[1] = {
	{0, 1},
};
static arc arcs_55_2[1] = {
	{95, 1},
};
static arc arcs_55_3[2] = {
	{115, 1},
	{0, 3},
};
static state states_55[4] = {
	{9, arcs_55_0},
	{1, arcs_55_1},
	{1, arcs_55_2},
	{2, arcs_55_3},
};
static arc arcs_56_0[1] = {
	{125, 1},
};
static arc arcs_56_1[2] = {
	{126, 0},
	{0, 1},
};
static state states_56[2] = {
	{1, arcs_56_0},
	{2, arcs_56_1},
};
static arc arcs_57_0[1] = {
	{127, 1},
};
static arc arcs_57_1[2] = {
	{128, 0},
	{0, 1},
};
static state states_57[2] = {
	{1, arcs_57_0},
	{2, arcs_57_1},
};
static arc arcs_58_0[1] = {
	{129, 1},
};
static arc arcs_58_1[2] = {
	{130, 0},
	{0, 1},
};
static state states_58[2] = {
	{1, arcs_58_0},
	{2, arcs_58_1},
};
static arc arcs_59_0[1] = {
	{131, 1},
};
static arc arcs_59_1[3] = {
	{132, 0},
	{133, 0},
	{0, 1},
};
static state states_59[2] = {
	{1, arcs_59_0},
	{3, arcs_59_1},
};
static arc arcs_60_0[1] = {
	{134, 1},
};
static arc arcs_60_1[3] = {
	{135, 0},
	{136, 0},
	{0, 1},
};
static state states_60[2] = {
	{1, arcs_60_0},
	{3, arcs_60_1},
};
static arc arcs_61_0[1] = {
	{137, 1},
};
static arc arcs_61_1[5] = {
	{29, 0},
	{138, 0},
	{139, 0},
	{140, 0},
	{0, 1},
};
static state states_61[2] = {
	{1, arcs_61_0},
	{5, arcs_61_1},
};
static arc arcs_62_0[4] = {
	{135, 1},
	{136, 1},
	{141, 1},
	{142, 2},
};
static arc arcs_62_1[1] = {
	{137, 2},
};
static arc arcs_62_2[1] = {
	{0, 2},
};
static state states_62[3] = {
	{4, arcs_62_0},
	{1, arcs_62_1},
	{1, arcs_62_2},
};
static arc arcs_63_0[1] = {
	{143, 1},
};
static arc arcs_63_1[3] = {
	{144, 1},
	{31, 2},
	{0, 1},
};
static arc arcs_63_2[1] = {
	{137, 3},
};
static arc arcs_63_3[1] = {
	{0, 3},
};
static state states_63[4] = {
	{1, arcs_63_0},
	{3, arcs_63_1},
	{1, arcs_63_2},
	{1, arcs_63_3},
};
static arc arcs_64_0[7] = {
	{13, 1},
	{146, 2},
	{149, 3},
	{19, 4},
	{152, 4},
	{153, 5},
	{77, 6},
};
static arc arcs_64_1[3] = {
	{47, 7},
	{145, 7},
	{15, 4},
};
static arc arcs_64_2[2] = {
	{147, 8},
	{148, 4},
};
static arc arcs_64_3[2] = {
	{150, 9},
	{151, 4},
};
static arc arcs_64_4[1] = {
	{0, 4},
};
static arc arcs_64_5[2] = {
	{153, 5},
	{0, 5},
};
static arc arcs_64_6[1] = {
	{77, 10},
};
static arc arcs_64_7[1] = {
	{15, 4},
};
static arc arcs_64_8[1] = {
	{148, 4},
};
static arc arcs_64_9[1] = {
	{151, 4},
};
static arc arcs_64_10[1] = {
	{77, 4},
};
static state states_64[11] = {
	{7, arcs_64_0},
	{3, arcs_64_1},
	{2, arcs_64_2},
	{2, arcs_64_3},
	{1, arcs_64_4},
	{2, arcs_64_5},
	{1, arcs_64_6},
	{1, arcs_64_7},
	{1, arcs_64_8},
	{1, arcs_64_9},
	{1, arcs_64_10},
};
static arc arcs_65_0[1] = {
	{22, 1},
};
static arc arcs_65_1[3] = {
	{154, 2},
	{28, 3},
	{0, 1},
};
static arc arcs_65_2[1] = {
	{0, 2},
};
static arc arcs_65_3[2] = {
	{22, 4},
	{0, 3},
};
static arc arcs_65_4[2] = {
	{28, 3},
	{0, 4},
};
static state states_65[5] = {
	{1, arcs_65_0},
	{3, arcs_65_1},
	{1, arcs_65_2},
	{2, arcs_65_3},
	{2, arcs_65_4},
};
static arc arcs_66_0[1] = {
	{22, 1},
};
static arc arcs_66_1[3] = {
	{155, 2},
	{28, 3},
	{0, 1},
};
static arc arcs_66_2[1] = {
	{0, 2},
};
static arc arcs_66_3[2] = {
	{22, 4},
	{0, 3},
};
static arc arcs_66_4[2] = {
	{28, 3},
	{0, 4},
};
static state states_66[5] = {
	{1, arcs_66_0},
	{3, arcs_66_1},
	{1, arcs_66_2},
	{2, arcs_66_3},
	{2, arcs_66_4},
};
static arc arcs_67_0[1] = {
	{109, 1},
};
static arc arcs_67_1[2] = {
	{33, 2},
	{23, 3},
};
static arc arcs_67_2[1] = {
	{23, 3},
};
static arc arcs_67_3[1] = {
	{22, 4},
};
static arc arcs_67_4[1] = {
	{0, 4},
};
static state states_67[5] = {
	{1, arcs_67_0},
	{2, arcs_67_1},
	{1, arcs_67_2},
	{1, arcs_67_3},
	{1, arcs_67_4},
};
static arc arcs_68_0[3] = {
	{13, 1},
	{146, 2},
	{77, 3},
};
static arc arcs_68_1[2] = {
	{14, 4},
	{15, 5},
};
static arc arcs_68_2[1] = {
	{156, 6},
};
static arc arcs_68_3[1] = {
	{19, 5},
};
static arc arcs_68_4[1] = {
	{15, 5},
};
static arc arcs_68_5[1] = {
	{0, 5},
};
static arc arcs_68_6[1] = {
	{148, 5},
};
static state states_68[7] = {
	{3, arcs_68_0},
	{2, arcs_68_1},
	{1, arcs_68_2},
	{1, arcs_68_3},
	{1, arcs_68_4},
	{1, arcs_68_5},
	{1, arcs_68_6},
};
static arc arcs_69_0[1] = {
	{157, 1},
};
static arc arcs_69_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_69_2[2] = {
	{157, 1},
	{0, 2},
};
static state states_69[3] = {
	{1, arcs_69_0},
	{2, arcs_69_1},
	{2, arcs_69_2},
};
static arc arcs_70_0[2] = {
	{22, 1},
	{23, 2},
};
static arc arcs_70_1[2] = {
	{23, 2},
	{0, 1},
};
static arc arcs_70_2[3] = {
	{22, 3},
	{158, 4},
	{0, 2},
};
static arc arcs_70_3[2] = {
	{158, 4},
	{0, 3},
};
static arc arcs_70_4[1] = {
	{0, 4},
};
static state states_70[5] = {
	{2, arcs_70_0},
	{2, arcs_70_1},
	{3, arcs_70_2},
	{2, arcs_70_3},
	{1, arcs_70_4},
};
static arc arcs_71_0[1] = {
	{23, 1},
};
static arc arcs_71_1[2] = {
	{22, 2},
	{0, 1},
};
static arc arcs_71_2[1] = {
	{0, 2},
};
static state states_71[3] = {
	{1, arcs_71_0},
	{2, arcs_71_1},
	{1, arcs_71_2},
};
static arc arcs_72_0[1] = {
	{101, 1},
};
static arc arcs_72_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_72_2[2] = {
	{101, 1},
	{0, 2},
};
static state states_72[3] = {
	{1, arcs_72_0},
	{2, arcs_72_1},
	{2, arcs_72_2},
};
static arc arcs_73_0[1] = {
	{22, 1},
};
static arc arcs_73_1[2] = {
	{28, 2},
	{0, 1},
};
static arc arcs_73_2[2] = {
	{22, 1},
	{0, 2},
};
static state states_73[3] = {
	{1, arcs_73_0},
	{2, arcs_73_1},
	{2, arcs_73_2},
};
static arc arcs_74_0[1] = {
	{22, 1},
};
static arc arcs_74_1[3] = {
	{23, 2},
	{28, 3},
	{0, 1},
};
static arc arcs_74_2[1] = {
	{22, 4},
};
static arc arcs_74_3[2] = {
	{22, 5},
	{0, 3},
};
static arc arcs_74_4[2] = {
	{28, 6},
	{0, 4},
};
static arc arcs_74_5[2] = {
	{28, 3},
	{0, 5},
};
static arc arcs_74_6[2] = {
	{22, 7},
	{0, 6},
};
static arc arcs_74_7[1] = {
	{23, 2},
};
static state states_74[8] = {
	{1, arcs_74_0},
	{3, arcs_74_1},
	{1, arcs_74_2},
	{2, arcs_74_3},
	{2, arcs_74_4},
	{2, arcs_74_5},
	{2, arcs_74_6},
	{1, arcs_74_7},
};
static arc arcs_75_0[1] = {
	{159, 1},
};
static arc arcs_75_1[1] = {
	{19, 2},
};
static arc arcs_75_2[2] = {
	{13, 3},
	{23, 4},
};
static arc arcs_75_3[2] = {
	{9, 5},
	{15, 6},
};
static arc arcs_75_4[1] = {
	{24, 7},
};
static arc arcs_75_5[1] = {
	{15, 6},
};
static arc arcs_75_6[1] = {
	{23, 4},
};
static arc arcs_75_7[1] = {
	{0, 7},
};
static state states_75[8] = {
	{1, arcs_75_0},
	{1, arcs_75_1},
	{2, arcs_75_2},
	{2, arcs_75_3},
	{1, arcs_75_4},
	{1, arcs_75_5},
	{1, arcs_75_6},
	{1, arcs_75_7},
};
static arc arcs_76_0[3] = {
	{160, 1},
	{29, 2},
	{31, 3},
};
static arc arcs_76_1[2] = {
	{28, 4},
	{0, 1},
};
static arc arcs_76_2[1] = {
	{22, 5},
};
static arc arcs_76_3[1] = {
	{22, 6},
};
static arc arcs_76_4[4] = {
	{160, 1},
	{29, 2},
	{31, 3},
	{0, 4},
};
static arc arcs_76_5[2] = {
	{28, 7},
	{0, 5},
};
static arc arcs_76_6[1] = {
	{0, 6},
};
static arc arcs_76_7[1] = {
	{31, 3},
};
static state states_76[8] = {
	{3, arcs_76_0},
	{2, arcs_76_1},
	{1, arcs_76_2},
	{1, arcs_76_3},
	{4, arcs_76_4},
	{2, arcs_76_5},
	{1, arcs_76_6},
	{1, arcs_76_7},
};
static arc arcs_77_0[1] = {
	{22, 1},
};
static arc arcs_77_1[3] = {
	{155, 2},
	{27, 3},
	{0, 1},
};
static arc arcs_77_2[1] = {
	{0, 2},
};
static arc arcs_77_3[1] = {
	{22, 2},
};
static state states_77[4] = {
	{1, arcs_77_0},
	{3, arcs_77_1},
	{1, arcs_77_2},
	{1, arcs_77_3},
};
static arc arcs_78_0[2] = {
	{154, 1},
	{162, 1},
};
static arc arcs_78_1[1] = {
	{0, 1},
};
static state states_78[2] = {
	{2, arcs_78_0},
	{1, arcs_78_1},
};
static arc arcs_79_0[1] = {
	{94, 1},
};
static arc arcs_79_1[1] = {
	{61, 2},
};
static arc arcs_79_2[1] = {
	{95, 3},
};
static arc arcs_79_3[1] = {
	{105, 4},
};
static arc arcs_79_4[2] = {
	{161, 5},
	{0, 4},
};
static arc arcs_79_5[1] = {
	{0, 5},
};
static state states_79[6] = {
	{1, arcs_79_0},
	{1, arcs_79_1},
	{1, arcs_79_2},
	{1, arcs_79_3},
	{2, arcs_79_4},
	{1, arcs_79_5},
};
static arc arcs_80_0[1] = {
	{90, 1},
};
static arc arcs_80_1[1] = {
	{106, 2},
};
static arc arcs_80_2[2] = {
	{161, 3},
	{0, 2},
};
static arc arcs_80_3[1] = {
	{0, 3},
};
static state states_80[4] = {
	{1, arcs_80_0},
	{1, arcs_80_1},
	{2, arcs_80_2},
	{1, arcs_80_3},
};
static arc arcs_81_0[2] = {
	{155, 1},
	{164, 1},
};
static arc arcs_81_1[1] = {
	{0, 1},
};
static state states_81[2] = {
	{2, arcs_81_0},
	{1, arcs_81_1},
};
static arc arcs_82_0[1] = {
	{94, 1},
};
static arc arcs_82_1[1] = {
	{61, 2},
};
static arc arcs_82_2[1] = {
	{95, 3},
};
static arc arcs_82_3[1] = {
	{107, 4},
};
static arc arcs_82_4[2] = {
	{163, 5},
	{0, 4},
};
static arc arcs_82_5[1] = {
	{0, 5},
};
static state states_82[6] = {
	{1, arcs_82_0},
	{1, arcs_82_1},
	{1, arcs_82_2},
	{1, arcs_82_3},
	{2, arcs_82_4},
	{1, arcs_82_5},
};
static arc arcs_83_0[1] = {
	{90, 1},
};
static arc arcs_83_1[1] = {
	{106, 2},
};
static arc arcs_83_2[2] = {
	{163, 3},
	{0, 2},
};
static arc arcs_83_3[1] = {
	{0, 3},
};
static state states_83[4] = {
	{1, arcs_83_0},
	{1, arcs_83_1},
	{2, arcs_83_2},
	{1, arcs_83_3},
};
static arc arcs_84_0[1] = {
	{22, 1},
};
static arc arcs_84_1[2] = {
	{28, 0},
	{0, 1},
};
static state states_84[2] = {
	{1, arcs_84_0},
	{2, arcs_84_1},
};
static arc arcs_85_0[1] = {
	{19, 1},
};
static arc arcs_85_1[1] = {
	{0, 1},
};
static state states_85[2] = {
	{1, arcs_85_0},
	{1, arcs_85_1},
};
static arc arcs_86_0[1] = {
	{167, 1},
};
static arc arcs_86_1[2] = {
	{9, 2},
	{0, 1},
};
static arc arcs_86_2[1] = {
	{0, 2},
};
static state states_86[3] = {
	{1, arcs_86_0},
	{2, arcs_86_1},
	{1, arcs_86_2},
};
static dfa dfas[87] = {
	{256, "single_input", 0, 3, states_0,
	 "\004\050\014\000\000\000\000\120\360\064\014\144\011\040\010\000\200\041\044\203\200"},
	{257, "file_input", 0, 2, states_1,
	 "\204\050\014\000\000\000\000\120\360\064\014\144\011\040\010\000\200\041\044\203\200"},
	{258, "eval_input", 0, 3, states_2,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{259, "decorator", 0, 7, states_3,
	 "\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{260, "decorators", 0, 2, states_4,
	 "\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{261, "funcdef", 0, 9, states_5,
	 "\000\010\004\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{262, "parameters", 0, 4, states_6,
	 "\000\040\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{263, "typedargslist", 0, 12, states_7,
	 "\000\040\010\240\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{264, "tname", 0, 4, states_8,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{265, "tfpdef", 0, 4, states_9,
	 "\000\040\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{266, "tfplist", 0, 3, states_10,
	 "\000\040\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{267, "varargslist", 0, 12, states_11,
	 "\000\040\010\240\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{268, "vname", 0, 2, states_12,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{269, "vfpdef", 0, 4, states_13,
	 "\000\040\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{270, "vfplist", 0, 3, states_14,
	 "\000\040\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{271, "stmt", 0, 2, states_15,
	 "\000\050\014\000\000\000\000\120\360\064\014\144\011\040\010\000\200\041\044\203\200"},
	{272, "simple_stmt", 0, 4, states_16,
	 "\000\040\010\000\000\000\000\120\360\064\014\000\000\040\010\000\200\041\044\003\200"},
	{273, "small_stmt", 0, 2, states_17,
	 "\000\040\010\000\000\000\000\120\360\064\014\000\000\040\010\000\200\041\044\003\200"},
	{274, "expr_stmt", 0, 6, states_18,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{275, "augassign", 0, 2, states_19,
	 "\000\000\000\000\000\000\377\017\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{276, "del_stmt", 0, 3, states_20,
	 "\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{277, "pass_stmt", 0, 2, states_21,
	 "\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{278, "flow_stmt", 0, 2, states_22,
	 "\000\000\000\000\000\000\000\000\360\000\000\000\000\000\000\000\000\000\000\000\200"},
	{279, "break_stmt", 0, 2, states_23,
	 "\000\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000\000\000\000\000\000"},
	{280, "continue_stmt", 0, 2, states_24,
	 "\000\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\000\000\000\000"},
	{281, "return_stmt", 0, 3, states_25,
	 "\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000\000\000\000"},
	{282, "yield_stmt", 0, 2, states_26,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\200"},
	{283, "raise_stmt", 0, 7, states_27,
	 "\000\000\000\000\000\000\000\000\200\000\000\000\000\000\000\000\000\000\000\000\000"},
	{284, "import_stmt", 0, 2, states_28,
	 "\000\000\000\000\000\000\000\000\000\024\000\000\000\000\000\000\000\000\000\000\000"},
	{285, "import_name", 0, 3, states_29,
	 "\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000\000\000\000"},
	{286, "import_from", 0, 8, states_30,
	 "\000\000\000\000\000\000\000\000\000\020\000\000\000\000\000\000\000\000\000\000\000"},
	{287, "import_as_name", 0, 4, states_31,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{288, "dotted_as_name", 0, 4, states_32,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{289, "import_as_names", 0, 3, states_33,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{290, "dotted_as_names", 0, 2, states_34,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{291, "dotted_name", 0, 2, states_35,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{292, "global_stmt", 0, 3, states_36,
	 "\000\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000\000\000"},
	{293, "assert_stmt", 0, 5, states_37,
	 "\000\000\000\000\000\000\000\000\000\000\010\000\000\000\000\000\000\000\000\000\000"},
	{294, "compound_stmt", 0, 2, states_38,
	 "\000\010\004\000\000\000\000\000\000\000\000\144\011\000\000\000\000\000\000\200\000"},
	{295, "if_stmt", 0, 8, states_39,
	 "\000\000\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000\000"},
	{296, "while_stmt", 0, 8, states_40,
	 "\000\000\000\000\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\000"},
	{297, "for_stmt", 0, 10, states_41,
	 "\000\000\000\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000"},
	{298, "try_stmt", 0, 13, states_42,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\000\000\000"},
	{299, "with_stmt", 0, 6, states_43,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\010\000\000\000\000\000\000\000\000"},
	{300, "with_var", 0, 3, states_44,
	 "\000\000\000\000\000\000\000\000\000\000\001\000\000\000\000\000\000\000\000\000\000"},
	{301, "except_clause", 0, 5, states_45,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000"},
	{302, "suite", 0, 5, states_46,
	 "\004\040\010\000\000\000\000\120\360\064\014\000\000\040\010\000\200\041\044\003\200"},
	{303, "testlist_safe", 0, 5, states_47,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{304, "old_test", 0, 2, states_48,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{305, "old_lambdef", 0, 5, states_49,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000"},
	{306, "test", 0, 6, states_50,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{307, "or_test", 0, 2, states_51,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\010\000\200\041\044\003\000"},
	{308, "and_test", 0, 2, states_52,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\010\000\200\041\044\003\000"},
	{309, "not_test", 0, 3, states_53,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\010\000\200\041\044\003\000"},
	{310, "comparison", 0, 2, states_54,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{311, "comp_op", 0, 4, states_55,
	 "\000\000\000\000\000\000\000\000\000\000\000\200\000\000\310\037\000\000\000\000\000"},
	{312, "expr", 0, 2, states_56,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{313, "xor_expr", 0, 2, states_57,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{314, "and_expr", 0, 2, states_58,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{315, "shift_expr", 0, 2, states_59,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{316, "arith_expr", 0, 2, states_60,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{317, "term", 0, 2, states_61,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{318, "factor", 0, 3, states_62,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{319, "power", 0, 4, states_63,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\044\003\000"},
	{320, "atom", 0, 11, states_64,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\044\003\000"},
	{321, "listmaker", 0, 5, states_65,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{322, "testlist_gexp", 0, 5, states_66,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{323, "lambdef", 0, 5, states_67,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000"},
	{324, "trailer", 0, 7, states_68,
	 "\000\040\000\000\000\000\000\000\000\040\000\000\000\000\000\000\000\000\004\000\000"},
	{325, "subscriptlist", 0, 3, states_69,
	 "\000\040\210\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{326, "subscript", 0, 5, states_70,
	 "\000\040\210\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{327, "sliceop", 0, 3, states_71,
	 "\000\000\200\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{328, "exprlist", 0, 3, states_72,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\000\000\000\200\041\044\003\000"},
	{329, "testlist", 0, 3, states_73,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{330, "dictsetmaker", 0, 8, states_74,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{331, "classdef", 0, 8, states_75,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\200\000"},
	{332, "arglist", 0, 8, states_76,
	 "\000\040\010\240\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{333, "argument", 0, 4, states_77,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{334, "list_iter", 0, 2, states_78,
	 "\000\000\000\000\000\000\000\000\000\000\000\104\000\000\000\000\000\000\000\000\000"},
	{335, "list_for", 0, 6, states_79,
	 "\000\000\000\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000"},
	{336, "list_if", 0, 4, states_80,
	 "\000\000\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000\000"},
	{337, "gen_iter", 0, 2, states_81,
	 "\000\000\000\000\000\000\000\000\000\000\000\104\000\000\000\000\000\000\000\000\000"},
	{338, "gen_for", 0, 6, states_82,
	 "\000\000\000\000\000\000\000\000\000\000\000\100\000\000\000\000\000\000\000\000\000"},
	{339, "gen_if", 0, 4, states_83,
	 "\000\000\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\000\000\000\000"},
	{340, "testlist1", 0, 2, states_84,
	 "\000\040\010\000\000\000\000\000\000\040\000\000\000\040\010\000\200\041\044\003\000"},
	{341, "encoding_decl", 0, 2, states_85,
	 "\000\000\010\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"},
	{342, "yield_expr", 0, 3, states_86,
	 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\200"},
};
static label labels[168] = {
	{0, "EMPTY"},
	{256, 0},
	{4, 0},
	{272, 0},
	{294, 0},
	{257, 0},
	{271, 0},
	{0, 0},
	{258, 0},
	{329, 0},
	{259, 0},
	{50, 0},
	{291, 0},
	{7, 0},
	{332, 0},
	{8, 0},
	{260, 0},
	{261, 0},
	{1, "def"},
	{1, 0},
	{262, 0},
	{51, 0},
	{306, 0},
	{11, 0},
	{302, 0},
	{263, 0},
	{265, 0},
	{22, 0},
	{12, 0},
	{16, 0},
	{264, 0},
	{36, 0},
	{266, 0},
	{267, 0},
	{269, 0},
	{268, 0},
	{270, 0},
	{273, 0},
	{13, 0},
	{274, 0},
	{276, 0},
	{277, 0},
	{278, 0},
	{284, 0},
	{292, 0},
	{293, 0},
	{275, 0},
	{342, 0},
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
	{49, 0},
	{1, "del"},
	{328, 0},
	{1, "pass"},
	{279, 0},
	{280, 0},
	{281, 0},
	{283, 0},
	{282, 0},
	{1, "break"},
	{1, "continue"},
	{1, "return"},
	{1, "raise"},
	{285, 0},
	{286, 0},
	{1, "import"},
	{290, 0},
	{1, "from"},
	{23, 0},
	{289, 0},
	{287, 0},
	{1, "as"},
	{288, 0},
	{1, "global"},
	{1, "assert"},
	{295, 0},
	{296, 0},
	{297, 0},
	{298, 0},
	{299, 0},
	{331, 0},
	{1, "if"},
	{1, "elif"},
	{1, "else"},
	{1, "while"},
	{1, "for"},
	{1, "in"},
	{1, "try"},
	{301, 0},
	{1, "finally"},
	{1, "with"},
	{300, 0},
	{312, 0},
	{1, "except"},
	{5, 0},
	{6, 0},
	{303, 0},
	{304, 0},
	{307, 0},
	{305, 0},
	{1, "lambda"},
	{323, 0},
	{308, 0},
	{1, "or"},
	{309, 0},
	{1, "and"},
	{1, "not"},
	{310, 0},
	{311, 0},
	{20, 0},
	{21, 0},
	{28, 0},
	{31, 0},
	{30, 0},
	{29, 0},
	{1, "is"},
	{313, 0},
	{18, 0},
	{314, 0},
	{33, 0},
	{315, 0},
	{19, 0},
	{316, 0},
	{34, 0},
	{35, 0},
	{317, 0},
	{14, 0},
	{15, 0},
	{318, 0},
	{17, 0},
	{24, 0},
	{48, 0},
	{32, 0},
	{319, 0},
	{320, 0},
	{324, 0},
	{322, 0},
	{9, 0},
	{321, 0},
	{10, 0},
	{26, 0},
	{330, 0},
	{27, 0},
	{2, 0},
	{3, 0},
	{335, 0},
	{338, 0},
	{325, 0},
	{326, 0},
	{327, 0},
	{1, "class"},
	{333, 0},
	{334, 0},
	{336, 0},
	{337, 0},
	{339, 0},
	{340, 0},
	{341, 0},
	{1, "yield"},
};
grammar _PyParser_Grammar = {
	87,
	dfas,
	{168, labels},
	256
};
