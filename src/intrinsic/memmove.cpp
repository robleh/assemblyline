#include <al/intrinsic.hpp>
#include <cstdint>
// https://opensource.apple.com/source/network_cmds/network_cmds-481.20.1/unbound/compat/memmove.c.auto.html

#pragma function(memmove)
void* memmove(void* dest, const void* src, size_t n)
{
	uint8_t* from = (uint8_t*)src;
	uint8_t* to = (uint8_t*)dest;

	if (from == to || n == 0)
		return dest;
	if (to > from && to - from < (int)n) {
		/* to overlaps with from */
		/*  <from......>         */
		/*         <to........>  */
		/* copy in reverse, to avoid overwriting from */
		int i;
		for (i = n - 1; i >= 0; i--)
			to[i] = from[i];
		return dest;
	}
	if (from > to && from - to < (int)n) {
		/* to overlaps with from */
		/*        <from......>   */
		/*  <to........>         */
		/* copy forwards, to avoid overwriting from */
		size_t i;
		for (i = 0; i < n; i++)
			to[i] = from[i];
		return dest;
	}
	memcpy(dest, src, n);
	return dest;
}