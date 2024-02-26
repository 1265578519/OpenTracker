/* this header file comes from libowfat, http://www.fefe.de/libowfat/ */
#ifndef TEXTCODE_H
#define TEXTCODE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* These take len bytes from src and write them in encoded form to
 * dest (if dest != NULL), returning the number of bytes written. */

/* dietlibc defines these in sys/cdefs.h, which is included from stddef.h */
#ifndef __readmemsz__
#define __readmemsz__(a,b)
#endif

#ifndef __writemem__
#define __writemem__(a)
#endif

/* Needs len/3*4 bytes */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_uuencoded(char* dest,const char* src,size_t len);

/* Needs len/3*4 bytes */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_base64(char* dest,const char* src,size_t len);

__writemem__(1) __readmemsz__(2,3)
size_t fmt_base64url(char* dest,const char* src,size_t len);

/* Worst case: len*3 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_quotedprintable(char* dest,const char* src,size_t len);

/* Worst case: len*3 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_quotedprintable2(char* dest,const char* src,size_t len,const char* escapeme);

/* Worst case: len*3 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_urlencoded(char* dest,const char* src,size_t len);

/* Worst case: len*3 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_urlencoded2(char* dest,const char* src,size_t len,const char* escapeme);

/* Worst case: len*2 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_yenc(char* dest,const char* src,size_t len);

/* Needs len*2 bytes */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_hexdump(char* dest,const char* src,size_t len);

/* Change '<' to '&lt;' and '&' to '&amp;' and '\n' to '<br>'; worst case: len*5 */
/* This is meant for outputting text that goes between tags */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_html(char* dest,const char* src,size_t len);

/* Change '<' to '&lt;' and '&' to '&amp;' and '"' to '&quot;'; worst case: len*6 */
/* This is meant for outputting text that goes in a tag argument between double quotes*/
__writemem__(1) __readmemsz__(2,3)
size_t fmt_html_tagarg(char* dest,const char* src,size_t len);

/* Change '<' to '&lt;' and '&' to '&amp;'; worst case: len*5 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_xml(char* dest,const char* src,size_t len);

/* Change '\' to "\\", '\n' to "\n", ^A to "\x01" etc; worst case: len*4 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_cescape(char* dest,const char* src,size_t len);

/* Worst case: len*4 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_cescape2(char* dest,const char* src,size_t len,const char* escapeme);

/* Fold whitespace to '_'; this is great for writing fields with
 * white spaces to a log file and still allow awk to do log analysis */
/* Worst case: same size */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_foldwhitespace(char* dest,const char* src,size_t len);

/* Worst case: len*3 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_ldapescape(char* dest,const char* src,size_t len);

__writemem__(1) __readmemsz__(2,3)
size_t fmt_ldapescape2(char* dest,const char* src,size_t len,const char* escapeme);

/* Encode JSON string from UTF-8; will backslash-escape the bare minimum.
 * Invalid UTF-8 in input will output as valid UTF-8 for each byte
 * Worst case: len*6 */
__writemem__(1) __readmemsz__(2,3)
size_t fmt_jsonescape(char* dest,const char* src,size_t len);

__writemem__(1) __readmemsz__(2,3)
size_t fmt_base85(char* dest,const char* src,size_t len);

/* These read one line from src, decode it, and write the result to
 * dest.  The number of decoded bytes is written to destlen.  dest
 * should be able to hold strlen(src) bytes as a rule of thumb. */
size_t scan_uuencoded(const char* src,char* dest,size_t* destlen);
size_t scan_base64(const char* src,char* dest,size_t* destlen);
size_t scan_base64url(const char* src,char* dest,size_t* destlen);
size_t scan_quotedprintable(const char* src,char* dest,size_t* destlen);
size_t scan_urlencoded(const char* src,char* dest,size_t* destlen);
size_t scan_urlencoded2(const char* src,char* dest,size_t* destlen);
size_t scan_yenc(const char* src,char* dest,size_t* destlen);
size_t scan_hexdump(const char* src,char* dest,size_t* destlen);
/* decodes all html5-standardized &foo; escapes, and also
 * "<br>" to "\n" and "<p>" to "\n\n", leaves the rest of the tags alone */
size_t scan_html(const char* src,char* dest,size_t* destlen);
/* decodes all html5-standardized &foo; escapes, but leaves all tags
 * alone */
size_t scan_html_tagarg(const char* src,char* dest,size_t* destlen);
/* Decodes escaped C string text, turning e.g. \n into newline */
size_t scan_cescape(const char* src,char* dest,size_t* destlen);
/* Decodes escaped LDIF text, turning e.g. \5C into \ */
size_t scan_ldapescape(const char* src,char* dest,size_t* destlen);
/* Decodes escaped JSON strings (like \"\r\n\u0013), turns escaped
 * surrogate pairs into UTF-8. Expects input to be valid UTF-8. Ends at
 * \0 or unescaped double quote. */
size_t scan_jsonescape(const char* src,char* dest,size_t* destlen);

size_t scan_base85(const char* src,char* dest,size_t* destlen);

#ifdef STRALLOC_H
/* WARNING: these functions _append_ to the stralloc, not overwrite! */
/* stralloc wrappers; return 1 on success, 0 on failure */
/* arg 1 is one of the fmt_* functions from above */
int fmt_to_sa(size_t (*func)(char*,const char*,size_t),
	      stralloc* sa,const char* src,size_t len);

int fmt_to_sa2(size_t (*func)(char*,const char*,size_t,const char*),
	      stralloc* sa,const char* src,size_t len,const char* escapeme);

/* arg 1 is one of the scan_* functions from above */
/* return number of bytes scanned */
size_t scan_to_sa(size_t (*func)(const char*,char*,size_t*),
			 const char* src,stralloc* sa);

#define fmt_uuencoded_sa(sa,src,len) fmt_to_sa(fmt_uuencoded,sa,src,len)
#define fmt_base64_sa(sa,src,len) fmt_to_sa(fmt_base64,sa,src,len)
#define fmt_base64url_sa(sa,src,len) fmt_to_sa(fmt_base64url,sa,src,len)
#define fmt_quotedprintable_sa(sa,src,len) fmt_to_sa(fmt_quotedprintable,sa,src,len)
#define fmt_urlencoded_sa(sa,src,len) fmt_to_sa(fmt_urlencoded,sa,src,len)
#define fmt_yenc_sa(sa,src,len) fmt_to_sa(fmt_yenc,sa,src,len)
#define fmt_hexdump_sa(sa,src,len) fmt_to_sa(fmt_hexdump,sa,src,len)
#define fmt_html_sa(sa,src,len) fmt_to_sa(fmt_html,sa,src,len)
#define fmt_cescape_sa(sa,src,len) fmt_to_sa(fmt_cescape,sa,src,len)
#define fmt_ldapescape_sa(sa,src,len) fmt_to_sa(fmt_ldapescape,sa,src,len)
#define fmt_jsonescape_sa(sa,src,len) fmt_to_sa(fmt_jsonescape,sa,src,len)

#define fmt_quotedprintable2_sa(sa,src,len,escapeme) fmt_to_sa2(fmt_quotedprintable2,sa,src,len,escapeme)
#define fmt_urlencoded2_sa(sa,src,len,escapeme) fmt_to_sa2(fmt_urlencoded2,sa,src,len,escapeme)
#define fmt_cescape2_sa(sa,src,len,escapeme) fmt_to_sa2(fmt_cescape2,sa,src,len,escapeme)

#define scan_uuencoded_sa(src,sa) scan_to_sa(scan_uuencoded,src,sa)
#define scan_base64_sa(src,sa) scan_to_sa(scan_base64,src,sa)
#define scan_base64url_sa(src,sa) scan_to_sa(scan_base64url,src,sa)
#define scan_quotedprintable_sa(src,sa) scan_to_sa(scan_quotedprintable,src,sa)
#define scan_urlencoded_sa(src,sa) scan_to_sa(scan_urlencoded,src,sa)
#define scan_yenc_sa(src,sa) scan_to_sa(scan_yenc,src,sa)
#define scan_hexdump_sa(src,sa) scan_to_sa(scan_hexdump,src,sa)
#define scan_html_sa(src,sa) scan_to_sa(scan_html,src,sa)
#define scan_cescape_sa(src,sa) scan_to_sa(scan_cescape,src,sa)
#define scan_ldapescape_sa(src,sa) scan_to_sa(scan_ldapescape,src,sa)
#define scan_jsonescape_sa(src,sa) scan_to_sa(scan_jsonescape,src,sa)
#endif

#ifdef ARRAY_H
void fmt_to_array(size_t (*func)(char*,const char*,size_t),
		  array* a,const char* src,size_t len);

void fmt_tofrom_array(size_t (*func)(char*,const char*,size_t),
		      array* dest,array* src);

void fmt_to_array2(size_t (*func)(char*,const char*,size_t,const char*),
		  array* a,const char* src,size_t len,const char* escapeme);

void fmt_tofrom_array2(size_t (*func)(char*,const char*,size_t,const char*),
		      array* dest,array* src,const char* escapeme);

size_t scan_to_array(size_t (*func)(const char*,char*,size_t*),
			    const char* src,array* dest);

size_t scan_tofrom_array(size_t (*func)(const char*,char*,size_t*),
			        array* src,array* dest);
#endif

extern const char base64[64];
extern const char base64url[64];

#ifdef __cplusplus
}
#endif

#endif
