#include "hash.hpp"
#include "state_matrix.hpp"

using nibble_t = ap_uint<4>;
using state_t = hls::vector<nibble_t, 256>;
using linear_t = struct linear_t {
    nibble_t a;
    nibble_t b;
};

const ap_uint<256> rc[42] = {
    ap_uint<256>("0x6a09e667f3bcc908b2fb1366ea957d3e3adec17512775099da2f590b0667322a", 16).reverse(),
    ap_uint<256>("0xbb896bf05955abcd5281828d66e7d99ac4203494f89bf12817deb43288712231", 16).reverse(),
    ap_uint<256>("0x1836e76b12d79c55118a1139d2417df52a2021225ff6350063d88e5f1f91631c", 16).reverse(),
    ap_uint<256>("0x263085a7000fa9c3317c6ca8ab65f7a7713cf4201060ce886af855a90d6a4eed", 16).reverse(),
    ap_uint<256>("0x1cebafd51a156aeb62a11fb3be2e14f60b7e48de85814270fd62e97614d7b441", 16).reverse(),
    ap_uint<256>("0xe5564cb574f7e09c75e2e244929e9549279ab224a28e445d57185e7d7a09fdc1", 16).reverse(),
    ap_uint<256>("0x5820f0f0d764cff3a5552a5e41a82b9eff6ee0aa615773bb07e8603424c3cf8a", 16).reverse(),
    ap_uint<256>("0xb126fb741733c5bfcef6f43a62e8e5706a26656028aa897ec1ea4616ce8fd510", 16).reverse(),
    ap_uint<256>("0xdbf0de32bca77254bb4f562581a3bc991cf94f225652c27f14eae958ae6aa616", 16).reverse(),
    ap_uint<256>("0xe6113be617f45f3de53cff03919a94c32c927b093ac8f23b47f7189aadb9bc67", 16).reverse(),
    ap_uint<256>("0x80d0d26052ca45d593ab5fb3102506390083afb5ffe107dacfcba7dbe601a12b", 16).reverse(),
    ap_uint<256>("0x43af1c76126714dfa950c368787c81ae3beecf956c85c962086ae16e40ebb0b4", 16).reverse(),
    ap_uint<256>("0x9aee8994d2d74a5cdb7b1ef294eed5c1520724dd8ed58c92d3f0e174b0c32045", 16).reverse(),
    ap_uint<256>("0x0b2aa58ceb3bdb9e1eef66b376e0c565d5d8fe7bacb8da866f859ac521f3d571", 16).reverse(),
    ap_uint<256>("0x7a1523ef3d970a3a9b0b4d610e02749d37b8d57c1885fe4206a7f338e8356866", 16).reverse(),
    ap_uint<256>("0x2c2db8f7876685f2cd9a2e0ddb64c9d5bf13905371fc39e0fa86e1477234a297", 16).reverse(),
    ap_uint<256>("0x9df085eb2544ebf62b50686a71e6e828dfed9dbe0b106c9452ceddff3d138990", 16).reverse(),
    ap_uint<256>("0xe6e5c42cb2d460c9d6e4791a1681bb2e222e54558eb78d5244e217d1bfcf5058", 16).reverse(),
    ap_uint<256>("0x8f1f57e44e126210f00763ff57da208a5093b8ff7947534a4c260a17642f72b2", 16).reverse(),
    ap_uint<256>("0xae4ef4792ea148608cf116cb2bff66e8fc74811266cd641112cd17801ed38b59", 16).reverse(),
    ap_uint<256>("0x91a744efbf68b192d0549b608bdb3191fc12a0e83543cec5f882250b244f78e4", 16).reverse(),
    ap_uint<256>("0x4b5d27d3368f9c17d4b2a2b216c7e74e7714d2cc03e1e44588cd9936de74357c", 16).reverse(),
    ap_uint<256>("0x0ea17cafb8286131bda9e3757b3610aa3f77a6d0575053fc926eea7e237df289", 16).reverse(),
    ap_uint<256>("0x848af9f57eb1a616e2c342c8cea528b8a95a5d16d9d87be9bb3784d0c351c32b", 16).reverse(),
    ap_uint<256>("0xc0435cc3654fb85dd9335ba91ac3dbde1f85d567d7ad16f9de6e009bca3f95b5", 16).reverse(),
    ap_uint<256>("0x927547fe5e5e45e2fe99f1651ea1cbf097dc3a3d40ddd21cee260543c288ec6b", 16).reverse(),
    ap_uint<256>("0xc117a3770d3a34469d50dfa7db020300d306a365374fa828c8b780ee1b9d7a34", 16).reverse(),
    ap_uint<256>("0x8ff2178ae2dbe5e872fac789a34bc228debf54a882743caad14f3a550fdbe68f", 16).reverse(),
    ap_uint<256>("0xabd06c52ed58ff091205d0f627574c8cbc1fe7cf79210f5a2286f6e23a27efa0", 16).reverse(),
    ap_uint<256>("0x631f4acb8d3ca4253e301849f157571d3211b6c1045347befb7c77df3c6ca7bd", 16).reverse(),
    ap_uint<256>("0xae88f2342c23344590be2014fab4f179fd4bf7c90db14fa4018fcce689d2127b", 16).reverse(),
    ap_uint<256>("0x93b89385546d71379fe41c39bc602e8b7c8b2f78ee914d1f0af0d437a189a8a4", 16).reverse(),
    ap_uint<256>("0x1d1e036abeef3f44848cd76ef6baa889fcec56cd7967eb909a464bfc23c72435", 16).reverse(),
    ap_uint<256>("0xa8e4ede4c5fe5e88d4fb192e0a0821e935ba145bbfc59c2508282755a5df53a5", 16).reverse(),
    ap_uint<256>("0x8e4e37a3b970f079ae9d22a499a714c875760273f74a9398995d32c05027d810", 16).reverse(),
    ap_uint<256>("0x61cfa42792f93b9fde36eb163e978709fafa7616ec3c7dad0135806c3d91a21b", 16).reverse(),
    ap_uint<256>("0xf037c5d91623288b7d0302c1b941b72676a943b372659dcd7d6ef408a11b40c0", 16).reverse(),
    ap_uint<256>("0x2a306354ca3ea90b0e97eaebcea0a6d7c6522399e885c613de824922c892c490", 16).reverse(),
    ap_uint<256>("0x3ca6cdd788a5bdc5ef2dceeb16bca31e0a0d2c7e9921b6f71d33e25dd2f3cf53", 16).reverse(),
    ap_uint<256>("0xf72578721db56bf8f49538b0ae6ea470c2fb1339dd26333f135f7def45376ec0", 16).reverse(),
    ap_uint<256>("0xe449a03eab359e34095f8b4b55cd7ac7c0ec6510f2c4cc79fa6b1fee6b18c59e", 16).reverse(),
    ap_uint<256>("0x73bd6978c59f2b219449b36770fb313fbe2da28f6b04275f071a1b193dde2072", 16).reverse()
};

static state_t sub_nibbles(state_t state, int round) {
#pragma HLS FUNCTION_INSTANTIATE variable=round
    const nibble_t sbox[2][16] = {
        { 9, 0, 4, 11, 13, 12, 3, 15, 1, 10, 2, 6, 7, 5, 8, 14 },
        { 3, 12, 6, 13, 5, 7, 1, 9, 15, 2, 0, 4, 11, 10, 14, 8 }
    };
#pragma HLS BIND_STORAGE variable=sbox type=rom_np impl=lutram
    state_t tmp;
    for (int i = 0; i < 256; i++) {
        tmp[i] = sbox[rc[round][i]][state[i]];
    }
    return tmp;
}

static linear_t mini_linear(nibble_t a, nibble_t b) {
#pragma HLS INLINE off
    nibble_t c, d;
    a.reverse();
    b.reverse();
    d[0] = b[0] ^ a[1];
    d[1] = b[1] ^ a[2];
    d[2] = b[2] ^ a[3] ^ a[0];
    d[3] = b[3] ^ a[0];
    c[0] = a[0] ^ d[1];
    c[1] = a[1] ^ d[2];
    c[2] = a[2] ^ d[3] ^ d[0];
    c[3] = a[3] ^ d[0];
    c.reverse();
    d.reverse();
    linear_t tmp;
    tmp.a = c;
    tmp.b = d;
    return tmp;
}

static state_t linear(state_t state) {
#pragma HLS INLINE off
    state_t tmp;
    for (int i = 0; i < 128; i++) {
        linear_t x = mini_linear(state[2 * i], state[2 * i + 1]);
        tmp[2 * i] = x.a;
        tmp[2 * i + 1] = x.b;
    }
    return tmp;
}

static state_t permute_0(state_t state) {
#pragma HLS INLINE off
    state_t tmp;
    for (int i = 0; i < 64; i++) {
        tmp[4 * i + 0] = state[4 * i + 0];
        tmp[4 * i + 1] = state[4 * i + 1];
        tmp[4 * i + 2] = state[4 * i + 3];
        tmp[4 * i + 3] = state[4 * i + 2];
    }
    return tmp;
}

static state_t permute_1(state_t state) {
#pragma HLS INLINE off
    state_t tmp;
    for (int i = 0; i < 128; i++) {
        tmp[i] = state[2 * i];
        tmp[i + 128] = state[2 * i + 1];
    }
    return tmp;
}

static state_t permute_2(state_t state) {
#pragma HLS INLINE off
    state_t tmp;
    for (int i = 0; i < 128; i++) {
        tmp[i] = state[i];
    }
    for (int i = 0; i < 128; i += 2) {
        tmp[128 + i + 0] = state[128 + i + 1];
        tmp[128 + i + 1] = state[128 + i + 0];
    }
    return tmp;
}

static state_t permute(state_t state) {
#pragma HLS INLINE off
    state = permute_0(state);
    state = permute_1(state);
    state = permute_2(state);
    return state;
}

static state_t compress_round(state_t state, int round) {
#pragma HLS FUNCTION_INSTANTIATE variable=round
    state = sub_nibbles(state, round);
    state = linear(state);
    state = permute(state);
    return state;
}

static ap_uint<1024> compress(ap_uint<1024> x) {
#pragma HLS INLINE off
    x.reverse();
    state_t state;
    for (int i = 0; i < 128; i++) {
        state[2 * i] = (x[i], x[i + 256], x[i + 512], x[i + 768]);
        state[2 * i + 1] = (x[i + 128], x[i + 384], x[i + 640], x[i + 896]);
    }

    for (int i = 0; i < 42; i++) {
        state = compress_round(state, i);
    }

    ap_uint<1024> tmp;
    for (int i = 0; i < 128; i++) {
        tmp[i] = state[2 * i][3];
        tmp[i + 256] = state[2 * i][2];
        tmp[i + 512] = state[2 * i][1];
        tmp[i + 768] = state[2 * i][0];
        tmp[i + 128] = state[2 * i + 1][3];
        tmp[i + 384] = state[2 * i + 1][2];
        tmp[i + 640] = state[2 * i + 1][1];
        tmp[i + 896] = state[2 * i + 1][0];
    }
    tmp.reverse();
    return tmp;
}

static ap_uint<512> compress(ap_uint<512> x) {
#pragma HLS INLINE
    ap_uint<1024> h = ap_uint<1024>("0x6fd14b963e00aa17636a2e057a15d5438a225e8d0c97ef0be9341259f2b3c361891da0c1536f801e2aa9056bea2b6d80588eccdb2075baa6a90f3a76baf83bf70169e60541e34a6946b58a8e2e6fe65a1047a7d0c1843c243b6e71b12d5ac199cf57f6ec9db1f856a706887c5716b156e3c2fcdfe68517fb545a4678cc8cdd4b", 16);
    h(1023, 512) = h(1023, 512) ^ x;
    h = compress(h);
    h(511, 0) = h(511, 0) ^ x;
    const ap_uint<512> padding = ap_uint<512>("0x80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200", 16);
    h(1023, 512) = h(1023, 512) ^ padding;
    h = compress(h);
    h(511, 0) = h(511, 0) ^ padding;
    return h(511, 0);
}

void jh512_top(ap_uint<512> in, ap_uint<512> *out) {
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
#pragma HLS INTERFACE mode=ap_none port=in
#pragma HLS INTERFACE mode=ap_none port=out
#pragma HLS PIPELINE II=1
    *out = compress(in);
}
