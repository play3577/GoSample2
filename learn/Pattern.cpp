#include <math.h>
#include "../Board.h"
#include "Pattern.h"

inline int get_liberty_val(const int liberty_num)
{
	return (liberty_num >= 3) ? 3 : liberty_num;
}

// ���X�|���X�p�^�[���l�擾(��]�A�Ώ̌`�̍ŏ��l)
ResponsePatternVal get_response_pattern_key_min(const ResponsePatternVal& val)
{
	ResponsePatternVal min = val;

	// 90�x��]
	ResponsePatternVal rot = val.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// 180�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// 270�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// �㉺���]
	rot = val.vmirror();
	if (rot < min)
	{
		min = rot;
	}

	// 90�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// ���E���]
	rot = val.hmirror();
	if (rot < min)
	{
		min = rot;
	}

	// 90�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	return min;
}

// �m�����X�|���X�p�^�[���l�擾(��]�A�Ώ̌`�̍ŏ��l)
NonResponsePatternVal get_nonresponse_pattern_key_min(const NonResponsePatternVal& val)
{
	if (val == 0)
	{
		return 0;
	}

	NonResponsePatternVal min = val;

	// 90�x��]
	NonResponsePatternVal rot = val.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// 180�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// 270�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// �㉺���]
	rot = val.vmirror();
	if (rot < min)
	{
		min = rot;
	}

	// 90�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	// ���E���]
	rot = val.hmirror();
	if (rot < min)
	{
		min = rot;
	}

	// 90�x��]
	rot = rot.rotate();
	if (rot < min)
	{
		min = rot;
	}

	return min;
}

ResponsePatternVal response_pattern(const Board& board, const XY xy, Color color)
{
	if (board.pre_xy == PASS)
	{
		return 0;
	}

	// ���O�̎��12�|�C���g�͈͓���
	XY d = xy - board.pre_xy;
	XY dx = get_x(d);
	XY dy = get_y(d);
	if (abs(dx) + abs(dy) > 2)
	{
		return 0;
	}

	// ������ɂ���
	PatternVal64 color_mask = (color == BLACK) ? 0 : 0b11;

	ResponsePatternVal val = { 0 };

	// 1�i��
	XY xyp = board.pre_xy - BOARD_WIDTH * 2;
	if (xyp > BOARD_WIDTH && !board.is_empty(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2));
	}

	// 2�i��
	xyp = board.pre_xy - BOARD_WIDTH - 1;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 1);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 2);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 3);
	}

	// 3�i��
	xyp = board.pre_xy - 2;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp) && !board.is_offboard(xyp + 1))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 4);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 5);
	}
	xyp += 2;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 6);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp) && !board.is_offboard(xyp - 1))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 7);
	}

	// 4�i��
	xyp = board.pre_xy + BOARD_WIDTH - 1;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 8);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 9);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 10);
	}

	// 5�i��
	xyp = board.pre_xy + BOARD_WIDTH * 2;
	if (xyp < BOARD_MAX - BOARD_WIDTH && !board.is_empty(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val64 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 11);
	}

	val.vals.move_pos = (dy + 2) * 5 + (dx + 2);
	return get_response_pattern_key_min(val);
}

NonResponsePatternVal nonresponse_pattern(const Board& board, const XY xy, Color color)
{
	// ������ɂ���
	PatternVal32 color_mask = (color == BLACK) ? 0 : 0b11;

	NonResponsePatternVal val = { 0 };

	// 1�i��
	XY xyp = xy - BOARD_WIDTH - 1;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2));
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 1);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 2);
	}

	// 2�i��
	xyp = xy - 1;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 3);
	}
	xyp += 2;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 4);
	}

	// 3�i��
	xyp = xy + BOARD_WIDTH - 1;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 5);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 6);
	}
	xyp++;
	if (!board.is_empty(xyp) && !board.is_offboard(xyp))
	{
		const Group& group = board.get_group(xyp);
		val.val32 |= ((group.color ^ color_mask) | (get_liberty_val(group.liberty_num) << 2)) << (4 * 7);
	}

	return get_nonresponse_pattern_key_min(val);
}