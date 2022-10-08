#include "hash.hpp"
#include "state_matrix.hpp"

using word_t = ap_uint<64>;
using state_t = StateMatrix<5, 5, 64>;

const word_t rc[24] = {
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
    0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
    0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
    0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
    0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

const int r[5][5] = {
    {0, 36, 3, 41, 18},
    {1, 44, 10, 45, 2},
    {62, 6, 43, 15, 61},
    {28, 55, 25, 21, 56},
    {27, 20, 39, 8, 14}
};

static ap_uint<1600> pad_512(ap_uint<512> msg) {
#pragma HLS INLINE
    ap_uint<1600> tmp;
    tmp(1599, 1088) = msg;
    tmp(1087, 1024) = 0x0100000000000080;
    tmp(1023, 0) = 0x0;
    return tmp;
}

static state_t to_state(ap_uint<1600> padded_msg) {
#pragma HLS INLINE
    state_t state;
    state[0][0] = reverse_bytes_64(padded_msg(1599, 1536));
    state[1][0] = reverse_bytes_64(padded_msg(1535, 1472));
    state[2][0] = reverse_bytes_64(padded_msg(1471, 1408));
    state[3][0] = reverse_bytes_64(padded_msg(1407, 1344));
    state[4][0] = reverse_bytes_64(padded_msg(1343, 1280));
    state[0][1] = reverse_bytes_64(padded_msg(1279, 1216));
    state[1][1] = reverse_bytes_64(padded_msg(1215, 1152));
    state[2][1] = reverse_bytes_64(padded_msg(1151, 1088));
    state[3][1] = reverse_bytes_64(padded_msg(1087, 1024));
    state[4][1] = reverse_bytes_64(padded_msg(1023, 960));
    state[0][2] = reverse_bytes_64(padded_msg(959, 896));
    state[1][2] = reverse_bytes_64(padded_msg(895, 832));
    state[2][2] = reverse_bytes_64(padded_msg(831, 768));
    state[3][2] = reverse_bytes_64(padded_msg(767, 704));
    state[4][2] = reverse_bytes_64(padded_msg(703, 640));
    state[0][3] = reverse_bytes_64(padded_msg(639, 576));
    state[1][3] = reverse_bytes_64(padded_msg(575, 512));
    state[2][3] = reverse_bytes_64(padded_msg(511, 448));
    state[3][3] = reverse_bytes_64(padded_msg(447, 384));
    state[4][3] = reverse_bytes_64(padded_msg(383, 320));
    state[0][4] = reverse_bytes_64(padded_msg(319, 256));
    state[1][4] = reverse_bytes_64(padded_msg(255, 192));
    state[2][4] = reverse_bytes_64(padded_msg(191, 128));
    state[3][4] = reverse_bytes_64(padded_msg(127, 64));
    state[4][4] = reverse_bytes_64(padded_msg(63, 0));
    return state;
}

static ap_uint<512> to_hash(state_t state) {
#pragma HLS INLINE
    ap_uint<512> tmp;
    tmp(511, 448) = reverse_bytes_64(state[0][0]);
    tmp(447, 384) = reverse_bytes_64(state[1][0]);
    tmp(383, 320) = reverse_bytes_64(state[2][0]);
    tmp(319, 256) = reverse_bytes_64(state[3][0]);
    tmp(255, 192) = reverse_bytes_64(state[4][0]);
    tmp(191, 128) = reverse_bytes_64(state[0][1]);
    tmp(127, 64) = reverse_bytes_64(state[1][1]);
    tmp(63, 0) = reverse_bytes_64(state[2][1]);
    return tmp;
}

static state_t keccak1600_round(state_t state, int round) {
#pragma HLS FUNCTION_INSTANTIATE variable=round
#pragma HLS PIPELINE II=1
    word_t c[5];
    word_t d[5];
    word_t b[5][5];
    for (int x = 0; x < 5; x++) {
        c[x] = state[x][0] ^ state[x][1] ^ state[x][2] ^ state[x][3] ^ state[x][4];
    }
    d[0] = c[4] ^ rotl(c[1], 1);
    d[1] = c[0] ^ rotl(c[2], 1);
    d[2] = c[1] ^ rotl(c[3], 1);
    d[3] = c[2] ^ rotl(c[4], 1);
    d[4] = c[3] ^ rotl(c[0], 1);
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            b[y][(2 * x + 3 * y) % 5] = rotl(state[x][y] ^ d[x], r[x][y]);
        }
    }
    state_t a2;
    for (int x = 0; x < 5; x++) {
            for (int y = 0; y < 5; y++) {
                a2[x][y] = b[x][y] ^ ~b[(x + 1) % 5][y] & b[(x + 2) % 5][y];
            }
    }
    a2[0][0] ^= rc[round];
    return a2;
}

static ap_uint<512> keccak1600(state_t state) {
#pragma HLS PIPELINE II=1
    state_t tmp = state;
    for (int i = 0; i < 24; i++) {
        tmp = keccak1600_round(tmp, i);
    }
    return to_hash(tmp);
}

void keccak512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = keccak1600(to_state(pad_512(in)));
}
