#include <arm_acle.h>
#include <arm_neon.h>

// Typedefs for MD_SIMD_FLOAT and MD_SIMD_MASK for double precision
typedef float64x2x2_t MD_SIMD_FLOAT;
typedef uint64x2x2_t MD_SIMD_MASK;

static inline float64x2x2_t simd_broadcast(double value)
{
    // Create a vector with the broadcasted value
    float64x2_t v = vdupq_n_f64(value);

    // Return the value replicated across both parts of the float64x2x2_t structure
    return (float64x2x2_t) { v, v };
}

static inline float64x2x2_t simd_zero(void)
{
    // Create a vector with the broadcasted value
    float64x2_t v = vdupq_n_f64(0.0);

    // Return the value replicated across both parts of the float64x2x2_t structure
    return (float64x2x2_t) { v, v };
}

// Helper function to subtract two float64x2x2_t vectors
static inline float64x2x2_t simd_sub(float64x2x2_t a, float64x2x2_t b)
{
    float64x2x2_t result;
    result.val[0] = vsubq_f64(a.val[0], b.val[0]);
    result.val[1] = vsubq_f64(a.val[1], b.val[1]);
    return result;
}

static inline float64x2x2_t simd_load(const double* ptr)
{
    // Load two 128-bit vectors in one instruction
    return vld1q_f64_x2(ptr);
}

static inline void simd_store(double* ptr, float64x2x2_t vec)
{
    // Store the first 128-bit vector (first two doubles)
    vst1q_f64_x2(ptr, vec);
}

static inline float64x2x2_t simd_add(float64x2x2_t a, float64x2x2_t b)
{
    float64x2x2_t result;
    result.val[0] = vaddq_f64(a.val[0], b.val[0]);
    result.val[1] = vaddq_f64(a.val[1], b.val[1]);
    return result;
}

static inline float64x2x2_t simd_mul(float64x2x2_t a, float64x2x2_t b)
{
    float64x2x2_t result;
    result.val[0] = vmulq_f64(a.val[0], b.val[0]);
    result.val[1] = vmulq_f64(a.val[1], b.val[1]);
    return result;
}

static inline float64x2x2_t simd_fma(float64x2x2_t a, float64x2x2_t b, float64x2x2_t c)
{
    float64x2x2_t result;
    result.val[0] = vfmaq_f64(c.val[0], a.val[0], b.val[0]);
    result.val[1] = vfmaq_f64(c.val[1], a.val[1], b.val[1]);
    return result;
}

static inline uint64x2x2_t simd_mask_from_u32(uint32_t a)
{
    const uint32_t all  = 0xFFFFFFFF;
    const uint32_t none = 0x0;
    uint64x2_t mask     = vreinterpretq_u64_u32(vdupq_n_u32((a & 0x8) ? all : none));
    mask                = vsetq_lane_u64((a & 0x4) ? 0xFFFFFFFFFFFFFFFFULL : 0x0ULL, mask, 1);
    uint64x2x2_t result;
    result.val[0] = mask;
    result.val[1] = mask;
    return result;
}


static inline uint64x2x2_t simd_mask_and(uint64x2x2_t a, uint64x2x2_t b)
{
    uint64x2_t result0 = vandq_u64(a.val[0], b.val[0]);
    uint64x2_t result1 = vandq_u64(a.val[1], b.val[1]);
    uint64x2x2_t result;
    result.val[0] = result0;
    result.val[1] = result1;
    return result;
}


static inline uint64x2x2_t simd_mask_cond_lt(float64x2x2_t a, float64x2x2_t b)
{
    uint64x2_t mask0 = vcltq_f64(a.val[0], b.val[0]);
    uint64x2_t mask1 = vcltq_f64(a.val[1], b.val[1]);
    uint64x2x2_t result;
    result.val[0] = mask0;
    result.val[1] = mask1;
    return result;
}

static inline float64x2x2_t simd_reciprocal(float64x2x2_t a)
{
    // Estimate the reciprocal
    float64x2x2_t reciprocal_estimate;
    reciprocal_estimate.val[0] = vrecpeq_f64(a.val[0]);
    reciprocal_estimate.val[1] = vrecpeq_f64(a.val[1]);

    // Compute the reciprocal correction
    float64x2x2_t reciprocal_correction;
    reciprocal_correction.val[0] = vrecpsq_f64(reciprocal_estimate.val[0], a.val[0]);
    reciprocal_correction.val[1] = vrecpsq_f64(reciprocal_estimate.val[1], a.val[1]);

    // Multiply the estimate by the correction to improve accuracy
    float64x2x2_t reciprocal_result;
    reciprocal_result.val[0] = vmulq_f64(reciprocal_estimate.val[0],
        reciprocal_correction.val[0]);
    reciprocal_result.val[1] = vmulq_f64(reciprocal_estimate.val[1],
        reciprocal_correction.val[1]);

    return reciprocal_result;
}

#include <arm_neon.h>

static inline double simd_incr_reduced_sum(
    double* m, float64x2x2_t v0, float64x2x2_t v1, float64x2x2_t v2, float64x2x2_t v3)
{
    // Horizontal add pairs within each vector
    float64x2_t sum0 = vpaddq_f64(v0.val[0], v0.val[1]); // Add pairs in v0
    float64x2_t sum1 = vpaddq_f64(v1.val[0], v1.val[1]); // Add pairs in v1
    float64x2_t sum2 = vpaddq_f64(v2.val[0], v2.val[1]); // Add pairs in v2
    float64x2_t sum3 = vpaddq_f64(v3.val[0], v3.val[1]); // Add pairs in v3

    // Sum all the resulting vectors
    float64x2_t sum = vaddq_f64(sum0, sum1);
    sum             = vaddq_f64(sum, sum2);
    sum             = vaddq_f64(sum, sum3);

    // Load existing values from memory and add to the sum
    float64x2x2_t mem = vld1q_f64_x2(m);
    sum               = vaddq_f64(sum, mem.val[0]);
    sum               = vaddq_f64(sum, mem.val[1]);

    // Store the result back to memory
    vst1q_f64_x2(m, mem);

    // Reduce the final vector to a single double
    float64x2_t sum_parts = vpaddq_f64(sum, sum); // Horizontal add
    double final_sum = vgetq_lane_f64(sum_parts, 0) + vgetq_lane_f64(sum_parts, 1); // Extract and sum lanes

    return final_sum;
}

static inline float64x2x2_t simd_masked_add(float64x2x2_t a, float64x2x2_t b, uint64x2x2_t m) {
    // Perform bitwise AND operation on the mask and vector b
    float64x2x2_t masked_b;
    masked_b.val[0] = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(b.val[0]), m.val[0]));
    masked_b.val[1] = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(b.val[1]), m.val[1]));
    
    // Add the result to vector a
    float64x2x2_t result;
    result.val[0] = vaddq_f64(a.val[0], masked_b.val[0]);
    result.val[1] = vaddq_f64(a.val[1], masked_b.val[1]);

    return result;
}


static inline float64x2x2_t select_by_mask(float64x2x2_t a, uint64x2x2_t mask) {
    // Select elements from 'a' based on the mask, and set other elements to 0.0
    float64x2x2_t result;
    result.val[0] = vbslq_f64(mask.val[0], a.val[0], vdupq_n_f64(0.0));
    result.val[1] = vbslq_f64(mask.val[1], a.val[1], vdupq_n_f64(0.0));
    
    return result;
}

static inline float64x2x2_t simd_load_h_dual(const double* m) {
    return (float64x2x2_t){vdupq_n_f64(0.0), vdupq_n_f64(0.0)};
}

static inline float64x2x2_t simd_load_h_duplicate(const double* m) {
    double value = *m;
    return (float64x2x2_t){vdupq_n_f64(value), vdupq_n_f64(value)};
}

static inline void simd_h_decr3(double* m, float64x2x2_t a0, float64x2x2_t a1, float64x2x2_t a2) {
    return;
}

static inline double simd_h_dual_incr_reduced_sum(double* m, float64x2x2_t v0, float64x2x2_t v1) {
    return 0.0;
}
