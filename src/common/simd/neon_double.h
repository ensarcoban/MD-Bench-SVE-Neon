/*
  Copyright (C) NHR@FAU, University Erlangen-Nuremberg.
  All rights reserved. This file is part of MD-Bench.
  Use of this source code is governed by a LGPL-3.0
  license that can be found in the LICENSE file.
  neon_double.h
*/

#include <arm_neon.h>

#define MD_SIMD_FLOAT float64x2_t
#define MD_SIMD_MASK uint64x2_t
#define MD_SIMD_INT int32x4_t
#define MD_SIMD_BITMASK MD_SIMD_INT
#define MD_SIMD_IBOOL uint16x8_t

static inline MD_SIMD_MASK cvtIB2B(MD_SIMD_IBOOL a) { return (MD_SIMD_MASK)a; }
static inline MD_SIMD_FLOAT simd_broadcast(double scalar)
{
    return vdupq_n_f64(scalar);
}
static inline MD_SIMD_FLOAT simd_zero(void) { return vdupq_n_f64(0.0); }
static inline MD_SIMD_FLOAT simd_add(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vaddq_f64(a, b);
}
static inline MD_SIMD_FLOAT simd_sub(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vsubq_f64(a, b);
}
static inline MD_SIMD_FLOAT simd_mul(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vmulq_f64(a, b);
}
static inline MD_SIMD_FLOAT simd_fma(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_FLOAT c)
{
    return vmlaq_f64(c, a, b);
}
static inline MD_SIMD_FLOAT simd_reciprocal(MD_SIMD_FLOAT a)
{
    return vrecpeq_f64(a);
}
static inline MD_SIMD_FLOAT simd_masked_add(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_MASK m)
{
    return vreinterpretq_f64_u64(vbslq_u64(m, vreinterpretq_u64_f64(vaddq_f64(a, b)), vreinterpretq_u64_f64(a)));
}
static inline MD_SIMD_MASK simd_mask_and(MD_SIMD_MASK a, MD_SIMD_MASK b)
{
    return vandq_u64(a, b);
}
static inline MD_SIMD_MASK simd_mask_cond_lt(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vreinterpretq_u64_u32(vcltq_f64(a, b));
}
static inline MD_SIMD_MASK simd_mask_from_u32(unsigned int a) { return vreinterpretq_u64_u32(vdupq_n_u32(a)); }
static inline unsigned int simd_mask_to_u32(MD_SIMD_MASK a) { return vgetq_lane_u32(vreinterpretq_u32_u64(a), 0); }
static inline MD_SIMD_FLOAT simd_load(double* p) { return vld1q_f64(p); }
static inline void simd_store(double* p, MD_SIMD_FLOAT a) { vst1q_f64(p, a); }
static inline MD_SIMD_FLOAT select_by_mask(MD_SIMD_FLOAT a, MD_SIMD_MASK m)
{
    return vreinterpretq_f64_u64(vbslq_u64(m, vreinterpretq_u64_f64(a), vdupq_n_u64(0)));
}
static inline double simd_h_reduce_sum(MD_SIMD_FLOAT a)
{
    double sum = vgetq_lane_f64(a, 0) + vgetq_lane_f64(a, 1);
    return sum;
}

static inline double simd_incr_reduced_sum(
    double* m, MD_SIMD_FLOAT v0, MD_SIMD_FLOAT v1, MD_SIMD_FLOAT v2, MD_SIMD_FLOAT v3)
{
    MD_SIMD_FLOAT t0, t2;
    t0 = vaddq_f64(v0, v1);
    t2 = vaddq_f64(v2, v3);
    t0 = vaddq_f64(t0, t2);
    double sum = simd_h_reduce_sum(t0);
    *m += sum;
    return sum;
}

static inline MD_SIMD_FLOAT simd_load_h_duplicate(const double* m)
{
    return vdupq_n_f64(m[0]);
}

static inline MD_SIMD_FLOAT simd_load_h_dual(const double* m)
{
    return vcombine_f64(vdup_n_f64(m[0]), vdup_n_f64(m[1]));
}

static inline double simd_h_dual_incr_reduced_sum(
    double* m, MD_SIMD_FLOAT v0, MD_SIMD_FLOAT v1)
{
    MD_SIMD_FLOAT t0 = vaddq_f64(v0, v1);
    double sum = simd_h_reduce_sum(t0);
    *m += sum;
    return sum;
}

static inline void simd_h_decr(double* m, MD_SIMD_FLOAT a)
{
    double sum = simd_h_reduce_sum(a);
    *m -= sum;
}

static inline void simd_h_decr3(
    double* m, MD_SIMD_FLOAT a0, MD_SIMD_FLOAT a1, MD_SIMD_FLOAT a2)
{
    simd_h_decr(m, a0);
    simd_h_decr(m + 2, a1);  // Assuming CLUSTER_N is 2 for simplicity
    simd_h_decr(m + 4, a2);  // Assuming CLUSTER_N is 2 for simplicity
}

// Functions used in LAMMPS kernel
#define simd_gather(vidx, m, s) (vld1q_f64(m + vgetq_lane_s32(vidx, 0) * s))

static inline MD_SIMD_INT simd_int_broadcast(int scalar)
{
    return vdupq_n_s32(scalar);
}

static inline MD_SIMD_INT simd_int_zero(void) { return vdupq_n_s32(0); }

static inline MD_SIMD_INT simd_int_seq(void)
{
    return vcombine_s32(vcreate_s32(0x0000000100000000), vcreate_s32(0x0000000300000002));
}

static inline MD_SIMD_INT simd_int_load(const int* m)
{
    return vld1q_s32(m);
}

static inline MD_SIMD_INT simd_int_add(MD_SIMD_INT a, MD_SIMD_INT b)
{
    return vaddq_s32(a, b);
}

static inline MD_SIMD_INT simd_int_mul(MD_SIMD_INT a, MD_SIMD_INT b)
{
    return vmulq_s32(a, b);
}

static inline MD_SIMD_INT simd_int_mask_load(const int* m, MD_SIMD_MASK k)
{
    MD_SIMD_INT zero = simd_int_zero();
    MD_SIMD_INT masked_load = vld1q_s32(m);
    return vbslq_s32(k, masked_load, zero);
}

static inline MD_SIMD_MASK simd_mask_int_cond_lt(MD_SIMD_INT a, MD_SIMD_INT b)
{
    return vreinterpretq_u64_s32(vcltq_s32(a, b));
}
