#include "hash.hpp"
#include "state_matrix.hpp"

using byte_t = ap_uint<8>;
using word_t = ap_uint<128>;
using state_t = StateMatrix<4, 4, 128>;
using inner_state_t = StateMatrix<4, 4, 8>;

const ap_uint<128> iv[8] = {
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16),
    ap_uint<128>("0x00020000000000000000000000000000", 16)
};

// -- AES -------------------------------------------------------------------------------------------------------------
static byte_t aes_mul2(byte_t x) {
    byte_t tmp;
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

static byte_t aes_sbox(byte_t x) {
    const byte_t rom[256] = {
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

static inner_state_t aes_sub_bytes(inner_state_t state) {
    inner_state_t res;
    for (int i = 0; i < 4; i++) {
        res[i][0] = aes_sbox(state[i][0]);
        res[i][1] = aes_sbox(state[i][1]);
        res[i][2] = aes_sbox(state[i][2]);
        res[i][3] = aes_sbox(state[i][3]);
    }
    return res;
}

static inner_state_t aes_shift_rows(inner_state_t state) {
    inner_state_t res;
    for (int i = 0; i < 4; i++) {
        res[i][0] = state[i][(0 + i) % 4];
        res[i][1] = state[i][(1 + i) % 4];
        res[i][2] = state[i][(2 + i) % 4];
        res[i][3] = state[i][(3 + i) % 4];
    }
    return res;
}

static inner_state_t aes_mix_cols(inner_state_t state) {
    byte_t innerTmp[4][4];
    byte_t outerTmp[4];
    inner_state_t res;
    for (int j = 0; j < 4; j++) {
        innerTmp[0][j] = state[0][j] ^ state[1][j];
        innerTmp[1][j] = state[1][j] ^ state[2][j];
        innerTmp[2][j] = state[2][j] ^ state[3][j];
        innerTmp[3][j] = state[3][j] ^ state[0][j];
        outerTmp[j] = innerTmp[0][j] ^ innerTmp[2][j];
        res[0][j] = state[0][j] ^ outerTmp[j] ^ aes_mul2(innerTmp[0][j]);
        res[1][j] = state[1][j] ^ outerTmp[j] ^ aes_mul2(innerTmp[1][j]);
        res[2][j] = state[2][j] ^ outerTmp[j] ^ aes_mul2(innerTmp[2][j]);
        res[3][j] = state[3][j] ^ outerTmp[j] ^ aes_mul2(innerTmp[3][j]);
    }
    return res;
}

static inner_state_t aes_add_key(inner_state_t state, ap_uint<128> key) {
    inner_state_t res;
    for (int i = 0; i < 4; i++) {
        res[0][i] = state[0][i] ^ key(127 - (32 * i + 0), 120 - (32 * i + 0));
        res[1][i] = state[1][i] ^ key(127 - (32 * i + 8), 120 - (32 * i + 8));
        res[2][i] = state[2][i] ^ key(127 - (32 * i + 16), 120 - (32 * i + 16));
        res[3][i] = state[3][i] ^ key(127 - (32 * i + 24), 120 - (32 * i + 24));
    }
    return res;
}

static inner_state_t aesenc(inner_state_t state, ap_uint<128> key) {
    inner_state_t tmp;
    tmp = aes_sub_bytes(state);
    tmp = aes_shift_rows(tmp);
    tmp = aes_mix_cols(tmp);
    tmp = aes_add_key(tmp, key);
    return tmp;
}

static inner_state_t zero_key_aesenc(inner_state_t state) {
    inner_state_t tmp;
    tmp = aes_sub_bytes(state);
    tmp = aes_shift_rows(tmp);
    tmp = aes_mix_cols(tmp);
    return tmp;
}
// --------------------------------------------------------------------------------------------------------------------

static ap_uint<1024> pad_512(ap_uint<512> msg) {
    ap_uint<1024> tmp;
    tmp(1023, 512) = msg;
    tmp[511] = 0x1;
    tmp(510, 144) = 0x0;
    tmp(143, 128) = 0x2;
    tmp(127, 0) = ap_uint<128>("0x00020000000000000000000000000000", 16);
    return tmp;
}

static state_t to_state(ap_uint<1024> padded_msg) {
    state_t tmp;
    tmp[0][0] = iv[0];
    tmp[1][0] = iv[1];
    tmp[2][0] = iv[2];
    tmp[3][0] = iv[3];
    tmp[0][1] = iv[4];
    tmp[1][1] = iv[5];
    tmp[2][1] = iv[6];
    tmp[3][1] = iv[7];
    tmp[0][2] = padded_msg(1023, 896);
    tmp[1][2] = padded_msg(895, 768);
    tmp[2][2] = padded_msg(767, 640);
    tmp[3][2] = padded_msg(639, 512);
    tmp[0][3] = padded_msg(511, 384);
    tmp[1][3] = padded_msg(383, 256);
    tmp[2][3] = padded_msg(255, 128);
    tmp[3][3] = padded_msg(127, 0);
    return tmp;
}

static state_t big_sub(state_t state, int round) {
    state_t tmp;
    ap_uint<64> k;
    k = ap_uint<64>(512 + 16 * round);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ap_uint<128> kk = ap_uint<128>(reverse_bytes_64(k)) << 64;
            inner_state_t aes_state = StateMatrix<4, 4, 8>(state[j][i], COL_ORDER);
            tmp[j][i] = zero_key_aesenc(aesenc(aes_state, kk)).to_uint(COL_ORDER);
            k += 1;
        }
    }
    return tmp;
}

static state_t big_shift(state_t state) {
    int rc[4] = {0, 1, 2, 3};
    return state.rotl_rows(rc);
}

static state_t big_mix(state_t state) {
    state_t tmp;
    for (int i = 0; i < 4; i++) {
        word_t a, b, c, d;
        for (int j = 0; j < 16; j++) {
            byte_t col[4];
            col[0] = state[0][i](127 - j * 8, 120 - j * 8);
            col[1] = state[1][i](127 - j * 8, 120 - j * 8);
            col[2] = state[2][i](127 - j * 8, 120 - j * 8);
            col[3] = state[3][i](127 - j * 8, 120 - j * 8);

            byte_t inner[4];
            inner[0] = col[0] ^ col[1];
            inner[1] = col[1] ^ col[2];
            inner[2] = col[2] ^ col[3];
            inner[3] = col[3] ^ col[0];
            byte_t outer = inner[0] ^ inner[2];
            a(127 - j * 8, 120 - j * 8) = col[0] ^ outer ^ aes_mul2(inner[0]);
            b(127 - j * 8, 120 - j * 8) = col[1] ^ outer ^ aes_mul2(inner[1]);
            c(127 - j * 8, 120 - j * 8) = col[2] ^ outer ^ aes_mul2(inner[2]);
            d(127 - j * 8, 120 - j * 8) = col[3] ^ outer ^ aes_mul2(inner[3]);
        }
        tmp[0][i] = a;
        tmp[1][i] = b;
        tmp[2][i] = c;
        tmp[3][i] = d;
    }
    return tmp;
}

static state_t echo_round(state_t state, int round) {
    state_t tmp;
    tmp = big_sub(state, round);
    tmp = big_shift(tmp);
    tmp = big_mix(tmp);
    return tmp;
}

static ap_uint<512> to_hash(state_t initial_state, state_t final_state) {
    hls::vector<ap_uint<128>, 8> v;
    v[0] = iv[0] ^ initial_state[0][2] ^ final_state[0][0] ^ final_state[0][2];
    v[1] = iv[1] ^ initial_state[1][2] ^ final_state[1][0] ^ final_state[1][2];
    v[2] = iv[2] ^ initial_state[2][2] ^ final_state[2][0] ^ final_state[2][2];
    v[3] = iv[3] ^ initial_state[3][2] ^ final_state[3][0] ^ final_state[3][2];
    return (v[0], v[1], v[2], v[3]);
}

static ap_uint<512> echo(state_t state) {
    state_t tmp = state;
    for (int i = 0; i < 10; i++) {
        tmp = echo_round(tmp, i);
    }
    return to_hash(state, tmp);
}

void echo512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = echo(to_state(pad_512(in)));
}
