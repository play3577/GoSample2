#include "Board.h"

int BOARD_SIZE = 9;
int BOARD_WIDTH = BOARD_SIZE + 1;
int BOARD_STONE_MAX = BOARD_SIZE * BOARD_SIZE;
int BOARD_MAX = BOARD_WIDTH * (BOARD_SIZE + 2);
double KOMI = 6.5;
XY DIR4[4];

MoveResult Board::move(const XY xy, const Color color, const bool fill_eye_err)
{
	// パスの場合
	if (xy == PASS) {
		ko = -1;
		return SUCCESS;
	}

	int tmp_ko;

	// 周囲4方向の連
	int around_group_self[4];
	int around_group_self_num = 0;
	int around_group_capture[4];
	int around_group_capture_num = 0;
	int around_group_oponnent[4];
	int around_group_oponnent_num = 0;

	int capture_num = 0;

	// 呼吸点が一致する連を取得
	for (int i = 0, hit_num = 0; hit_num < group_num; i++)
	{
		if (bit_test(&group_unusedflg[i / BIT], i % BIT) == 0)
		{
			hit_num++;
			if (group[i].hit_liberties(xy))
			{
				if (group[i].color == color)
				{
					around_group_self[around_group_self_num++] = i;
				}
				else {
					// 取ることができるか
					if (group[i].liberty_num == 1)
					{
						around_group_capture[around_group_capture_num++] = i;
						capture_num += group[i].stone_num;
						if (group[i].stone_num == 1)
						{
							tmp_ko = group[i].stone[0];
						}
					}
					else {
						around_group_oponnent[around_group_oponnent_num++] = i;
					}
				}
			}
		}
	}

	int offboard_num = 0;
	int empty_num = 0;
	int alive_num = 0;
	// 周囲4方向の空白
	XY around_liberty[4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		XY xyd = xy + DIR4[i];

		if (board[xyd] == G_OFFBOARD)
		{
			offboard_num++;
			continue;
		}

		if (board[xyd] == G_NONE)
		{
			empty_num++;
			around_liberty[i] = 1;
			continue;
		}

		// 隣接する自分の色の連の呼吸点がなくならないか
		if (group[board[xyd]].color == color && group[board[xyd]].liberty_num >= 2)
		{
			alive_num++;
		}
	}

	// 自殺手
	if (capture_num == 0 && empty_num == 0 && alive_num == 0)
	{
		return ILLIGAL;
	}
	// コウ
	if (xy == ko)
	{
		return KO;
	}
	// 眼
	if (offboard_num + alive_num == 4 && fill_eye_err)
	{
		return EYE;
	}

	// 石を置く
	if (around_group_self_num == 0)
	{
		// 連を追加
		int idx = add_group(xy, color, around_liberty);

		// 連番号を埋める
		board[xy] = idx;

		// 石の数を加算
		stone_num[color]++;
	}
	else
	{
		// 連に石を追加
		Group& group0 = group[around_group_self[0]];
		group0.add_stone_and_liberties(xy, around_liberty);

		// 連番号を埋める
		board[xy] = around_group_self[0];

		// 石の数を加算
		stone_num[color]++;

		for (int i = 1; i < around_group_self_num; i++)
		{
			Group& groupi = group[around_group_self[i]];

			// 連をつなげる
			group0.chain_group(xy, groupi);

			// 連番号を置き換える
			for (int j = 0; j < groupi.stone_num; j++)
			{
				board[groupi.stone[j]] = around_group_self[0];
			}

			// 連を削除
			remove_group(around_group_self[i]);
		}
	}

	// 石を取る
	for (int i = 0; i < around_group_capture_num; i++)
	{
		// 連のあった位置を空白にして呼吸点を追加する
		Group &remove = group[around_group_capture[i]];
		for (int j = 0; j < remove.stone_num; j++)
		{
			// 連のあった位置を空白にする
			XY xyr = remove.stone[j];
			board[xyr] = G_NONE;

			// 隣接する石に呼吸点を追加
			for (XY d : DIR4)
			{
				XY xyd = xyr + d;
				if (board[xyd] != G_OFFBOARD && board[xyd] != G_NONE)
				{
					if (group[board[xyd]].color == color)
					{
						group[board[xyd]].add_liberty(xyr);
					}
				}
			}
		}

		// 相手の石の数を減算
		stone_num[opponent(color)] -= remove.stone_num;

		// 連を削除
		remove_group(around_group_capture[i]);
	}

	// 呼吸点の削除
	for (int i = 0; i < around_group_oponnent_num; i++)
	{
		group[around_group_oponnent[i]].remove_liberty(xy);
	}

	// コウ
	if (capture_num == 1 && group[board[xy]].stone_num == 1 && group[board[xy]].liberty_num == 1)
	{
		ko = tmp_ko;
	}
	else {
		ko = -1;
	}

	return SUCCESS;
}