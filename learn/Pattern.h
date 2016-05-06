#pragma once
#include <map>

typedef unsigned char PatternVal8;
typedef unsigned int PatternVal32;
typedef unsigned long long PatternVal64;
typedef unsigned int HashKey;

// 90度回転(12-point diamond)
inline PatternVal64 rotate_diamond12(const PatternVal64& val64)
{
	//         [1 ]
	//     [2 ][3 ][4 ]
	// [5 ][6 ][  ][7 ][8 ]
	//     [9 ][10][11]
	//         [12]
	// から
	//         [5 ]
	//     [9 ][6 ][2 ]
	// [12][10][  ][3 ][1 ]
	//     [11][7 ][4 ]
	//         [8 ]

	PatternVal64 rot = 0;

	// 石の色、呼吸点
	// 1 → 8
	// 4 → 11
	rot |= (val64 & 0b000000000000000000000000000000001111000000001111) << ((8 - 1)/*(11 - 4)*/ * 4);
	// 2 → 4
	rot |= (val64 & 0b000000000000000000000000000000000000000011110000) << ((4 - 2) * 4);
	// 3 → 7
	// 8 → 12
	rot |= (val64 & 0b000000000000000011110000000000000000111100000000) << ((7 - 3)/*(12 - 8)*/ * 4);
	// 5 → 1
	// 10 → 6
	rot |= (val64 & 0b000000001111000000000000000011110000000000000000) >> ((5 - 1)/*(10 - 6)*/ * 4);
	// 6 → 3
	rot |= (val64 & 0b000000000000000000000000111100000000000000000000) >> ((6 - 3) * 4);
	// 7 → 10
	rot |= (val64 & 0b000000000000000000001111000000000000000000000000) << ((10 - 7) * 4);
	// 9 → 2
	// 12 → 5
	rot |= (val64 & 0b111100000000111100000000000000000000000000000000) >> ((9 - 2)/*(12 - 5)*/ * 4);
	// 11 → 9
	rot |= (val64 & 0b000011110000000000000000000000000000000000000000) >> ((11 - 9) * 4);

	return rot;
}

// 上下反転(12-point diamond)
inline PatternVal64 vmirror_diamond12(const PatternVal64& val64) {
	//         [1 ]
	//     [2 ][3 ][4 ]
	// [5 ][6 ][  ][7 ][8 ]
	//     [9 ][10][11]
	//         [12]
	// から
	//         [12]
	//     [9 ][10][11]
	// [5 ][6 ][  ][7 ][8 ]
	//     [2 ][3 ][4 ]
	//         [1 ]

	PatternVal64 rot = 0;

	// 石の色
	// 1 → 12
	rot |= (val64 & 0b000000000000000000000000000000000000000000001111) << ((12 - 1) * 4);
	// 2 → 9
	// 3 → 10
	// 4 → 11
	rot |= (val64 & 0b000000000000000000000000000000001111111111110000) << ((9 - 2)/*(10 - 3)*//*(11 - 4)*/ * 4);
	// 9 → 2
	// 10 → 3
	// 11 → 4
	rot |= (val64 & 0b000011111111111100000000000000000000000000000000) >> ((9 - 2)/*(10 - 3)*//*(11 - 4)*/ * 4);
	// 12 → 1
	rot |= (val64 & 0b111100000000000000000000000000000000000000000000) >> ((12 - 1) * 4);

	return rot;
}

// 左右反転(12-point diamond)
inline PatternVal64 hmirror_diamond12(const PatternVal64& val64)
{
	//         [1 ]
	//     [2 ][3 ][4 ]
	// [5 ][6 ][  ][7 ][8 ]
	//     [9 ][10][11]
	//         [12]
	// から
	//         [1 ]
	//     [4 ][3 ][2 ]
	// [8 ][7 ][  ][6 ][5 ]
	//     [11][10][9 ]
	//         [12]

	PatternVal64 rot = 0;

	// 石の色
	// 2 → 4
	// 9 → 11
	rot |= (val64 & 0b000000000000111100000000000000000000000011110000) >> ((4 - 2)/*(11 - 9)*/ * 4);
	// 5 → 8
	rot |= (val64 & 0b000000000000000000000000000011110000000000000000) >> ((8 - 5) * 4);
	// 6 → 7
	rot |= (val64 & 0b000000000000000000000000111100000000000000000000) >> ((7 - 6) * 4);
	// 4 → 2
	// 11 → 9
	rot |= (val64 & 0b000011110000000000000000000000001111000000000000) >> ((4 - 2)/*(11 - 9)*/ * 4);
	// 7 → 6
	rot |= (val64 & 0b000000000000000000001111000000000000000000000000) >> ((7 - 6) * 4);
	// 8 → 5
	rot |= (val64 & 0b000000000000000011110000000000000000000000000000) >> ((8 - 5) * 4);

	return rot;
}

struct Diamond12PatternVal
{
	__declspec(align(8)) struct Vals {
		PatternVal8 color_liberties[12 / 2];
	};

	union {
		PatternVal64 val64;
		Vals vals;
	};

	Diamond12PatternVal() {}

	Diamond12PatternVal(const Diamond12PatternVal& val) : val64(val.val64) {}

	Diamond12PatternVal(const PatternVal64 val64) : val64(val64) {}

	bool operator ==(const Diamond12PatternVal& val) const {
		return val64 == val.val64;
	}

	bool operator !=(const Diamond12PatternVal& val) const {
		return val64 != val.val64;
	}

	bool operator <(const Diamond12PatternVal& val) const {
		return val64 < val.val64;
	}

	// 90度回転
	Diamond12PatternVal rotate() const {
		return rotate_diamond12(val64);
	}

	// 上下反転
	Diamond12PatternVal vmirror() const {
		return vmirror_diamond12(val64);
	}

	// 左右反転
	Diamond12PatternVal hmirror() const {
		return hmirror_diamond12(val64);
	}
};

struct ResponsePatternVal
{
	__declspec(align(8)) struct Vals {
		PatternVal8 color_liberties[12 / 2];
		PatternVal8 move_pos;
	};

	union {
		PatternVal64 val64;
		Vals vals;
	};

	ResponsePatternVal() {}

	ResponsePatternVal(const ResponsePatternVal& val) : val64(val.val64) {}

	ResponsePatternVal(const unsigned long long val64) : val64(val64) {}

	ResponsePatternVal(const PatternVal64 color_liberties, const PatternVal8 move_pos) : val64(color_liberties) {
		this->vals.move_pos = move_pos;
	}

	bool operator ==(const ResponsePatternVal& val) const {
		return val64 == val.val64;
	}

	bool operator !=(const ResponsePatternVal& val) const {
		return val64 != val.val64;
	}

	bool operator <(const ResponsePatternVal& val) const {
		return val64 < val.val64;
	}

	// 90度回転
	ResponsePatternVal rotate() const {
		ResponsePatternVal rot = rotate_diamond12(val64);

		// move_pos
		// [0 ][1 ][2 ][3 ][4 ]
		// [5 ][6 ][7 ][8 ][9 ]
		// [10][11][12][13][14]
		// [15][16][17][18][19]
		// [20][21][22][23][24]
		// から
		// [20][15][10][5 ][0 ]
		// [21][16][11][6 ][1 ]
		// [22][17][12][7 ][2 ]
		// [23][18][13][8 ][3 ]
		// [24][19][14][9 ][4 ]

		const PatternVal8 rot_tbl[] = { 4, 9, 14, 19, 24, 3, 8, 13, 18, 23, 2, 7, 12, 17, 22, 1, 6, 11, 16, 21, 0, 5, 10, 15, 20 };
		rot.vals.move_pos |= rot_tbl[vals.move_pos];

		return rot;
	}

	// 上下反転
	ResponsePatternVal vmirror() const {
		ResponsePatternVal rot = vmirror_diamond12(val64);

		// move_pos
		// [0 ][1 ][2 ][3 ][4 ]
		// [5 ][6 ][7 ][8 ][9 ]
		// [10][11][12][13][14]
		// [15][16][17][18][19]
		// [20][21][22][23][24]
		// から
		// [20][21][22][23][24]
		// [15][16][17][18][19]
		// [10][11][12][13][14]
		// [5 ][6 ][7 ][8 ][9 ]
		// [0 ][1 ][2 ][3 ][4 ]

		const PatternVal8 mirror_tbl[] = { 20, 21, 22, 23, 24, 15, 16, 17, 18, 19, 10, 11, 12, 13, 14, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4 };
		rot.vals.move_pos = mirror_tbl[vals.move_pos];

		return rot;
	}

	// 左右反転
	ResponsePatternVal hmirror() const {
		ResponsePatternVal rot = hmirror_diamond12(val64);

		// move_pos
		// [0 ][1 ][2 ][3 ][4 ]
		// [5 ][6 ][7 ][8 ][9 ]
		// [10][11][12][13][14]
		// [15][16][17][18][19]
		// [20][21][22][23][24]
		// から
		// [4 ][3 ][2 ][1 ][0 ]
		// [9 ][8 ][7 ][6 ][5 ]
		// [14][13][12][11][10]
		// [19][18][17][16][15]
		// [24][23][22][21][20]

		const PatternVal8 mirror_tbl[] = { 4, 3, 2, 1, 0, 9, 8, 7, 6, 5, 14, 13, 12, 11, 10, 19, 18, 17, 16, 15, 24, 23, 22, 21, 20 };
		rot.vals.move_pos = mirror_tbl[vals.move_pos];

		return rot;
	}
};

struct NonResponsePatternVal
{
	__declspec(align(8)) struct Vals {
		PatternVal8 color_liberties[8 / 2];
	};

	union {
		PatternVal32 val32;
		Vals vals;
	};

	NonResponsePatternVal() {}

	NonResponsePatternVal(const NonResponsePatternVal& val) {
		val32 = val.val32;
	}

	NonResponsePatternVal(const PatternVal32 val32) {
		this->val32 = val32;
	}

	bool operator ==(const NonResponsePatternVal& val) const {
		return val32 == val.val32;
	}

	bool operator !=(const NonResponsePatternVal& val) const {
		return val32 != val.val32;
	}

	bool operator <(const NonResponsePatternVal& val) const {
		return val32 < val.val32;
	}

	// 90度回転
	NonResponsePatternVal rotate() const {
		// [1][2][3]
		// [4][ ][5]
		// [6][7][8]
		// から
		// [6][4][1]
		// [7][ ][2]
		// [8][5][3]

		NonResponsePatternVal rot = 0;

		// 石の色
		// 1 → 3
		// 5 → 7
		rot.val32 |= (val32 & 0b00000000000011110000000000001111) << ((3 - 1)/*(7 - 5)*/ * 4);
		// 2 → 5
		rot.val32 |= (val32 & 0b00000000000000000000000011110000) << ((5 - 2) * 4);
		// 3 → 8
		rot.val32 |= (val32 & 0b00000000000000000000111100000000) << ((8 - 3) * 4);
		// 4 → 2
		// 8 → 6
		rot.val32 |= (val32 & 0b11110000000000001111000000000000) >> ((4 - 2)/*(8 - 6)*/ * 4);
		// 6 → 1
		rot.val32 |= (val32 & 0b00000000111100000000000000000000) >> ((6 - 1) * 4);
		// 7 → 4
		rot.val32 |= (val32 & 0b00001111000000000000000000000000) >> ((7 - 4) * 4);

		return rot;
	}

	// 上下反転
	NonResponsePatternVal vmirror() const {
		// [1][2][3]
		// [4][ ][5]
		// [6][7][8]
		// から
		// [6][7][8]
		// [4][ ][5]
		// [1][2][3]

		NonResponsePatternVal rot = 0;

		// 1 → 6
		// 2 → 7
		// 3 → 8
		rot.val32 |= (val32 & 0b00000000000000000000111111111111) << ((6 - 1)/*(7 - 2)*//*(8 - 3)*/ * 4);
		// 6 → 1
		// 7 → 2
		// 8 → 3
		rot.val32 |= (val32 & 0b11111111111100000000000000000000) >> ((6 - 1)/*(7 - 2)*//*(8 - 3)*/ * 4);

		return rot;
	}

	// 左右反転
	NonResponsePatternVal hmirror() const {
		// [1][2][3]
		// [4][ ][5]
		// [6][7][8]
		// から
		// [3][2][1]
		// [5][ ][4]
		// [8][7][6]

		NonResponsePatternVal rot = 0;

		// 1 → 3
		// 6 → 8
		rot.val32 |= (val32 & 0b00000000111100000000000000001111) << ((3 - 1)/*(8 - 6)*/ * 4);
		// 4 → 5
		rot.val32 |= (val32 & 0b00000000000011110000000000000000) << ((5 - 4) * 4);
		// 3 → 1
		// 8 → 6
		rot.val32 |= (val32 & 0b11110000000000000000111100000000) >> ((3 - 1)/*(8 - 6)*/ * 4);
		// 5 → 4
		rot.val32 |= (val32 & 0b00000000000011110000000000000000) >> ((5 - 4) * 4);

		return rot;
	}
};

extern ResponsePatternVal response_pattern(const Board& board, const XY xy, Color color);
extern NonResponsePatternVal nonresponse_pattern(const Board& board, const XY xy, Color color);

inline bool is_neighbour(const Board& board, XY xy)
{
	XY dx = get_x(xy) - get_x(board.pre_xy);
	XY dy = get_y(xy) - get_y(board.pre_xy);

	return abs(dx) <= 1 && abs(dy) <= 1;
}

// rollout policyの重み
struct RolloutPolicyWeight
{
	// アタリを防ぐ手の重み
	float save_atari_weight;

	// 直前の手と隣接
	float neighbour_weight;

	// レスポンスマッチの重み
	float response_match_weight;

	// レスポンスパターンの重み
	std::map<ResponsePatternVal, float> response_pattern_weight;

	// ノンレスポンスパターンの重み
	std::map<NonResponsePatternVal, float> nonresponse_pattern_weight;
};

// tree policyの重み
struct TreePolicyWeight : public RolloutPolicyWeight
{
	// アタリになる手
	float self_atari_weight;

	// 2手前からの距離
	float pre_distance_weight[2][17];

	// ノンレスポンスパターン(12-point diamond)
	std::map<Diamond12PatternVal, float> diamond12_pattern_weight;
};
