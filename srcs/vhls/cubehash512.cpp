#include "hash.hpp"

#define x0   (state[ 0])
#define x1   (state[ 1])
#define x2   (state[ 2])
#define x3   (state[ 3])
#define x4   (state[ 4])
#define x5   (state[ 5])
#define x6   (state[ 6])
#define x7   (state[ 7])
#define x8   (state[ 8])
#define x9   (state[ 9])
#define xa   (state[10])
#define xb   (state[11])
#define xc   (state[12])
#define xd   (state[13])
#define xe   (state[14])
#define xf   (state[15])
#define xg   (state[16])
#define xh   (state[17])
#define xi   (state[18])
#define xj   (state[19])
#define xk   (state[20])
#define xl   (state[21])
#define xm   (state[22])
#define xn   (state[23])
#define xo   (state[24])
#define xp   (state[25])
#define xq   (state[26])
#define xr   (state[27])
#define xs   (state[28])
#define xt   (state[29])
#define xu   (state[30])
#define xv   (state[31])

using word_t = ap_uint<32>;
using state_t = hls::vector<word_t, 32>;

const state_t iv = {
    0x2AEA2A61, 0x50F494D4, 0x2D538B8B, 0x4167D83E, 0x3FEE2313, 0xC701CF8C, 0xCC39968E, 0x50AC5695,
    0x4D42C787, 0xA647A8B3, 0x97CF0BEF, 0x825B4537, 0xEEF864D2, 0xF22090C4, 0xD0E5CD33, 0xA23911AE,
    0xFCD398D9, 0x148FE485, 0x1B017BEF, 0xB6444532, 0x6A536159, 0x2FF5781C, 0x91FA7934, 0x0DBADEA9,
    0xD65C8A2B, 0xA5A70E75, 0xB1C62456, 0xBC796576, 0x1921C8F7, 0xE7989AF1, 0x7795D246, 0xD43E3B44
};

static ap_uint<768> pad_512(ap_uint<512> msg) {
    ap_uint<768> tmp;
    tmp(767, 256) = msg;
    tmp[255] = 1;
    tmp(254, 0) = 0;
    return tmp;
}

static state_t add_input(state_t state, ap_uint<256> block) {
    for (int i = 0; i < 8; i++) {
        state[i] ^= reverse_bytes_32(block(255 - i * 32, 224 - i * 32));
    }
    return state;
}

static state_t round_even(state_t state) {
    xg = (x0 + xg);
    x0 = rotl(x0, 7);
    xh = (x1 + xh);
    x1 = rotl(x1, 7);
    xi = (x2 + xi);
    x2 = rotl(x2, 7);
    xj = (x3 + xj);
    x3 = rotl(x3, 7);
    xk = (x4 + xk);
    x4 = rotl(x4, 7);
    xl = (x5 + xl);
    x5 = rotl(x5, 7);
    xm = (x6 + xm);
    x6 = rotl(x6, 7);
    xn = (x7 + xn);
    x7 = rotl(x7, 7);
    xo = (x8 + xo);
    x8 = rotl(x8, 7);
    xp = (x9 + xp);
    x9 = rotl(x9, 7);
    xq = (xa + xq);
    xa = rotl(xa, 7);
    xr = (xb + xr);
    xb = rotl(xb, 7);
    xs = (xc + xs);
    xc = rotl(xc, 7);
    xt = (xd + xt);
    xd = rotl(xd, 7);
    xu = (xe + xu);
    xe = rotl(xe, 7);
    xv = (xf + xv);
    xf = rotl(xf, 7);
    x8 ^= xg;
    x9 ^= xh;
    xa ^= xi;
    xb ^= xj;
    xc ^= xk;
    xd ^= xl;
    xe ^= xm;
    xf ^= xn;
    x0 ^= xo;
    x1 ^= xp;
    x2 ^= xq;
    x3 ^= xr;
    x4 ^= xs;
    x5 ^= xt;
    x6 ^= xu;
    x7 ^= xv;
    xi = (x8 + xi);
    x8 = rotl(x8, 11);
    xj = (x9 + xj);
    x9 = rotl(x9, 11);
    xg = (xa + xg);
    xa = rotl(xa, 11);
    xh = (xb + xh);
    xb = rotl(xb, 11);
    xm = (xc + xm);
    xc = rotl(xc, 11);
    xn = (xd + xn);
    xd = rotl(xd, 11);
    xk = (xe + xk);
    xe = rotl(xe, 11);
    xl = (xf + xl);
    xf = rotl(xf, 11);
    xq = (x0 + xq);
    x0 = rotl(x0, 11);
    xr = (x1 + xr);
    x1 = rotl(x1, 11);
    xo = (x2 + xo);
    x2 = rotl(x2, 11);
    xp = (x3 + xp);
    x3 = rotl(x3, 11);
    xu = (x4 + xu);
    x4 = rotl(x4, 11);
    xv = (x5 + xv);
    x5 = rotl(x5, 11);
    xs = (x6 + xs);
    x6 = rotl(x6, 11);
    xt = (x7 + xt);
    x7 = rotl(x7, 11);
    xc ^= xi;
    xd ^= xj;
    xe ^= xg;
    xf ^= xh;
    x8 ^= xm;
    x9 ^= xn;
    xa ^= xk;
    xb ^= xl;
    x4 ^= xq;
    x5 ^= xr;
    x6 ^= xo;
    x7 ^= xp;
    x0 ^= xu;
    x1 ^= xv;
    x2 ^= xs;
    x3 ^= xt;
    return state;
}

static state_t round_odd(state_t state) {
    xj = (xc + xj);
    xc = rotl(xc, 7);
    xi = (xd + xi);
    xd = rotl(xd, 7);
    xh = (xe + xh);
    xe = rotl(xe, 7);
    xg = (xf + xg);
    xf = rotl(xf, 7);
    xn = (x8 + xn);
    x8 = rotl(x8, 7);
    xm = (x9 + xm);
    x9 = rotl(x9, 7);
    xl = (xa + xl);
    xa = rotl(xa, 7);
    xk = (xb + xk);
    xb = rotl(xb, 7);
    xr = (x4 + xr);
    x4 = rotl(x4, 7);
    xq = (x5 + xq);
    x5 = rotl(x5, 7);
    xp = (x6 + xp);
    x6 = rotl(x6, 7);
    xo = (x7 + xo);
    x7 = rotl(x7, 7);
    xv = (x0 + xv);
    x0 = rotl(x0, 7);
    xu = (x1 + xu);
    x1 = rotl(x1, 7);
    xt = (x2 + xt);
    x2 = rotl(x2, 7);
    xs = (x3 + xs);
    x3 = rotl(x3, 7);
    x4 ^= xj;
    x5 ^= xi;
    x6 ^= xh;
    x7 ^= xg;
    x0 ^= xn;
    x1 ^= xm;
    x2 ^= xl;
    x3 ^= xk;
    xc ^= xr;
    xd ^= xq;
    xe ^= xp;
    xf ^= xo;
    x8 ^= xv;
    x9 ^= xu;
    xa ^= xt;
    xb ^= xs;
    xh = (x4 + xh);
    x4 = rotl(x4, 11);
    xg = (x5 + xg);
    x5 = rotl(x5, 11);
    xj = (x6 + xj);
    x6 = rotl(x6, 11);
    xi = (x7 + xi);
    x7 = rotl(x7, 11);
    xl = (x0 + xl);
    x0 = rotl(x0, 11);
    xk = (x1 + xk);
    x1 = rotl(x1, 11);
    xn = (x2 + xn);
    x2 = rotl(x2, 11);
    xm = (x3 + xm);
    x3 = rotl(x3, 11);
    xp = (xc + xp);
    xc = rotl(xc, 11);
    xo = (xd + xo);
    xd = rotl(xd, 11);
    xr = (xe + xr);
    xe = rotl(xe, 11);
    xq = (xf + xq);
    xf = rotl(xf, 11);
    xt = (x8 + xt);
    x8 = rotl(x8, 11);
    xs = (x9 + xs);
    x9 = rotl(x9, 11);
    xv = (xa + xv);
    xa = rotl(xa, 11);
    xu = (xb + xu);
    xb = rotl(xb, 11);
    x0 ^= xh;
    x1 ^= xg;
    x2 ^= xj;
    x3 ^= xi;
    x4 ^= xl;
    x5 ^= xk;
    x6 ^= xn;
    x7 ^= xm;
    x8 ^= xp;
    x9 ^= xo;
    xa ^= xr;
    xb ^= xq;
    xc ^= xt;
    xd ^= xs;
    xe ^= xv;
    xf ^= xu;
    return state;
}

static state_t transform(state_t state) {
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    state = round_even(state);
    state = round_odd(state);
    return state;
}

static ap_uint<512> finalize(state_t state) {
    xv ^= 1;
    for (int i = 0; i < 10; i++) {
        state = transform(state);
    }
    return (
        reverse_bytes_32(x0),
        reverse_bytes_32(x1),
        reverse_bytes_32(x2),
        reverse_bytes_32(x3),
        reverse_bytes_32(x4),
        reverse_bytes_32(x5),
        reverse_bytes_32(x6),
        reverse_bytes_32(x7),
        reverse_bytes_32(x8),
        reverse_bytes_32(x9),
        reverse_bytes_32(xa),
        reverse_bytes_32(xb),
        reverse_bytes_32(xc),
        reverse_bytes_32(xd),
        reverse_bytes_32(xe),
        reverse_bytes_32(xf)
    );
}

void cubehash512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    state_t state = iv;
    ap_uint<768> padded_msg = pad_512(in);
    state = add_input(state, padded_msg(767, 512));
    state = transform(state);
    state = add_input(state, padded_msg(511, 256));
    state = transform(state);
    state = add_input(state, padded_msg(255, 0));
    state = transform(state);
    *out = finalize(state);
}
