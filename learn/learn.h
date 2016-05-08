#pragma once

#include "Pattern.h"

extern void prepare_pattern(const wchar_t* dirs);
extern void learn_pattern(const wchar_t* dirs);
extern void init_hash_table_and_weight(const uint64_t seed);
extern void check_hash();
extern void dump_weight();
extern void print_response_pattern(const ResponsePatternVal& val);
extern void print_nonresponse_pattern(const NonResponsePatternVal& val);
extern void print_diamond12_pattern(const Diamond12PatternVal& val);

const int HASH_KEY_BIT = 24;
const int HASH_KEY_MAX = 1 << HASH_KEY_BIT;
const int HASH_KEY_MASK = HASH_KEY_MAX - 1;

// ハッシュキー衝突検出用
extern ResponsePatternVal response_pattern_collision[HASH_KEY_MAX];
extern NonResponsePatternVal nonresponse_pattern_collision[HASH_KEY_MAX];
