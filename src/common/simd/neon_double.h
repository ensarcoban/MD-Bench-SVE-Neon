#include <arm_acle.h>
#include <arm_neon.h>

// Typedefs for MD_SIMD_FLOAT and MD_SIMD_MASK for double precision
#define MD_SIMD_FLOAT float64x2_t
#define MD_SIMD_MASK  uint64x2_t 

static inline float64x2_t simd_broadcast(double value)
{
    return vdupq_n_f64(value);
}

static inline float64x2_t simd_zero(void)
{
    return vdupq_n_f64(0.0);;
}

// Helper function to subtract two float64x2_t vectors
static inline float64x2_t simd_sub(float64x2_t a, float64x2_t b)
{
    return vsubq_f64(a, b);
}

static inline float64x2_t simd_load(const double* ptr)
{
    // Load two 128-bit vectors in one instruction
    return vld1q_f64(ptr);
}

static inline void simd_store(double* ptr, float64x2_t vec)
{
    // Store the first 128-bit vector (first two doubles)
    vst1q_f64(ptr, vec);
}

static inline float64x2_t simd_add(float64x2_t a, float64x2_t b)
{
    return vaddq_f64(a, b);
}

static inline float64x2_t simd_mul(float64x2_t a, float64x2_t b)
{
    return vmulq_f64(a, b);
}

static inline float64x2_t simd_fma(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return vfmaq_f64(c, a, b);
}

static inline uint64x2_t simd_mask_from_u32(uint32_t a) // @?
{
    const uint64_t all  = 0xFFFFFFFFFFFFFFFF;
    const uint64_t none = 0x0;
    // int64x2_t v0 = {(int64_t)(a & 0x2)? all:none,(int64_t)(a & 0x1)? all:none};
    // return v0;
    return vreinterpretq_f64_u64(
            vsetq_lane_u32((a & 0x2) ? all : none,
                vsetq_lane_u32((a & 0x1) ? all : none, vdupq_n_u32(0), 0),1));
}


static inline uint64x2_t simd_mask_and(uint64x2_t a, uint64x2_t b)
{
    return vandq_u64(a, b);
}


static inline uint64x2_t simd_mask_cond_lt(float64x2_t a, float64x2_t b)
{
    return vcltq_f64(a, b);
}

static inline float64x2_t simd_reciprocal(float64x2_t a)
{
    // Estimate the reciprocal
    float64x2_t reciprocal_estimate;
    reciprocal_estimate = vrecpeq_f64(a);

    // Compute the reciprocal correction
    float64x2_t reciprocal_correction;
    reciprocal_correction = vrecpsq_f64(reciprocal_estimate, a);
    // Multiply the estimate by the correction to improve accuracy
    float64x2_t reciprocal_result;
    reciprocal_result = vmulq_f64(reciprocal_estimate,
        reciprocal_correction);
    return reciprocal_result;
}

static inline double simd_incr_reduced_sum(
    double* m, float64x2_t v0, float64x2_t v1, float64x2_t v2, float64x2_t v3)
{
    // Sum all the resulting vectors
    float64x2_t sum = vaddq_f64(v0, v1);
    sum             = vaddq_f64(sum, v2);
    sum             = vaddq_f64(sum, v3);

    // Load existing values from memory and add to the sum
    float64x2_t mem = vld1q_f64(m);
    sum               = vaddq_f64(sum, mem);

    // Store the result back to memory
    vst1q_f64(m, sum);

    // Reduce the final vector to a single double
    float64x2_t sum_parts = vpaddq_f64(sum, sum); // Horizontal add
    double final_sum = vgetq_lane_f64(sum_parts, 0) + vgetq_lane_f64(sum_parts, 1); // Extract and sum lanes

    return final_sum;
}

static inline float64x2_t simd_masked_add(float64x2_t a, float64x2_t b, uint64x2_t m) {
    // Perform bitwise AND operation on the mask and vector b
    float64x2_t masked_b = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(b), m));
    
    // Add the result to vector a
    return vaddq_f64(a, masked_b);
}


static inline float64x2_t select_by_mask(float64x2_t a, uint64x2_t mask) {
    // Select elements from 'a' based on the mask, and set other elements to 0.0
    float64x2_t result;
    result = vbslq_f64(mask, a, vdupq_n_f64(0.0));
    
    return result;
}

static inline float64x2_t simd_load_h_dual(const double* m) {
    return (float64x2_t) vdupq_n_f64(0.0);
}

static inline float64x2_t simd_load_h_duplicate(const double* m) {
    double value = *m;
    return (float64x2_t) vdupq_n_f64(value);
}

static inline void simd_h_decr3(double* m, float64x2_t a0, float64x2_t a1, float64x2_t a2) {
    return;
}

static inline double simd_h_dual_incr_reduced_sum(double* m, float64x2_t v0, float64x2_t v1) {
    return 0.0;
}
