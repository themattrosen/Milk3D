//Shared
#pragma once

// find the number of args in __VA_ARGS__ at compile time
//https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
// user mchiasson
#define EXPAND(x) x 
#define _NUM_VA_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9, _10, _11, _12, _13, _14, _15, _16, _17, N, ...) N 
#define NUM_VA_ARGS(...) EXPAND(_NUM_VA_ARGS(__VA_ARGS__, 17, 16, 15, 14, 13, 12, 11, 10, 9,8,7,6,5,4,3,2,1,0, -1))

// Call macro for each argument
//https://stackoverflow.com/questions/6707148/foreach-macro-on-macros-arguments
// user luser droog
#define PASTE(a, b) a ## b
#define XPASTE(a, b) PASTE(a, b)

#define APPLY_0(M) 
#define APPLY_1(M, a) M(a)
#define APPLY_2(M, a, b) M(a) M(b)
#define APPLY_3(M, a, b, c) M(a) M(b) M(c)
#define APPLY_4(M, a, b, c, d) M(a) M(b) M(c) M(d)
#define APPLY_5(M, a, b, c, d, e) M(a) M(b) M(c) M(d) M(e)
#define APPLY_6(M, a, b, c, d, e, f) M(a) M(b) M(c) M(d) M(e) M(f)
#define APPLY_7(M, a, b, c, d, e, f, g) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g)
#define APPLY_8(M, a, b, c, d, e, f, g, h)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h)
#define APPLY_9(M, a, b, c, d, e, f, g, h, i)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i)
#define APPLY_10(M, a, b, c, d, e, f, g, h, i, j)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j)
#define APPLY_11(M, a, b, c, d, e, f, g, h, i, j, k)    \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k)
#define APPLY_12(M, a, b, c, d, e, f, g, h, i, j, k, l) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l)
#define APPLY_13(M, a, b, c, d, e, f, g, h, i, j, k, l, m)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m)
#define APPLY_14(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n)   \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n)
#define APPLY_15(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)    \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o)
#define APPLY_16(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o) M(p)
#define APPLY_17(M, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)  \
    M(a) M(b) M(c) M(d) M(e) M(f) M(g) M(h) M(i) M(j) M(k) M(l) M(m) M(n) M(o) M(p) M(q)

#define APPLY_1_0(M, arg) 
#define APPLY_1_1(M, arg, a) M(arg, a)
#define APPLY_1_2(M, arg, a, b) M(arg, a) M(arg, b)
#define APPLY_1_3(M, arg, a, b, c) M(arg, a) M(arg, b) M(arg, c)
#define APPLY_1_4(M, arg, a, b, c, d) M(arg, a) M(arg, b) M(arg, c) M(arg, d)
#define APPLY_1_5(M, arg, a, b, c, d, e) M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e)
#define APPLY_1_6(M, arg, a, b, c, d, e, f) M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f)
#define APPLY_1_7(M, arg, a, b, c, d, e, f, g) \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g)
#define APPLY_1_8(M, arg, a, b, c, d, e, f, g, h)  \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h)
#define APPLY_1_9(M, arg, a, b, c, d, e, f, g, h, i)   \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i)
#define APPLY_1_10(M, arg, a, b, c, d, e, f, g, h, i, j)   \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) M(arg, j)
#define APPLY_1_11(M, arg, a, b, c, d, e, f, g, h, i, j, k)    \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) M(arg, j) M(arg, k)
#define APPLY_1_12(M, arg, a, b, c, d, e, f, g, h, i, j, k, l) \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) M(arg, j) M(arg, k) M(arg, l)
#define APPLY_1_13(M, arg, a, b, c, d, e, f, g, h, i, j, k, l, m)  \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) M(arg, j) M(arg, k) M(arg, l) M(arg, m)
#define APPLY_1_14(M, arg, a, b, c, d, e, f, g, h, i, j, k, l, m, n)   \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h)	\
	M(arg, i) M(arg, j) M(arg, k) M(arg, l) M(arg, m) M(arg, n)
#define APPLY_1_15(M, arg, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o)    \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) \
	M(arg, j) M(arg, k) M(arg, l) M(arg, m) M(arg, n) M(arg, o)
#define APPLY_1_16(M, arg, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) \
	M(arg, j) M(arg, k) M(arg, l) M(arg, m) M(arg, n) M(arg, o) M(arg, p)
#define APPLY_1_17(M, arg, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q)  \
    M(arg, a) M(arg, b) M(arg, c) M(arg, d) M(arg, e) M(arg, f) M(arg, g) M(arg, h) M(arg, i) \
	M(arg, j) M(arg, k) M(arg, l) M(arg, m) M(arg, n) M(arg, o) M(arg, p) M(arg, q)

#define APPLYX(APP, M, ...) EXPAND(APP(M, __VA_ARGS__))
#define APPLY_ON_EACH(M, ...) APPLYX(EXPAND(XPASTE(APPLY_, NUM_VA_ARGS(__VA_ARGS__))), M, __VA_ARGS__)

#define APPLYX_1(APP, M, arg, ...) EXPAND(APP(M, arg, __VA_ARGS__))
#define APPLY_ON_EACH_1(M, arg, ...) APPLYX_1(EXPAND(XPASTE(APPLY_1_, NUM_VA_ARGS(__VA_ARGS__))), M, arg, __VA_ARGS__) 
