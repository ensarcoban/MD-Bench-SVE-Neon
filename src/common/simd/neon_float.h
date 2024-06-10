#include <arm_neon.h>

#define MD_SIMD_FLOAT   float32x4_t
#define MD_SIMD_MASK    uint32x4_t
#define MD_SIMD_INT     int32x4_t
#define MD_SIMD_BITMASK MD_SIMD_INT
#define MD_SIMD_IBOOL   uint32x4_t

static inline MD_SIMD_MASK cvtIB2B(MD_SIMD_IBOOL a) { return a; }
static inline MD_SIMD_FLOAT simd_broadcast(float scalar)
{
    return vdupq_n_f32(scalar);
}
static inline MD_SIMD_FLOAT simd_zero(void) { return vdupq_n_f32(0.0f); }
static inline MD_SIMD_FLOAT simd_add(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vaddq_f32(a, b);
}
static inline MD_SIMD_FLOAT simd_sub(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vsubq_f32(a, b);
}
static inline MD_SIMD_FLOAT simd_mul(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vmulq_f32(a, b);
}
static inline MD_SIMD_FLOAT simd_fma(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_FLOAT c)
{
    return vfmaq_f32(c, a, b); // c + (a * b)
}
static inline MD_SIMD_FLOAT simd_reciprocal(MD_SIMD_FLOAT a)
{
    return vrecpeq_f32(a);
}
static inline MD_SIMD_FLOAT simd_masked_add(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_MASK m)
{
    return vbslq_f32(m, vaddq_f32(a, b), a);
}
static inline MD_SIMD_MASK simd_mask_and(MD_SIMD_MASK a, MD_SIMD_MASK b)
{
    return vandq_u32(a, b);
}
static inline MD_SIMD_MASK simd_mask_cond_lt(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b)
{
    return vcltq_f32(a, b);
}
static inline MD_SIMD_MASK simd_mask_from_u32(unsigned int a)
{
    return vdupq_n_u32(a);
}
static inline unsigned int simd_mask_to_u32(MD_SIMD_MASK a)
{
    return vgetq_lane_u32(a, 0);
}
static inline MD_SIMD_FLOAT simd_load(const float* p) { return vld1q_f32(p); }
static inline void simd_store(float* p, MD_SIMD_FLOAT a) { vst1q_f32(p, a); }
static inline MD_SIMD_FLOAT select_by_mask(MD_SIMD_FLOAT a, MD_SIMD_MASK m)
{
    return vbslq_f32(m, a, simd_zero());
}
static inline float simd_h_reduce_sum(MD_SIMD_FLOAT a)
{
    float32x2_t sum1 = vadd_f32(vget_low_f32(a), vget_high_f32(a));
    float32x2_t sum2 = vpadd_f32(sum1, sum1);
    return vget_lane_f32(sum2, 0);
}

static inline float simd_incr_reduced_sum(
    float* m, MD_SIMD_FLOAT v0, MD_SIMD_FLOAT v1, MD_SIMD_FLOAT v2, MD_SIMD_FLOAT v3)
{
    MD_SIMD_FLOAT t0, t2;
    float32x2_t t3, t4;

    t0 = vaddq_f32(v0, vextq_f32(v0, v0, 2));
    t2 = vaddq_f32(v2, vextq_f32(v2, v2, 2));
    t0 = vaddq_f32(t0, vextq_f32(v1, v1, 2));
    t2 = vaddq_f32(t2, vextq_f32(v3, v3, 2));
    t0 = vaddq_f32(t0, vextq_f32(t0, t0, 2));
    t0 = vaddq_f32(t0, t2);
    t3 = vget_low_f32(t0);
    t4 = vld1_f32(m);
    t4 = vadd_f32(t4, t3);
    vst1_f32(m, t4);

    t0 = vaddq_f32(t0, vrev64q_f32(t0));
    return vgetq_lane_f32(t0, 0);
}

static inline MD_SIMD_FLOAT simd_load_h_duplicate(const float* m)
{
    return vcombine_f32(vld1_f32(m), vld1_f32(m));
}

static inline MD_SIMD_FLOAT simd_load_h_dual(const float* m)
{
    return vcombine_f32(vld1_dup_f32(m), vld1_dup_f32(m + 1));
}

static inline float simd_h_dual_incr_reduced_sum(
    float* m, MD_SIMD_FLOAT v0, MD_SIMD_FLOAT v1)
{
    MD_SIMD_FLOAT t0;
    float32x2_t t2, t3;

    t0 = vaddq_f32(v0, vextq_f32(v0, v0, 2));
    t0 = vaddq_f32(t0, vextq_f32(v1, v1, 2));
    t2 = vget_low_f32(t0);
    t3 = vld1_f32(m);
    t3 = vadd_f32(t3, t2);
    vst1_f32(m, t3);

    t0 = vaddq_f32(t0, vrev64q_f32(t0));
    return vgetq_lane_f32(t0, 0);
}

static inline void simd_h_decr(float* m, MD_SIMD_FLOAT a)
{
    float32x2_t t;
    a = vaddq_f32(a, vrev64q_f32(a));
    t = vld1_f32(m);
    t = vsub_f32(t, vget_low_f32(a));
    vst1_f32(m, t);
}

static inline void simd_h_decr3(float* m, MD_SIMD_FLOAT a0, MD_SIMD_FLOAT a1, MD_SIMD_FLOAT a2)
{
    simd_h_decr(m, a0);
    simd_h_decr(m + 2, a1);
    simd_h_decr(m + 4, a2);
}

// Functions used in LAMMPS kernel
#define simd_gather(vidx, m, s) { \
    float32x4_t result; \
    for (int i = 0; i < 4; ++i) { \
        result[i] = m[vidx[i] * s]; \
    } \
    result; \
}
static inline MD_SIMD_INT simd_int_broadcast(int scalar)
{
    return vdupq_n_s32(scalar);
}
static inline MD_SIMD_INT simd_int_zero(void) { return vdupq_n_s32(0); }
static inline MD_SIMD_INT simd_int_seq(void)
{
    return vcombine_s32(vcreate_s32(0x03020100), vcreate_s32(0x07060504));
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
    MD_SIMD_INT result = simd_int_zero();
    return vbslq_s32(k, vld1q_s32(m), result);
}
static inline MD_SIMD_MASK simd_mask_int_cond_lt(MD_SIMD_INT a, MD_SIMD_INT b)
{
    return vcltq_s32(a, b);
}
