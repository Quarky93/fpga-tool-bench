#ifndef WARPHASH_H
#define WARPHASH_H

#define AP_INT_MAX_W 4096
#include <iostream>
#include <ap_int.h>
#include <hls_stream.h>
#include <hls_vector.h>
#include <ap_axi_sdata.h>

using namespace hls;

template<int S>
ap_uint<S> rotr(ap_uint<S> x, int n) {
#pragma HLS INLINE
    ap_uint<S> tmp = x;
    tmp.rrotate(n);
    return tmp;
}

template<int S>
ap_uint<S> rotl(ap_uint<S> x, int n) {
#pragma HLS INLINE
    ap_uint<S> tmp = x;
    tmp.lrotate(n);
    return tmp;
}

template<int S, int N>
ap_uint<S*N> concat_vector(hls::vector<ap_uint<S>, N> x) {
#pragma HLS INLINE
    int len = S*N;
    ap_uint<S*N> tmp;
    for (int i = 0; i < N; i++) {
        tmp((len - 1) - (i * S), (len - S) - (i * S)) = x[i];
    }
    return tmp;
}

inline ap_uint<128> reverse_bytes_128(const ap_uint<128> x) {
#pragma HLS INLINE
    ap_uint<128> tmp;
    for (int i = 0; i < 16; i++) {
        tmp(127 - 8 * i, 120 - 8 * i) = x(7 + 8 * i, 8 * i);
    }
    return tmp;
}

inline ap_uint<64> reverse_bytes_64(const ap_uint<64> x) {
#pragma HLS INLINE
    ap_uint<64> tmp;
    for (int i = 0; i < 8; i++) {
        tmp(63 - 8 * i, 56 - 8 * i) = x(7 + 8 * i, 8 * i);
    }
    return tmp;
}

inline ap_uint<32> reverse_bytes_32(const ap_uint<32> x) {
#pragma HLS INLINE
    ap_uint<32> tmp;
    for (int i = 0; i < 4; i++) {
        tmp(31 - 8 * i, 24 - 8 * i) = x(7 + 8 * i, 8 * i);
    }
    return tmp;
}

#endif