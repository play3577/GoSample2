#include <stdio.h>
#include <time.h>
#include <regex>
#include "../Debug.h"
#include "../Board.h"
#include "../learn/Sgf.h"
#include "../learn/learn.h"
#include "../UCTSample.h"
#include "../UCTParallel.h"
#include "../UCTSaveAtari.h"
#include "../UCTPattern.h";
#include "../FixedIndexList.h"

using namespace std;

UCTPattern player;

void init_board(Board& board, Color* test_board, const int boardsize)
{
	for (int i = 0; i < boardsize * boardsize; i++)
	{
		XY xy = get_xy(i % boardsize + 1, i / boardsize + 1);
		if (test_board[i] != EMPTY)
		{
			board.move(xy, test_board[i], false);
		}
	}
}

// SGFを読み込んでボードを初期化する
void load_sgf(Board& board, char* sgf_text)
{
	// ;で区切る
	char* next = strtok(sgf_text, ";");

	// 1つ目は読み飛ばす
	next = strtok(NULL, ";");

	// サイズ取得
	regex re("SZ\\[\\(d+)\\]");
	cmatch ma;
	if (regex_match(next, ma, re))
	{
		int size = atoi(ma.str(1).c_str());
		board.init(size);
	}
	else
	{
		fprintf(stderr, "sgf error : size\n");
		return;
	}

	while ((next = strtok(NULL, ";")) != NULL)
	{
		Color color = get_color_from_sgf(next);
		if (color == 0) {
			fprintf(stderr, "sgf error : color\n");
			return;
		}

		XY xy = get_xy_from_sgf(next);

		board.move(xy, color, false);
	}
}

template <typename T>
void assert(const T& val, const T& expected)
{
	if (val == expected)
	{
		printf("success\n");
	}
	else {
		printf("fail\n");
	}
}

template <typename T>
void assert_not(const T& val, const T& expected)
{
	if (val != expected)
	{
		printf("success\n");
	}
	else {
		printf("fail\n");
	}
}

void print_child(UCTNode* root)
{
	for (int i = 0; i < root->child_num; i++)
	{
		UCTNode* child = root->child + i;
		printf("xy = %d, x,y = %d,%d : win = %d : playout_num = %d : rate = %.2f\n", child->xy, get_x(child->xy), get_y(child->xy), child->win_num, child->playout_num, float(child->win_num) / child->playout_num);
	}
}

void test_001()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		2, 2, 2, 2, 2, 1, 0, 1, 1, // 1
		2, 2, 2, 2, 2, 1, 1, 1, 1, // 2 
		2, 2, 2, 2, 2, 1, 1, 0, 1, // 3
		2, 2, 2, 2, 2, 1, 1, 1, 1, // 4
		2, 2, 2, 2, 2, 1, 1, 2, 2, // 5
		1, 1, 1, 1, 2, 1, 1, 2, 2, // 6
		1, 1, 0, 0, 0, 0, 0, 2, 2, // 7
		2, 2, 2, 2, 2, 2, 2, 2, 2, // 8
		2, 2, 2, 2, 2, 2, 2, 2, 2  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(5, 7));
}

void test_002()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		2, 2, 2, 2, 2, 1, 0, 1, 1, // 1
		2, 2, 2, 2, 2, 1, 1, 1, 1, // 2 
		2, 2, 2, 2, 2, 1, 1, 0, 1, // 3
		2, 2, 2, 2, 2, 1, 1, 1, 1, // 4
		2, 2, 2, 2, 2, 1, 1, 1, 1, // 5
		1, 1, 1, 1, 2, 1, 1, 2, 2, // 6
		1, 1, 0, 0, 0, 0, 0, 2, 0, // 7
		2, 2, 2, 2, 2, 2, 2, 2, 2, // 8
		2, 2, 2, 2, 2, 2, 2, 2, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, WHITE);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(5, 7));
}

void test_003()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 2, 1, 1, 0, 1, 1, 1, 1, // 1
		2, 2, 2, 1, 1, 1, 1, 1, 1, // 2 
		2, 0, 2, 2, 2, 1, 1, 2, 1, // 3
		2, 2, 2, 1, 1, 1, 1, 2, 2, // 4
		0, 2, 1, 1, 0, 2, 1, 2, 0, // 5
		2, 0, 2, 1, 1, 2, 1, 2, 2, // 6
		0, 2, 0, 2, 2, 1, 1, 1, 0, // 7
		2, 2, 2, 2, 0, 2, 0, 1, 1, // 8
		0, 2, 2, 0, 2, 0, 2, 2, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, PASS);
}

void test_004()
{
	// アタリから逃げるケース

	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 2 
		0, 0, 0, 0, 0, 1, 1, 2, 0, // 3
		0, 0, 0, 0, 2, 1, 2, 0, 0, // 4
		0, 0, 0, 0, 2, 1, 1, 2, 2, // 5
		0, 0, 0, 0, 1, 2, 2, 1, 1, // 6
		0, 0, 0, 0, 0, 2, 1, 2, 0, // 7
		0, 0, 0, 0, 0, 0, 1, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	// 9,7はNG
	assert_not(xy, get_xy(9, 7));
}

void test_005()
{
	// 負けなのに勝ちと判定

	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		1, 1, 2, 2, 0, 1, 2, 2, 0, // 1
		1, 1, 2, 2, 2, 2, 0, 2, 2, // 2 
		1, 1, 1, 2, 0, 0, 0, 2, 1, // 3
		0, 1, 2, 2, 2, 0, 2, 2, 0, // 4
		1, 1, 1, 1, 2, 2, 1, 2, 0, // 5
		0, 0, 0, 1, 2, 1, 1, 2, 2, // 6
		0, 1, 1, 0, 1, 0, 1, 1, 2, // 7
		0, 1, 1, 0, 1, 0, 1, 2, 2, // 8
		0, 1, 1, 1, 1, 1, 1, 1, 2  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, PASS);
}

void test_ladder_search_001()
{
	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 2 
		0, 0, 0, 0, 2, 1, 2, 0, 0, // 3
		0, 0, 0, 0, 0, 2, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	Color tmp_board[BOARD_BYTE_MAX] = { 0 };
	XY liberties[4] = { get_xy(6, 1), get_xy(7, 2), 0, 0 };

	bool is_ladder = Board::ladder_search(board, BLACK, get_xy(6, 2), tmp_board, liberties, 5);

	assert(is_ladder, true);
}

void test_ladder_search_002()
{
	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 1, 0, 0, // 1
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 2 
		0, 0, 0, 0, 2, 1, 2, 0, 0, // 3
		0, 0, 0, 0, 0, 2, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	Color tmp_board[BOARD_BYTE_MAX] = { 0 };
	XY liberties[4] = { get_xy(6, 1), get_xy(7, 2), 0, 0 };

	bool is_ladder = Board::ladder_search(board, BLACK, get_xy(6, 2), tmp_board, liberties, 5);

	assert(is_ladder, true);
}

void test_ladder_search_003()
{
	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 1, 1, 0, // 1
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 2 
		0, 0, 0, 2, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 2, 1, 2, 0, 0, 0, // 4
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	Color tmp_board[BOARD_BYTE_MAX] = { 0 };
	XY liberties[4] = { get_xy(5, 2), get_xy(6, 3), 0, 0 };

	bool is_ladder = Board::ladder_search(board, BLACK, get_xy(5, 3), tmp_board, liberties, 5);

	assert(is_ladder, false);
}

void test_ladder_search_004()
{
	// 6,2に白を置いた後に例外が発生した

	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 1, 0, 0, 0, 0, // 1
		0, 0, 0, 0, 0, 2, 0, 0, 0, // 2 
		0, 0, 0, 0, 0, 1, 2, 0, 0, // 3
		0, 1, 1, 0, 2, 1, 1, 2, 0, // 4
		0, 0, 0, 0, 1, 2, 2, 0, 0, // 5
		0, 0, 0, 0, 1, 0, 2, 0, 0, // 6
		0, 0, 0, 2, 1, 2, 1, 1, 2, // 7
		0, 0, 0, 0, 2, 1, 2, 2, 0, // 8
		0, 0, 0, 0, 0, 1, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(5, 3));
}

void test_ladder_search_005()
{
	// 7,7に白を置いた後に例外が発生した

	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 1, 0, 0, 0, 0, 0, 0, 0, // 2 
		0, 0, 0, 0, 1, 0, 1, 0, 0, // 3
		0, 0, 0, 2, 1, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 2, 1, 2, 0, 0, // 6
		0, 0, 0, 2, 2, 1, 2, 0, 0, // 7
		0, 0, 0, 0, 0, 2, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 1, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(6, 5));
}

void test_ladder_search_006()
{
	// 5,4が正解だが、7,6に置かれた

	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 1, 0, 0, 0, // 1
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 2 
		0, 0, 0, 2, 0, 0, 0, 0, 0, // 3
		0, 0, 2, 1, 0, 1, 0, 0, 0, // 4
		0, 2, 1, 1, 2, 0, 0, 0, 0, // 5
		0, 2, 1, 2, 0, 0, 0, 0, 0, // 6
		0, 0, 2, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 1, 0, 0, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(5, 4));
}

void test_ladder_search_007()
{
	// 6,9が正解だが、3,9に置かれた

	// シチョウ判定
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 1, 0, 0, 0, // 1
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 2 
		0, 0, 2, 1, 1, 2, 0, 0, 1, // 3
		0, 0, 0, 2, 1, 1, 2, 0, 0, // 4
		0, 0, 0, 0, 2, 1, 1, 2, 0, // 5
		0, 0, 0, 0, 2, 2, 1, 1, 2, // 6
		0, 0, 0, 2, 1, 1, 1, 1, 2, // 7
		0, 0, 1, 2, 1, 2, 1, 2, 0, // 8
		0, 0, 0, 1, 2, 0, 2, 0, 0  // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(6, 9));
}

void test_is_self_atari_001()
{
	// アタリになる手
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		1, 0, 0, 0, 0, 0, 0, 0, 0, // 2
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	bool res = board.is_self_atari(WHITE, get_xy(1, 1));

	assert(res, true);
}

void test_is_self_atari_002()
{
	// アタリにならない手
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		1, 1, 0, 0, 0, 0, 0, 0, 0, // 1
		1, 0, 0, 0, 0, 0, 0, 0, 0, // 2
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	bool res = board.is_self_atari(WHITE, get_xy(2, 2));

	assert(res, false);
}

void test_is_self_atari_003()
{
	// アタリにならない手(取ることができる)
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		1, 1, 2, 2, 0, 0, 0, 0, 0, // 1
		1, 0, 1, 1, 2, 0, 0, 0, 0, // 2
		1, 0, 2, 2, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	bool res = board.is_self_atari(WHITE, get_xy(2, 2));

	assert(res, false);
}

void test_is_self_atari_004()
{
	// アタリにならない手(コウになる)
	Color test_board[] = {
		//  1  2  3  4  5  6  7  8  9
		1, 1, 2, 2, 0, 0, 0, 0, 0, // 1
		1, 0, 1, 2, 0, 0, 0, 0, 0, // 2
		1, 1, 2, 2, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	bool res = board.is_self_atari(WHITE, get_xy(2, 2));

	assert(res, false);
}

void test_is_self_atari_005()
{
	// アタリになる手(コウにならない)
	Color test_board[] = {
		//  1  2  3  4  5  6  7  8  9
		1, 1, 2, 2, 0, 0, 0, 0, 0, // 1
		1, 0, 1, 1, 2, 0, 0, 0, 0, // 2
		1, 1, 2, 2, 0, 0, 0, 0, 0, // 3
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	bool res = board.is_self_atari(WHITE, get_xy(2, 2));

	assert(res, true);
}

void test_pattern_001()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 1, 1, 1, 1, 0, 0, 0, 0, // 2
		1, 1, 0, 1, 2, 0, 0, 0, 0, // 3
		0, 1, 1, 1, 1, 0, 0, 0, 0, // 4
		0, 0, 2, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);

	board.move(get_xy(3, 3), BLACK, false);

	debug_print_board(board);

	ResponsePatternVal val = response_pattern_min(board, get_xy(3, 1), WHITE);

	print_response_pattern(val);
}

void test_pattern_002()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 1, 1, 2, 1, 0, 0, 0, 0, // 2
		1, 1, 0, 1, 2, 0, 0, 0, 0, // 3
		0, 1, 2, 1, 1, 0, 0, 0, 0, // 4
		0, 0, 2, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	NonResponsePatternVal val = nonresponse_pattern_min(board, get_xy(3, 3), WHITE);

	print_nonresponse_pattern(val);
}

void test_pattern_003()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 1, 1, 1, 1, 0, 0, 0, 0, // 2
		1, 1, 0, 1, 2, 0, 0, 0, 0, // 3
		0, 1, 1, 1, 1, 0, 0, 0, 0, // 4
		0, 0, 2, 0, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	Diamond12PatternVal val = diamond12_pattern_min(board, get_xy(3, 3), WHITE);

	print_diamond12_pattern(val);
}

void test_legal_001()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 2, 2, 2, 2, 0, 0, 0, 0, // 1
		2, 1, 1, 1, 1, 2, 0, 0, 0, // 2
		1, 1, 0, 1, 2, 0, 0, 0, 0, // 3
		2, 1, 1, 1, 2, 0, 0, 0, 0, // 4
		0, 2, 2, 2, 0, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);
	debug_print_board(board);

	MoveResult ret = board.is_legal(get_xy(3, 3), WHITE, false);

	assert(ret, SUCCESS);
}

void test_save_atari_001()
{
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
		0, 0, 0, 1, 0, 0, 0, 0, 0, // 3
		0, 0, 1, 2, 0, 0, 0, 0, 0, // 4
		0, 0, 0, 0, 2, 0, 0, 0, 0, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
		0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);

	board.move(get_xy(5, 4), BLACK, false);

	debug_print_board(board);

	XY xy = player.select_move(board, WHITE);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(4, 5));
}

void test_uctpattern_001()
{
	// 白(5,5)の後に(1,3)を打った
	Color test_board[] = {
	//  1  2  3  4  5  6  7  8  9
		0, 0, 0, 2, 2, 2, 2, 0, 2, // 1
		0, 1, 2, 2, 2, 1, 1, 2, 0, // 2
		0, 2, 1, 2, 1, 1, 0, 1, 0, // 3
		0, 1, 1, 2, 1, 0, 0, 0, 0, // 4
		0, 2, 2, 1, 0, 0, 1, 0, 0, // 5
		2, 0, 2, 1, 0, 0, 0, 0, 0, // 6
		0, 2, 1, 0, 1, 0, 0, 1, 0, // 7
		0, 2, 1, 1, 0, 1, 0, 0, 0, // 8
		0, 2, 2, 2, 1, 0, 0, 0, 0, // 9
	};
	Board board(9);
	init_board(board, test_board, 9);

	board.move(get_xy(5, 5), WHITE, false);

	debug_print_board(board);

	XY xy = player.select_move(board, BLACK);

	print_child(player.root);
	printf("xy = %d, x,y = %d,%d\n", xy, get_x(xy), get_y(xy));

	assert(xy, get_xy(6, 5));
}

void print_fixedindexlist(FixedIndexList<XY, BOARD_BYTE_MAX>& empty_list)
{
	for (XY xy = empty_list.begin(); xy != empty_list.end(); xy = empty_list.next(xy))
	{
		printf("%d ", xy);
	}
	printf("\n");
}

void test_fixedindexlist()
{
	Board board(9);

	FixedIndexList<XY, BOARD_BYTE_MAX> empty_list;

	for (XY y = BOARD_WIDTH; y < BOARD_MAX - BOARD_WIDTH; y += BOARD_WIDTH)
	{
		for (XY x = 1; x <= BOARD_SIZE; x++)
		{
			const XY xy = y + x;
			if (board.is_empty(xy))
			{
				empty_list.add(xy);
			}
		}
	}

	print_fixedindexlist(empty_list);

	// remove
	empty_list.remove(11);
	empty_list.remove(35);
	empty_list.remove(45);
	empty_list.remove(55);
	empty_list.remove(99);

	printf("remove\n");
	print_fixedindexlist(empty_list);

	// add
	empty_list.add(11);
	empty_list.add(35);
	empty_list.add(99);

	printf("add\n");
	print_fixedindexlist(empty_list);
}

int main()
{
	load_weight(L"../learn");

	test_001();
	test_002();
	test_003();
	test_004();
	test_005();
	//test_ladder_search_001();
	//test_ladder_search_002();
	//test_ladder_search_003();
	//test_ladder_search_004();
	//test_ladder_search_005();
	//test_ladder_search_006();
	//test_ladder_search_007();
	//test_is_self_atari_001();
	//test_is_self_atari_002();
	//test_is_self_atari_003();
	//test_is_self_atari_004();
	//test_is_self_atari_005();
	//test_pattern_001();
	//test_pattern_002();
	//test_pattern_003();
	//test_legal_001();
	//test_save_atari_001();
	//test_uctpattern_001();
	//test_fixedindexlist();

	printf("Press any key.\n");
	getchar();

	return 0;
}