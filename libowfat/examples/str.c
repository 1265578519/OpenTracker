#include "str.h"
#include "buffer.h"

/*
 * The str_* functions have a pretty similar functionality to the byte_* ones
 * -- the most important enhacement is the recognition of \0.
 */

int
main()
{
	char *string1 = "foo bar";
	char string2[42] = "fnord";
	unsigned int len;
	unsigned int amm;
	unsigned int pos;
	int diff;
	int start;

	/* For determinating the difference between two strings, thew libowfat
	 * offers two functions:
	 * 
	 * int str_diff(const char *a,const char *b);
	 *
	 * and
	 *
	 * int str_diffn(const char *a,const char *b,unsigned int limit);
	 *
	 * They both return <0 if a is lexicographically smaller than b, 0 if they
	 * are equal and >0 if b is greater than a. The only difference is that
	 * str_diff compares everytime until a \0 is encountered and str_diffn
	 * compares max. until limit. Beware of checking just for 1 and -1!
	 */

	diff = str_diff(string1, string2);
	
	buffer_puts(buffer_1, "str_diff(string1, string2): ");
	buffer_putlong(buffer_1, diff);
	buffer_putnlflush(buffer_1);


	/* As in byte_diff(), there is a macro str_equal(a,b) for convience. */

	/* For the frequent task of testing whether a string starts with an another
	 * is the following function:
	 *
	 * int str_start(const char *a,const char *b);
	 *
	 * It simply returns 1 if a starts with b and 0 otherwise.
	 */

	start = str_start(string1, "foo");

	buffer_puts(buffer_1, "str_start(string1, \"foo\"): ");
	buffer_putlong(buffer_1, start);
	buffer_putnlflush(buffer_1);
 

	/* Determinating the length of a string is also easy with the libowfat:
	 *
	 * unsigned int str_len(const char *s);
	 * 
	 * returns the index of \0 in the string and that's also the length.
	 */

	len = str_len(string1);

	buffer_puts(buffer_1, "str_len(buffer_1): ");
	buffer_putulong(buffer_1, len);
	buffer_putnlflush(buffer_1);


	/* One of the most used ANSI-C-functions is strcpy() and the libowfat has
	 * an own version:
	 *
	 * unsigned int str_copy(char *out,const char *in);
	 *
	 * It copies "in" into "out" until the first \0 is copied and returns the
	 * number of bytes copied.
	 */

	amm = str_copy(string2, string1); /* Returns 7, string2 now begins with "foo bar\0". */

	buffer_puts(buffer_1, "str_copy(string1, string2): ");
	buffer_putulong(buffer_1, amm);
	buffer_putnlflush(buffer_1);


	/* Similar to byte_* there are:
	 *  
	 * unsigned int str_chr(const char *haystack,char needle);
	 * 
	 * and
	 *
	 * unsigned int str_rchr(const char *haystack,char needle);
	 *
	 * The first one returns the index of the _first_ occurency of "needle" in
	 * "haystack" and the second returns the last one. Both return the index of
	 * \0 if the "needle" hasn't been found.
	 */

	pos = str_chr(string1, 'b'); /* Returns 4. */

	buffer_puts(buffer_1, "str_chr(string1, 'b'): ");
	buffer_putulong(buffer_1, pos);
	buffer_putnlflush(buffer_1);

	return 0;
}
