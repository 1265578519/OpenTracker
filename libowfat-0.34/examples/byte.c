#include "byte.h"
#include "buffer.h"

/*
 * The functions that are beginning with byte_ offer some handy functions, to
 * manipulate with raw bytes.
 */

int
main()
{	/* The indices:     0123456 */
	char memory1[42] = "foo bar";
	char memory2[23] = "fnord";
	unsigned int pos;
	int diff;
	
	/* First we want to search forward for a certain char - we can achieve it
	 * by using:
	 *
	 * unsigned int  byte_chr(const  char  *haystack,unsigned int len,char
	 * needle);
	 *
	 * which returns the _first_ position of the searched char ("needle") or
	 * the supplied length ("len") if the search fails.
	 */

	pos = byte_chr(memory1, 7, 'b'); /* Returns 4. */

	buffer_puts(buffer_1, "byte_chr(memory1, 7, 'b'): ");
	buffer_putulong(buffer_1, pos);
	buffer_putnlflush(buffer_1);

	/* Now let's focus on the opposite: we want to search backward in a
	 * mem-region:
	 *
	 * unsigned int byte_rchr(const void* haystack,unsigned int len,char
	 * needle);
	 *
	 * now it returns the _last_ position of the "needle" or len.
	 */

	pos = byte_rchr(memory1, 7, 'o'); /* Returns 2.*/

	buffer_puts(buffer_1, "byte_rchr(memory1, 7, 'o'): ");
	buffer_putulong(buffer_1, pos);
	buffer_putnlflush(buffer_1);

	/* Enough of searching for now -- another important task is copying of
	 * memory. Of course, libowfat helps you also in this point:
	 *
	 * void byte_copy(void* out, unsigned int len, const void* in);
	 *
	 * It simply copies len bytes from in to out -- starting at in[0] and
	 * out[0].  Please recog that it has an another API than memcpy() that has
	 * the last two parameters swapped.
	 */

	byte_copy(memory1, 2, memory2); /* Makes memory1 look: "fno bar" */

	buffer_puts(buffer_1, "byte_copy(memory1, 2, memory2): ");
	buffer_puts(buffer_1, memory1);
	buffer_putnlflush(buffer_1);

	/* There is also a function byte_copyr() that does exactly the same except
	 * of starting at the _end_ of the strings, i.e. in[len-1] and out[len-1].
	 * I won't dicuss it in detail as only the internals has changed.
	 */

	/* Another point is the comparing between memory regions -- in the libowfat
	 * is
	 *
	 * int byte_diff(const void* a, unsigned int len, const void* b);
	 *
	 * the utility of choice. It returns 0 if the regions are equal, <0 if
	 * a[0]...a[len] is lexicographically smaller and >0 if it is greater than
	 * b. After the first difference is found, no further reading beyond this
	 * difference is done. Beware of checking only for 1 and -1!
	 */

	diff = byte_diff(memory1, 5, memory2);

	buffer_puts(buffer_1, "byte_diff(memory1, 5, memory2): ");
	buffer_putlong(buffer_1, diff);
	buffer_putnlflush(buffer_1);

	/* For convience, there is also a macro called byte_equal() for checking
	 * for equality -- in fact it is just a !byte_diff().
	 */

	/* Last but not feast a quite simple yet useful function that make it
	 * possible to ban the BSD-legacy-crap called bzero() from your code w/o
	 * having to use memset() that confused even Rich Stevens:
	 *
	 * void byte_zero(char *out,unsigned int len);
	 *
	 * fills the specified block of memory with 0.
	 */

	byte_zero(memory1, 42);

	return 0;
}
