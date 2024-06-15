#include <arm_neon.h>

// Typedefs for MD_SIMD_FLOAT and MD_SIMD_MASK for double precision
typedef float64x2_t MD_SIMD_FLOAT;
typedef uint64x2_t MD_SIMD_MASK;

// Broadcasting a scalar value to a SIMD vector
static inline MD_SIMD_FLOAT simd_broadcast(double value) { return vdupq_n_f64(value); }

// Zeroing a SIMD vector
static inline MD_SIMD_FLOAT simd_zero(void) { return vdupq_n_f64(0.0); }

// Loading data into a SIMD vector
static inline MD_SIMD_FLOAT simd_load(const double* ptr) { return vld1q_f64(ptr); }

// Storing data from a SIMD vector
static inline void simd_store(double* ptr, MD_SIMD_FLOAT vec) { vst1q_f64(ptr, vec); }

// Adding two SIMD vectors
static inline MD_SIMD_FLOAT simd_add(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b) { return vaddq_f64(a, b); }

// Multiplying two SIMD vectors
static inline MD_SIMD_FLOAT simd_mul(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b) { return vmulq_f64(a, b); }

// Fused multiply-add operation
static inline MD_SIMD_FLOAT simd_fma(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_FLOAT c)
{
    return vfmaq_f64(c, a, b);
}

static inline MD_SIMD_FLOAT simd_mask_from_u32(uint32_t a)
{
    const uint64_t all  = 0xFFFFFFFFFFFFFFFF;
    const uint64_t none = 0x0;
    return vreinterpretq_f64_u64(vsetq_lane_u64((a & 0x2) ? all : none,
        vsetq_lane_u64((a & 0x1) ? all : none, vdupq_n_u64(0), 0),
        1));
}

static inline float64x2_t simd_mask_and(float64x2_t a, float64x2_t b) {
    return vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(a), vreinterpretq_u64_f64(b)));
}

static inline float64x2_t simd_mask_cond_lt(float64x2_t a, float64x2_t b) {
    return vreinterpretq_f64_u64(vcltq_f64(a, b));
}

static inline float64x2_t simd_reciprocal(float64x2_t a) {
    float64x2_t reciprocal = vrecpeq_f64(a);
    reciprocal = vmulq_f64(reciprocal, vrecpsq_f64(reciprocal, a));
    return reciprocal;
}

static inline double simd_incr_reduced_sum(double *m, float64x2_t v0, float64x2_t v1, float64x2_t v2, float64x2_t v3) {
    // // Horizontal add pairs within each vector
    // float64x2_t sum0 = vpaddq_f64(v0, v1); // Add pairs in v0 and v1
    // float64x2_t sum1 = vpaddq_f64(v2, v3); // Add pairs in v2 and v3

    // Combine the two resulting vectors
    float64x2_t sum = vpaddq_f64(v0, v1); // Add pairs in sum0 and sum1

    // Load existing values from memory and add to the sum
    float64x2_t mem = vld1q_f64(m);
    sum = vaddq_f64(sum, mem);

    // Store the result back to memory
    vst1q_f64(m, sum);

    // Reduce the final vector to a single double
    float64x1_t sum_low = vget_low_f64(sum);
    float64x1_t sum_high = vget_high_f64(sum);
    sum_low = vadd_f64(sum_low, sum_high); // Add high and low parts

    return vget_lane_f64(sum_low, 0); // Extract the final sum
}

static inline float64x2_t simd_masked_add(float64x2_t a, float64x2_t b, uint64x2_t m) {
    // Perform bitwise AND operation on the mask and vector b
    float64x2_t masked_b = vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(b), m));
    // Add the result to vector a
    return vaddq_f64(a, masked_b);
}
// // Reciprocal approximation
// MD_SIMD_FLOAT simd_reciprocal(MD_SIMD_FLOAT a) {
//     return vrecpeq_f32(a);
// }

// // Mask for comparison (less than)
// MD_SIMD_MASK simd_mask_cond_lt(MD_SIMD_FLOAT a, MD_SIMD_FLOAT b) {
//     return vcltq_f32(a, b);
// }

// // Logical AND operation on masks
// MD_SIMD_MASK simd_mask_and(MD_SIMD_MASK a, MD_SIMD_MASK b) {
//     return vandq_u32(a, b);
// }

// Select elements based on mask
MD_SIMD_FLOAT select_by_mask(MD_SIMD_FLOAT a, MD_SIMD_MASK mask) {
    return vbslq_f32(mask, a, vdupq_n_f32(0.0f));
}

// // Mask creation from unsigned int
// MD_SIMD_MASK simd_mask_from_u32(unsigned int value) {
//     return vdupq_n_u32(value);
// }

// // Increment reduced sum
// void simd_incr_reduced_sum(float* ptr, MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_FLOAT
// c, MD_SIMD_FLOAT d) {
//     MD_SIMD_FLOAT sum = vaddq_f32(vaddq_f32(a, b), vaddq_f32(c, d));
//     float result[4];
//     vst1q_f32(result, sum);
//     for (int i = 0; i < 4; i++) {
//         ptr[i] += result[i];
//     }
// }
// Function to load and broadcast a single double
static inline float64x2_t simd_load_h_duplicate(const double* m)
{
    return vld1q_dup_f64(m);
}

// Function to load and broadcast two doubles and combine them
static inline float64x2_t simd_load_h_dual(const double* m)
{
    float64x1_t t0, t1;
    t0 = vld1_dup_f64(m);     // Load and duplicate the first double
    t1 = vld1_dup_f64(m + 1); // Load and duplicate the second double
    return vcombine_f64(t0, t1);
}

// static inline MD_SIMD_FLOAT simd_masked_add(
//     MD_SIMD_FLOAT a, MD_SIMD_FLOAT b, MD_SIMD_MASK m)
// {
//     return vdupq_n_f32(0.0f);
// }

static inline void simd_h_decr3(
    MD_FLOAT* m, MD_SIMD_FLOAT a0, MD_SIMD_FLOAT a1, MD_SIMD_FLOAT a2)
{
    return;
}

static inline MD_FLOAT simd_h_dual_incr_reduced_sum(
    MD_FLOAT* m, MD_SIMD_FLOAT v0, MD_SIMD_FLOAT v1)
{
    return 0.0f;
}