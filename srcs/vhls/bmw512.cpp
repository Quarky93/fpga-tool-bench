#include "hash.hpp"

#define SHL(x, n) ((x) << (n))
#define SHR(x, n) ((x) >> (n))
#define S64_0(x) (SHR((x), 1) ^ SHL((x), 3) ^ rotl((x), 4) ^ rotl((x), 37))
#define S64_1(x) (SHR((x), 1) ^ SHL((x), 2) ^ rotl((x), 13) ^ rotl((x), 43))
#define S64_2(x) (SHR((x), 2) ^ SHL((x), 1) ^ rotl((x), 19) ^ rotl((x), 53))
#define S64_3(x) (SHR((x), 2) ^ SHL((x), 2) ^ rotl((x), 28) ^ rotl((x), 59))
#define S64_4(x)  (SHR((x), 1) ^ (x))
#define S64_5(x)  (SHR((x), 2) ^ (x))
#define R64_1(x) rotl((x), 5)
#define R64_2(x) rotl((x), 11)
#define R64_3(x) rotl((x), 27)
#define R64_4(x) rotl((x), 32)
#define R64_5(x) rotl((x), 37)
#define R64_6(x) rotl((x), 43)
#define R64_7(x) rotl((x), 53)

using word_t = ap_uint<64>;
using double_pipe_t = hls::vector<word_t, 16>;
using quad_pipe_t = hls::vector<word_t, 32>;

const double_pipe_t iv = {
    0x8081828384858687, 0x88898a8b8c8d8e8f,
    0x9091929394959697, 0x98999a9b9c9d9e9f,
    0xa0a1a2a3a4a5a6a7, 0xa8a9aaabacadaeaf,
    0xb0b1b2b3b4b5b6b7, 0xb8b9babbbcbdbebf,
    0xc0c1c2c3c4c5c6c7, 0xc8c9cacbcccdcecf,
    0xd0d1d2d3d4d5d6d7, 0xd8d9dadbdcdddedf,
    0xe0e1e2e3e4e5e6e7, 0xe8e9eaebecedeeef,
    0xf0f1f2f3f4f5f6f7, 0xf8f9fafbfcfdfeff
};

const double_pipe_t fv = {
    0xaaaaaaaaaaaaaaa0,  0xaaaaaaaaaaaaaaa1,
    0xaaaaaaaaaaaaaaa2,  0xaaaaaaaaaaaaaaa3,
    0xaaaaaaaaaaaaaaa4,  0xaaaaaaaaaaaaaaa5,
    0xaaaaaaaaaaaaaaa6,  0xaaaaaaaaaaaaaaa7,
    0xaaaaaaaaaaaaaaa8,  0xaaaaaaaaaaaaaaa9,
    0xaaaaaaaaaaaaaaaa,  0xaaaaaaaaaaaaaaab,
    0xaaaaaaaaaaaaaaac,  0xaaaaaaaaaaaaaaad,
    0xaaaaaaaaaaaaaaae,  0xaaaaaaaaaaaaaaaf
};

static ap_uint<1024> pad_512(ap_uint<512> msg) {
#pragma HLS INLINE
    ap_uint<1024> tmp;
    tmp(1023, 512) = msg;
    tmp[511] = 1;
    tmp(510, 64) = 0;
    tmp(63, 0) = 512;
    return tmp;
}

static double_pipe_t to_state(ap_uint<1024> padded_msg) {
#pragma HLS INLINE
    double_pipe_t tmp;
    tmp[0] = reverse_bytes_64(padded_msg(1023, 960));
    tmp[1] = reverse_bytes_64(padded_msg(959, 896));
    tmp[2] = reverse_bytes_64(padded_msg(895, 832));
    tmp[3] = reverse_bytes_64(padded_msg(831, 768));
    tmp[4] = reverse_bytes_64(padded_msg(767, 704));
    tmp[5] = reverse_bytes_64(padded_msg(703, 640));
    tmp[6] = reverse_bytes_64(padded_msg(639, 576));
    tmp[7] = reverse_bytes_64(padded_msg(575, 512));
    tmp[8] = reverse_bytes_64(padded_msg(511, 448));
    tmp[9] = reverse_bytes_64(padded_msg(447, 384));
    tmp[10] = reverse_bytes_64(padded_msg(383, 320));
    tmp[11] = reverse_bytes_64(padded_msg(319, 256));
    tmp[12] = reverse_bytes_64(padded_msg(255, 192));
    tmp[13] = reverse_bytes_64(padded_msg(191, 128));
    tmp[14] = reverse_bytes_64(padded_msg(127, 64));
    tmp[15] = padded_msg(63, 0);
    return tmp;
}

static ap_uint<512> to_hash(double_pipe_t x) {
#pragma HLS INLINE
    ap_uint<512> tmp;
    tmp(511, 448) = reverse_bytes_64(x[8]);
    tmp(447, 384) = reverse_bytes_64(x[9]);
    tmp(383, 320) = reverse_bytes_64(x[10]);
    tmp(319, 256) = reverse_bytes_64(x[11]);
    tmp(255, 192) = reverse_bytes_64(x[12]);
    tmp(191, 128) = reverse_bytes_64(x[13]);
    tmp(127, 64) = reverse_bytes_64(x[14]);
    tmp(63, 0) = reverse_bytes_64(x[15]);
    return tmp;
}

static word_t expand_f1(double_pipe_t m, double_pipe_t h, quad_pipe_t q, int i) {
#pragma HLS INLINE
    return S64_1(q[i-16]) + S64_2(q[i-15]) + S64_3(q[i-14]) + S64_0(q[i-13]) +
    S64_1(q[i-12]) + S64_2(q[i-11]) + S64_3(q[i-10]) + S64_0(q[i-9]) +
    S64_1(q[i-8]) + S64_2(q[i-7]) + S64_3(q[i-6]) + S64_0(q[i-5]) +
    S64_1(q[i-4]) + S64_2(q[i-3]) + S64_3(q[i-2]) + S64_0(q[i-1]) +
    ((i*(0x0555555555555555ull) + rotl(m[(i-16)%16],((i-16)%16)+1) + rotl(m[(i-13)%16],((i-13)%16)+1) - rotl(m[(i-6)%16],((i-6)%16)+1)) ^ h[(i-16+7)%16]);
}

static word_t expand_f2(double_pipe_t m, double_pipe_t h, quad_pipe_t q, int i) {
#pragma HLS INLINE
    return q[i-16] + R64_1(q[i-15]) + q[i-14] + R64_2(q[i-13]) +
    q[i-12] + R64_3(q[i-11]) + q[i-10] + R64_4(q[i-9]) +
    q[i-8] + R64_5(q[i-7]) + q[i-6] + R64_6(q[i-5]) +
    q[i-4] + R64_7(q[i-3]) + S64_4(q[i-2]) + S64_5(q[i-1]) +
    ((i*(0x0555555555555555ull) + rotl(m[(i-16)%16],((i-16)%16)+1) + rotl(m[(i-13)%16],((i-13)%16)+1) - rotl(m[(i-6)%16],((i-6)%16)+1)) ^ h[(i-16+7)%16]);
}

static double_pipe_t compress(double_pipe_t m, double_pipe_t h) {
#pragma HLS INLINE off
    double_pipe_t w;
    quad_pipe_t q;

    w[0] = (m[5] ^ h[5]) - (m[7] ^ h[7]) + (m[10] ^ h[10]) + (m[13] ^ h[13]) + (m[14] ^ h[14]);
    w[1] = (m[6] ^ h[6]) - (m[8] ^ h[8]) + (m[11] ^ h[11]) + (m[14] ^ h[14]) - (m[15] ^ h[15]);
    w[2] = (m[0] ^ h[0]) + (m[7] ^ h[7]) + (m[9] ^ h[9]) - (m[12] ^ h[12]) + (m[15] ^ h[15]);
    w[3] = (m[0] ^ h[0]) - (m[1] ^ h[1]) + (m[8] ^ h[8]) - (m[10] ^ h[10]) + (m[13] ^ h[13]);
    w[4] = (m[1] ^ h[1]) + (m[2] ^ h[2]) + (m[9] ^ h[9]) - (m[11] ^ h[11]) - (m[14] ^ h[14]);
    w[5] = (m[3] ^ h[3]) - (m[2] ^ h[2]) + (m[10] ^ h[10]) - (m[12] ^ h[12]) + (m[15] ^ h[15]);
    w[6] = (m[4] ^ h[4]) - (m[0] ^ h[0]) - (m[3] ^ h[3]) - (m[11] ^ h[11]) + (m[13] ^ h[13]);
    w[7] = (m[1] ^ h[1]) - (m[4] ^ h[4]) - (m[5] ^ h[5]) - (m[12] ^ h[12]) - (m[14] ^ h[14]);
    w[8] = (m[2] ^ h[2]) - (m[5] ^ h[5]) - (m[6] ^ h[6]) + (m[13] ^ h[13]) - (m[15] ^ h[15]);
    w[9] = (m[0] ^ h[0]) - (m[3] ^ h[3]) + (m[6] ^ h[6]) - (m[7] ^ h[7]) + (m[14] ^ h[14]);
    w[10] = (m[8] ^ h[8]) - (m[1] ^ h[1]) - (m[4] ^ h[4]) - (m[7] ^ h[7]) + (m[15] ^ h[15]);
    w[11] = (m[8] ^ h[8]) - (m[0] ^ h[0]) - (m[2] ^ h[2]) - (m[5] ^ h[5]) + (m[9] ^ h[9]);
    w[12] = (m[1] ^ h[1]) + (m[3] ^ h[3]) - (m[6] ^ h[6]) - (m[9] ^ h[9]) + (m[10] ^ h[10]);
    w[13] = (m[2] ^ h[2]) + (m[4] ^ h[4]) + (m[7] ^ h[7]) + (m[10] ^ h[10]) + (m[11] ^ h[11]);
    w[14] = (m[3] ^ h[3]) - (m[5] ^ h[5]) + (m[8] ^ h[8]) - (m[11] ^ h[11]) - (m[12] ^ h[12]);
    w[15] = (m[12] ^ h[12]) - (m[4] ^ h[4]) - (m[6] ^ h[6]) - (m[9] ^ h[9]) + (m[13] ^ h[13]);

    q[0] = S64_0(w[0]) + h[1];
    q[1] = S64_1(w[1]) + h[2];
    q[2] = S64_2(w[2]) + h[3];
    q[3] = S64_3(w[3]) + h[4];
    q[4] = S64_4(w[4]) + h[5];
    q[5] = S64_0(w[5]) + h[6];
    q[6] = S64_1(w[6]) + h[7];
    q[7] = S64_2(w[7]) + h[8];
    q[8] = S64_3(w[8]) + h[9];
    q[9] = S64_4(w[9]) + h[10];
    q[10] = S64_0(w[10]) + h[11];
    q[11] = S64_1(w[11]) + h[12];
    q[12] = S64_2(w[12]) + h[13];
    q[13] = S64_3(w[13]) + h[14];
    q[14] = S64_4(w[14]) + h[15];
    q[15] = S64_0(w[15]) + h[0];

    for (int i = 0; i < 2; i++) {
        q[i+16] = expand_f1(m, h, q, i + 16);
    }

    for (int i = 0; i < 14; i++) {
        q[i+16+2] = expand_f2(m, h, q, i + 16 + 2);
    }

    word_t xl = q[16] ^ q[17] ^ q[18] ^ q[19] ^ q[20] ^ q[21] ^ q[22] ^ q[23];
    word_t xh = xl ^ q[24] ^ q[25] ^ q[26] ^ q[27] ^ q[28] ^ q[29] ^ q[30] ^ q[31];

    double_pipe_t tmp;
    tmp[0] = (SHL(xh, 5) ^ SHR(q[16], 5) ^ m[0]) + (xl ^ q[24] ^ q[0]);
    tmp[1] = (SHR(xh, 7) ^ SHL(q[17], 8) ^ m[1]) + (xl ^ q[25] ^ q[1]);
    tmp[2] = (SHR(xh, 5) ^ SHL(q[18], 5) ^ m[2]) + (xl ^ q[26] ^ q[2]);
    tmp[3] = (SHR(xh, 1) ^ SHL(q[19], 5) ^ m[3]) + (xl ^ q[27] ^ q[3]);
    tmp[4] = (SHR(xh, 3) ^ q[20] ^ m[4]) + (xl ^ q[28] ^ q[4]);
    tmp[5] = (SHL(xh, 6) ^ SHR(q[21], 6) ^ m[5]) + (xl ^ q[29] ^ q[5]);
    tmp[6] = (SHR(xh, 4) ^ SHL(q[22], 6) ^ m[6]) + (xl ^ q[30] ^ q[6]);
    tmp[7] = (SHR(xh, 11) ^ SHL(q[23], 2) ^ m[7]) + (xl ^ q[31] ^ q[7]);

    tmp[8] = rotl(tmp[4], 9) + (xh ^ q[24] ^ m[8]) + (SHL(xl, 8) ^ q[23] ^ q[8]);
    tmp[9] = rotl(tmp[5], 10) + (xh ^ q[25] ^ m[9]) + (SHR(xl, 6) ^ q[16] ^ q[9]);
    tmp[10] = rotl(tmp[6], 11) + (xh ^ q[26] ^ m[10]) + (SHL(xl, 6) ^ q[17] ^ q[10]);
    tmp[11] = rotl(tmp[7], 12) + (xh ^ q[27] ^ m[11]) + (SHL(xl, 4) ^ q[18] ^ q[11]);
    tmp[12] = rotl(tmp[0], 13) + (xh ^ q[28] ^ m[12]) + (SHR(xl, 3) ^ q[19] ^ q[12]);
    tmp[13] = rotl(tmp[1], 14) + (xh ^ q[29] ^ m[13]) + (SHR(xl, 4) ^ q[20] ^ q[13]);
    tmp[14] = rotl(tmp[2], 15) + (xh ^ q[30] ^ m[14]) + (SHR(xl, 7) ^ q[21] ^ q[14]);
    tmp[15] = rotl(tmp[3], 16) + (xh ^ q[31] ^ m[15]) + (SHR(xl, 2) ^ q[22] ^ q[15]);
    return tmp;
}

static ap_uint<512> bmw(double_pipe_t m) {
#pragma HLS INLINE off
    double_pipe_t tmp = compress(m, iv);
    return to_hash(compress(tmp, fv));
}

void bmw512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = bmw(to_state(pad_512(in)));
}
