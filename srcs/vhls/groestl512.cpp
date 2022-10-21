#include "hash.hpp"
#include "state_matrix.hpp"

using word_t = ap_uint<8>;
using state_t = StateMatrix<8, 16, 8>;

static ap_uint<1024> pad_512(ap_uint<512> msg) {
#pragma HLS INLINE
    ap_uint<1024> tmp;
    tmp(1023, 512) = msg;
    tmp[511] = 0x1;
    tmp(510, 1) = 0x0;
    tmp[0] = 0x1;
    return tmp;
}

static state_t to_state(ap_uint<1024> padded_msg) {
#pragma HLS INLINE
    return StateMatrix<8, 16, 8>(padded_msg, COL_ORDER);
}

static ap_uint<512> to_hash(state_t state) {
#pragma HLS INLINE
    ap_uint<64> tmp8 = (state[0][8], state[1][8], state[2][8], state[3][8], state[4][8], state[5][8], state[6][8], state[7][8]);
    ap_uint<64> tmp9 = (state[0][9], state[1][9], state[2][9], state[3][9], state[4][9], state[5][9], state[6][9], state[7][9]);
    ap_uint<64> tmp10 = (state[0][10], state[1][10], state[2][10], state[3][10], state[4][10], state[5][10], state[6][10], state[7][10]);
    ap_uint<64> tmp11 = (state[0][11], state[1][11], state[2][11], state[3][11], state[4][11], state[5][11], state[6][11], state[7][11]);
    ap_uint<64> tmp12 = (state[0][12], state[1][12], state[2][12], state[3][12], state[4][12], state[5][12], state[6][12], state[7][12]);
    ap_uint<64> tmp13 = (state[0][13], state[1][13], state[2][13], state[3][13], state[4][13], state[5][13], state[6][13], state[7][13]);
    ap_uint<64> tmp14 = (state[0][14], state[1][14], state[2][14], state[3][14], state[4][14], state[5][14], state[6][14], state[7][14]);
    ap_uint<64> tmp15 = (state[0][15], state[1][15], state[2][15], state[3][15], state[4][15], state[5][15], state[6][15], state[7][15]);
    return (tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
}

static word_t sbox(word_t x) {
#pragma HLS INLINE
    const word_t rom[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };
#pragma HLS BIND_STORAGE variable=rom type=rom_np impl=lutram
    return rom[x];
}

static word_t mul2(word_t x) {
#pragma HLS INLINE
    word_t tmp;
    tmp[0] = x[7];
    tmp[1] = x[7] ^ x[0];
    tmp[2] = x[1];
    tmp[3] = x[7] ^ x[2];
    tmp[4] = x[7] ^ x[3];
    tmp[5] = x[4];
    tmp[6] = x[5];
    tmp[7] = x[6];
    return tmp;
}

static state_t mix_bytes(state_t state) {
#pragma HLS INLINE
    state_t ret;
    for (int i = 0; i < 16; i++) {
        hls::vector<word_t, 8> b = state.get_col(i);
        hls::vector<word_t, 8> b2;
        hls::vector<word_t, 8> b4;
        for (int j = 0; j < 8; j++) {
            b2[j] = mul2(b[j]);
            b4[j] = mul2(b2[j]);
        }
        word_t t1 = b2[0] ^ b2[2] ^ b[5] ^ b4[7] ^ b[7];
        word_t t2 = b2[1] ^ b[4] ^ b4[6] ^ b[6] ^ b2[7];
        word_t t3 = b[0] ^ b4[2] ^ b[2] ^ b2[3] ^ b2[5];
        word_t t4 = b[1] ^ b4[3] ^ b[3] ^ b2[4] ^ b2[6];
        word_t t5 = b4[0] ^ b[0] ^ b2[3] ^ b4[5];
        word_t t6 = b4[1] ^ b4[4] ^ b[4] ^ b2[7];
        word_t t7 = b4[1] ^ b[1] ^ b2[4];
        word_t t8 = b2[0] ^ b4[5] ^ b[5];
        word_t t9 = b[2] ^ b2[5];
        word_t ta = b2[1] ^ b[6];
        word_t tb = b[3] ^ b2[6];
        word_t tc = b2[2] ^ b[7];

        ret[0][i] = t1 ^ t2 ^ t9 ^ b4[3] ^ b4[4];
        ret[1][i] = t1 ^ t5 ^ ta ^ tb ^ b4[4];
        ret[2][i] = t2 ^ t5 ^ t7 ^ tc;
        ret[3][i] = t1 ^ t3 ^ t7 ^ b4[6];
        ret[4][i] = t3 ^ t4 ^ ta ^ b4[0] ^ b4[7];
        ret[5][i] = t4 ^ t6 ^ t9 ^ tc ^ b4[0];
        ret[6][i] = t3 ^ t6 ^ t8 ^ tb;
        ret[7][i] = t2 ^ t4 ^ t8 ^ b4[2];
    }
    return ret;
}

static state_t add_round_constant_p(state_t state, word_t round) {
#pragma HLS INLINE
    state_t ret = state;
    for (int i = 0; i < 16; i++) {
        ret[0][i] = state[0][i] ^ (word_t) (i * 16) ^ round;
    }
    return ret;
}

static state_t add_round_constant_q(state_t state, word_t round) {
#pragma HLS INLINE
    state_t ret = state;
    for (int j = 0; j < 7; j++) {
        for (int i = 0; i < 16; i++) {
            ret[j][i] = state[j][i] ^ 0xff;
        }
    }
    for (int i = 0; i < 16; i++) {
        ret[7][i] = state[7][i] ^ (word_t) (255 - 16 * i) ^ round;
    }
    return ret;
}

static state_t sub_bytes(state_t state) {
#pragma HLS INLINE
    state_t ret;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 16; j++) {
            ret[i][j] = sbox(state[i][j]);
        }
    }
    return ret;
}

static state_t shift_bytes_p(state_t state) {
#pragma HLS INLINE
    state_t ret;
    int rc[8] = {0, 1, 2, 3, 4, 5, 6, 11};
    ret = state.rotl_rows(rc);
    return ret;
}

static state_t shift_bytes_q(state_t state) {
#pragma HLS INLINE
    state_t ret;
    int rc[8] = {1, 3, 5, 11, 0, 2, 4, 6};
    ret = state.rotl_rows(rc);
    return ret;
}

static state_t inner_round_p(state_t state, int round) {
#pragma HLS INLINE off
    state_t ret = add_round_constant_p(state, round);
    ret = sub_bytes(ret);
    ret = shift_bytes_p(ret);
    ret = mix_bytes(ret);
    return ret;
}

static state_t inner_round_q(state_t state, int round) {
#pragma HLS INLINE off
    state_t ret = add_round_constant_q(state, round);
    ret = sub_bytes(ret);
    ret = shift_bytes_q(ret);
    ret = mix_bytes(ret);
    return ret;
}

static state_t compress_p(state_t state) {
#pragma HLS INLINE off
    state_t ret = state;
    for (int i = 0; i < 14; i++) {
        ret = inner_round_p(ret, i);
    }
    return ret;
}

static state_t compress_q(state_t state) {
#pragma HLS INLINE off
    state_t ret = state;
    for (int i = 0; i < 14; i++) {
        ret = inner_round_q(ret, i);
    }
    return ret;
}

static ap_uint<512> groestl(state_t state) {
#pragma HLS INLINE off
    StateMatrix<8, 16, 8> iv = StateMatrix<8, 16, 8>();
    iv[6][15] = 0x02;

    state_t mid = compress_p(state ^ iv) ^ compress_q(state) ^ iv;
    return to_hash(compress_p(mid) ^ mid);
}

void groestl512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = groestl(to_state(pad_512(in)));
}
