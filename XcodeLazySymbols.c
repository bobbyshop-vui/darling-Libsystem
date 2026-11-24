#include <math.h>
#include <stdint.h>
#include <stddef.h>

typedef float simd_float2 __attribute__((ext_vector_type(2)));
typedef float simd_float3 __attribute__((ext_vector_type(3), aligned(16)));
typedef float simd_float4 __attribute__((ext_vector_type(4)));
typedef double simd_double2 __attribute__((ext_vector_type(2)));
typedef double simd_double3 __attribute__((ext_vector_type(3), aligned(16)));
typedef double simd_double4 __attribute__((ext_vector_type(4)));

typedef struct { simd_float2 columns[2]; } simd_float2x2;
typedef struct { simd_float3 columns[3]; } simd_float3x3;
typedef struct { simd_float4 columns[4]; } simd_float4x4;
typedef struct { simd_double2 columns[2]; } simd_double2x2;
typedef struct { simd_double3 columns[3]; } simd_double3x3;
typedef struct { simd_double4 columns[4]; } simd_double4x4;

static int invert_matrix(const double input[4][4], double output[4][4], int n)
{
	double rows[4][8] = {{0}};
	for (int r = 0; r < n; ++r) {
		for (int c = 0; c < n; ++c) rows[r][c] = input[r][c];
		rows[r][n + r] = 1.0;
	}

	for (int c = 0; c < n; ++c) {
		int pivot = c;
		for (int r = c + 1; r < n; ++r)
			if (fabs(rows[r][c]) > fabs(rows[pivot][c])) pivot = r;
		if (rows[pivot][c] == 0.0 || !isfinite(rows[pivot][c])) return 0;
		if (pivot != c) {
			for (int k = 0; k < 2 * n; ++k) {
				double t = rows[c][k];
				rows[c][k] = rows[pivot][k];
				rows[pivot][k] = t;
			}
		}
		double divisor = rows[c][c];
		for (int k = 0; k < 2 * n; ++k) rows[c][k] /= divisor;
		for (int r = 0; r < n; ++r) {
			if (r == c) continue;
			double factor = rows[r][c];
			for (int k = 0; k < 2 * n; ++k) rows[r][k] -= factor * rows[c][k];
		}
	}

	for (int r = 0; r < n; ++r)
		for (int c = 0; c < n; ++c)
			output[r][c] = rows[r][n + c];
	return 1;
}

#define DEFINE_SIMD_INVERSE(NAME, TYPE, N, SCALAR) \
TYPE NAME(TYPE value) \
{ \
	double input[4][4] = {{0}}, output[4][4] = {{0}}; \
	for (int r = 0; r < N; ++r) \
		for (int c = 0; c < N; ++c) input[r][c] = (double)value.columns[c][r]; \
	TYPE result = {0}; \
	if (!invert_matrix(input, output, N)) { \
		for (int r = 0; r < N; ++r) \
			for (int c = 0; c < N; ++c) result.columns[c][r] = (SCALAR)NAN; \
		return result; \
	} \
	for (int r = 0; r < N; ++r) \
		for (int c = 0; c < N; ++c) result.columns[c][r] = (SCALAR)output[r][c]; \
	return result; \
}

DEFINE_SIMD_INVERSE(__invert_f2, simd_float2x2, 2, float)
DEFINE_SIMD_INVERSE(__invert_f3, simd_float3x3, 3, float)
DEFINE_SIMD_INVERSE(__invert_f4, simd_float4x4, 4, float)
DEFINE_SIMD_INVERSE(__invert_d2, simd_double2x2, 2, double)
DEFINE_SIMD_INVERSE(__invert_d3, simd_double3x3, 3, double)
DEFINE_SIMD_INVERSE(__invert_d4, simd_double4x4, 4, double)

double __tanpi(double x)
{
	return tan(x * 3.14159265358979323846264338327950288);
}

simd_float4 _simd_round_f4(simd_float4 value)
{
	return (simd_float4){ roundf(value.x), roundf(value.y), roundf(value.z), roundf(value.w) };
}

char *sandbox_extension_issue_file(const char *extension_class, const char *path, uint32_t flags)
{
	(void)extension_class;
	(void)path;
	(void)flags;
	return NULL;
}

char *sandbox_extension_issue_generic(const char *extension_class, uint32_t flags)
{
	(void)extension_class;
	(void)flags;
	return NULL;
}

int sandbox_extension_consume(const char *token)
{
	(void)token;
	return -1;
}

int sandbox_extension_release(int handle)
{
	(void)handle;
	return -1;
}
