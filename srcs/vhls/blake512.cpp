#include "hash.hpp"

using word_t = ap_uint<64>;
using v_t = hls::vector<word_t, 16>;
using g_t = struct g_t {
    word_t a;
    word_t b;
    word_t c;
    word_t d;
};

using state_t = struct state_t {
    v_t v;
    v_t m;
};

const int sigma[10][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}
};

const ap_uint<64> constants[16] = {
    0x243F6A8885A308D3, 0x13198A2E03707344, 0xA4093822299F31D0, 0x082EFA98EC4E6C89,
    0x452821E638D01377, 0xBE5466CF34E90C6C, 0xC0AC29B7C97C50DD, 0x3F84D5B5B5470917,
    0x9216D5D98979FB1B, 0xD1310BA698DFB5AC, 0x2FFD72DBD01ADFB7, 0xB8E1AFED6A267E96,
    0xBA7C9045F12C7F99, 0x24A19947B3916CF7, 0x0801F2E2858EFC16, 0x636920D871574E69
};

static ap_uint<1024> pad_512(ap_uint<512> msg) {
#pragma HLS INLINE
    ap_uint<1024> tmp;
    tmp(1023, 512) = msg;
    tmp[511] = 0x1;
    tmp(510, 129) = 0x0;
    tmp[128] = 0x1;
    tmp(127, 0) = 512;
    return tmp;
}

static g_t g(word_t m0, word_t m1, word_t a, word_t b, word_t c, word_t d, word_t c_a, word_t c_b) {
#pragma HLS INLINE
    a = a + b + (m0 ^ c_b);
    d = rotr(d ^ a, 32);
    c = c + d;
    b = rotr(b ^ c, 25);
    a = a + b + (m1 ^ c_a);
    d = rotr(d ^ a, 16);
    c = c + d;
    b = rotr(b ^ c, 11);
    g_t ret;
    ret.a = a;
    ret.b = b;
    ret.c = c;
    ret.d = d;
    return ret;
}

static state_t blake_round(state_t state, int round) {
#pragma HLS FUNCTION_INSTANTIATE variable=round
    word_t rc[8][2];
    word_t m[8][2];
    for (int i = 0 ; i < 8; i++) {
        word_t s_a = sigma[round % 10][2 * i];
        word_t s_b = sigma[round % 10][2 * i + 1];
        rc[i][0] = constants[s_a];
        rc[i][1] = constants[s_b];
        m[i][0] = state.m[s_a];
        m[i][1] = state.m[s_b];
    }
    g_t g0 = g(m[0][0], m[0][1], state.v[0], state.v[4], state.v[8], state.v[12], rc[0][0], rc[0][1]);
    g_t g1 = g(m[1][0], m[1][1], state.v[1], state.v[5], state.v[9], state.v[13], rc[1][0], rc[1][1]);
    g_t g2 = g(m[2][0], m[2][1], state.v[2], state.v[6], state.v[10], state.v[14], rc[2][0], rc[2][1]);
    g_t g3 = g(m[3][0], m[3][1], state.v[3], state.v[7], state.v[11], state.v[15], rc[3][0], rc[3][1]);

    g_t g4 = g(m[4][0], m[4][1], g0.a, g1.b, g2.c, g3.d, rc[4][0], rc[4][1]);
    g_t g5 = g(m[5][0], m[5][1], g1.a, g2.b, g3.c, g0.d, rc[5][0], rc[5][1]);
    g_t g6 = g(m[6][0], m[6][1], g2.a, g3.b, g0.c, g1.d, rc[6][0], rc[6][1]);
    g_t g7 = g(m[7][0], m[7][1], g3.a, g0.b, g1.c, g2.d, rc[7][0], rc[7][1]);

    state.v[0] = g4.a;
    state.v[5] = g4.b;
    state.v[10] = g4.c;
    state.v[15] = g4.d;

    state.v[1] = g5.a;
    state.v[6] = g5.b;
    state.v[11] = g5.c;
    state.v[12] = g5.d;

    state.v[2] = g6.a;
    state.v[7] = g6.b;
    state.v[8] = g6.c;
    state.v[13] = g6.d;

    state.v[3] = g7.a;
    state.v[4] = g7.b;
    state.v[9] = g7.c;
    state.v[14] = g7.d;

    return state;
}

static ap_uint<512> blake(ap_uint<1024> padded_msg) {
    state_t init;
    init.v[0] = 0x6A09E667F3BCC908;
    init.v[1] = 0xBB67AE8584CAA73B;
    init.v[2] = 0x3C6EF372FE94F82B;
    init.v[3] = 0xA54FF53A5F1D36F1;
    init.v[4] = 0x510E527FADE682D1;
    init.v[5] = 0x9B05688C2B3E6C1F;
    init.v[6] = 0x1F83D9ABFB41BD6B;
    init.v[7] = 0x5BE0CD19137E2179;
    init.v[8] = constants[0];
    init.v[9] = constants[1];
    init.v[10] = constants[2];
    init.v[11] = constants[3];
    init.v[12] = constants[4] ^ 512;
    init.v[13] = constants[5] ^ 512;
    init.v[14] = constants[6];
    init.v[15] = constants[7];

    for (int i = 0; i < 16; i++) {
        init.m[i] = padded_msg((16 - i) * 64 - 1, (15 - i) * 64);
    }

    state_t tmp = init;
    for (int i = 0; i < 16; i++) {
        tmp = blake_round(tmp, i);
    }

    ap_uint<64> h0 = init.v[0] ^ tmp.v[0] ^ tmp.v[8];
    ap_uint<64> h1 = init.v[1] ^ tmp.v[1] ^ tmp.v[9];
    ap_uint<64> h2 = init.v[2] ^ tmp.v[2] ^ tmp.v[10];
    ap_uint<64> h3 = init.v[3] ^ tmp.v[3] ^ tmp.v[11];
    ap_uint<64> h4 = init.v[4] ^ tmp.v[4] ^ tmp.v[12];
    ap_uint<64> h5 = init.v[5] ^ tmp.v[5] ^ tmp.v[13];
    ap_uint<64> h6 = init.v[6] ^ tmp.v[6] ^ tmp.v[14];
    ap_uint<64> h7 = init.v[7] ^ tmp.v[7] ^ tmp.v[15];

    return (h0, h1, h2, h3, h4, h5, h6, h7);
}

void blake512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = blake(pad_512(in));
}
