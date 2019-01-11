/* this is a uudecode that also handles yenc encoded data.  The encoded
 * messages are read from stdin.  The yenc decoder even tries to
 * reconstruct mutilated encodings.  One of my news servers has the
 * problem that it gets corrupted yenc data through one of the uplinks
 * that replace "^." with "^..".  This decoder will try reversing this
 * when decoding a part with broken crc. */
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "textcode.h"
#include "str.h"
#include "buffer.h"
#include "open.h"
#include "stralloc.h"
#include "scan.h"
#include "case.h"

static const uint32_t crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

uint32_t crc32(uint32_t crc, const char* buf, unsigned int len) {
  const unsigned char* b=(const unsigned char*)buf;
  crc = crc ^ 0xfffffffful;
  while (len) {
    crc = (crc >> 8) ^ crc_table[(crc & 0xff) ^ *b];
    ++b; --len;
  }
  return crc ^ 0xfffffffful;
}

int main(int argc,char* argv[]) {
  char buf[4096];
  char obuf[4096];
  buffer filein;
  buffer fileout;
  int fd=0;
  int ofd=-1;
  int found=0;
  char line[1000];	/* uuencoded lines can never be longer than 64 characters */
  int l;
  enum { BEFOREBEGIN, AFTERBEGIN, SKIPHEADER } state=BEFOREBEGIN;
  enum { UUDECODE, YENC, MIME } mode=UUDECODE;
  enum { NONE, BASE64, QP} mimeenc=NONE;
  char filename[1024];
  unsigned long fmode=0,lineno=0;
  unsigned long offset,endoffset,totalsize,linelen,part,reconstructed=0; /* used only for yenc */
  static stralloc yencpart;
  unsigned int crc;

  if (argc>1) {
    fd=open_read(argv[1]);
    if (fd<0) {
      buffer_puts(buffer_2,"error: could not open \"");
      buffer_puts(buffer_2,argv[1]);
      buffer_putsflush(buffer_2,"\"\n");
      return 1;
    }
  }
  buffer_init(&filein,read,fd,buf,sizeof buf);
  /* skip to "^begin " */
  for (;;) {
    if ((l=buffer_getline(&filein,line,(sizeof line)-1))==0 && line[l]!='\n') {
hiteof:
      if (state!=BEFOREBEGIN) {
	if (mode!=MIME) {
	  buffer_puts(buffer_1,"premature end of file in line ");
	  buffer_putulong(buffer_1,lineno);
	  buffer_putsflush(buffer_1,"!\n");
	}
	if (ofd>=0) {
	  buffer_flush(&fileout);
	  fchmod(ofd,fmode);
	  close(ofd);
	}
	++found;
      }
      if (!found)
	buffer_putsflush(buffer_2,"warning: hit end of file without finding any uuencoded data!\n");
      return 0;
    }
    ++lineno;
    if (l>0 && line[l-1]=='\r') --l;	/* kill DOS line endings */
    line[l]=0;
    if (str_start(line,"begin ")) {
      if (state!=BEFOREBEGIN) {
	buffer_puts(buffer_1,"new begin without previous end in line ");
	buffer_putulong(buffer_1,lineno);
	buffer_putsflush(buffer_1,"!\n");
	if (ofd>=0) {
	  buffer_flush(&fileout);
	  fchmod(ofd,fmode);
	  close(ofd);
	}
	++found;
      }
      state=BEFOREBEGIN;
      if (line[l=6+scan_8long(line+6,&fmode)]==' ' && fmode) {
	int i;
	++l;
	mode=UUDECODE;
foundfilename:
	if (line[l]=='"') {
	  ++l;
	  line[str_chr(line+l,'"')]=0;
	}
	if (line[l+(i=str_rchr(line+l,'/'))]) l+=i+1;
	while (line[l]=='.') ++l;
	if (line[l]) {
	  if (mode==YENC)
	    ofd=open_write(line+l);
	  else
	    ofd=open_excl(line+l);
	  if (ofd<0) {
	    buffer_puts(buffer_2,"error: could not create file \"");
	    buffer_puts(buffer_2,line+l);
	    buffer_putsflush(buffer_2,"\" (must not exist yet)\n");
	  } else {
	    if (mode!=YENC || part==1) {
	      if (mode==YENC)
		buffer_puts(buffer_2,"decoding yEnc file \"");
	      else
		buffer_puts(buffer_2,"uudecoding file \"");
	      buffer_puts(buffer_2,line+l);
	      buffer_putsflush(buffer_2,"\"\n");
	    }
	    state=AFTERBEGIN;
	    buffer_init(&fileout,write,ofd,obuf,sizeof obuf);
	    continue;
	  }
	}
      }
    } else if (str_equal(line,"end")) {
      if (ofd>=0) {
	buffer_flush(&fileout);
	fchmod(ofd,fmode);
	close(ofd);
	ofd=-1;
      }
      ++found;
      state=BEFOREBEGIN;
      continue;
    } else if (str_start(line,"Content-Disposition: ")) {
      char* c=strstr(line,"filename=");
      if (!c) {
	if ((l=buffer_getline(&filein,line,(sizeof line)-1))==0 && line[l]!='\n') goto hiteof;
	c=strstr(line,"filename=");
      }
      if (c) {
	mode=MIME;
	filename[0]=0;
	c+=9;
	if (*c=='"') {
	  char* d=strchr(c+1,'"');
	  if (d) {
	    *d=0;
	    strcpy(filename,c+1);
	  }
	}
	if (!filename[0]) {
	  strcpy(filename,c);
	  /* TODO: truncate at space */
	}
	if (state!=BEFOREBEGIN) {
	  if (ofd>=0) {
	    buffer_flush(&fileout);
	    fchmod(ofd,fmode);
	    close(ofd);
	    ofd=-1;
	  }
	  ++found;
	}
	fmode=0644;
      }
    } else if (str_start(line,"Content-Transfer-Encoding: ")) {
      if (case_starts(line+27,"base64"))
	mimeenc=BASE64;
      else if (case_starts(line+27,"quoted-printable"))
	mimeenc=QP;
      else if (case_starts(line+27,"7bit"))
	mimeenc=NONE; /* this is not an attachment */
      else {
	buffer_puts(buffer_1,"unknown encoding \"");
	buffer_puts(buffer_1,line+27);
	buffer_puts(buffer_1,"\" on line ");
	buffer_putulong(buffer_1,lineno);
	buffer_putsflush(buffer_1,".\n");
      }
    } else if (!line[0]) {
      /* empty line */
      if (ofd==-1 && filename[0]) {
	ofd=open_excl(filename);
	if (ofd<0) {
	  buffer_puts(buffer_2,"error: could not create file \"");
	  buffer_puts(buffer_2,filename);
	  buffer_putsflush(buffer_2,"\" (must not exist yet)\n");
	} else {
	  buffer_puts(buffer_2,"decoding MIME attachment \"");
	  buffer_puts(buffer_2,filename);
	  buffer_putsflush(buffer_2,"\"\n");
	  filename[0]=0;
	  state=AFTERBEGIN;
	  buffer_init(&fileout,write,ofd,obuf,sizeof obuf);
	  continue;
	}
      }
      if (state==AFTERBEGIN)
	state=SKIPHEADER;
      else if (state==SKIPHEADER)
	state=AFTERBEGIN;
    } else if (str_start(line,"=ybegin ")) {
      char* filename=strstr(line," name=");
      if (!filename) {
invalidybegin:
	buffer_puts(buffer_2,"invalid =ybegin at line ");
	buffer_putulong(buffer_2,lineno);
	buffer_putsflush(buffer_2,".\n");
	continue;
      }
      l=filename-line+6;
      if (!(filename=strstr(line," part="))) {
	part=1;
      } else if (filename[6+scan_ulong(filename+6,&part)] != ' ') goto invalidybegin;
      if (part==1) reconstructed=0;
      if (!(filename=strstr(line," size="))) goto invalidybegin;
      if (filename[6+scan_ulong(filename+6,&totalsize)] != ' ') goto invalidybegin;
      if (!(filename=strstr(line," line="))) goto invalidybegin;
      if (filename[6+scan_ulong(filename+6,&linelen)] != ' ') goto invalidybegin;
      mode=YENC;
      stralloc_copys(&yencpart,""); crc=0;
      goto foundfilename;
    } else if (str_start(line,"=ypart ")) {
      char* tmp=strstr(line," begin=");
      char c;
      if (!tmp) {
invalidpart:
	buffer_puts(buffer_2,"invalid =ypart at line ");
	buffer_putulong(buffer_2,lineno);
	buffer_putsflush(buffer_2,".\n");
	continue;
      }
      c=tmp[7+scan_ulong(tmp+7,&offset)];
      if (c!=' ' && c!=0) goto invalidpart;
      if (!(tmp=strstr(line," end="))) goto invalidpart;
      c=tmp[5+scan_ulong(tmp+5,&endoffset)];
      if (c!=' ' && c!=0) goto invalidpart;
      if (offset>0) --offset; --endoffset;
      if (endoffset<offset || endoffset>totalsize) goto invalidpart;
      lseek(ofd,offset,SEEK_SET);
      continue;
    } else if (str_start(line,"=yend")) {
      /* first try to decode it normally and see if the CRC matches */
      unsigned long i,wantedcrc,gotcrc;
      gotcrc=0;
      stralloc out;
      char* tmp=strstr(line," pcrc32=");

      if (tmp) {
	if (!scan_xlong(tmp+8,&wantedcrc))
	  goto invalidpart;
	wantedcrc &= 0xfffffffful;
	gotcrc=1;
      } else if (part==1) {
	tmp=strstr(line," crc32=");
	if (!tmp)
	  goto invalidpart;
	if (!scan_xlong(tmp+7,&wantedcrc))
	  goto invalidpart;
	wantedcrc &= 0xfffffffful;
	gotcrc=1;
	endoffset=totalsize;
      } else
	goto invalidpart;
      stralloc_init(&out);
      stralloc_0(&yencpart);
      stralloc_ready(&out,yencpart.len);
      for (i=0; i<yencpart.len; ) {
	unsigned long x,scanned;
	x=scan_yenc(yencpart.s+i,out.s+out.len,&scanned);
	i+=x+1; out.len+=scanned;
      }
      i=crc32(0,out.s,out.len);
      if (endoffset == offset+out.len-1) ++endoffset;
      if (out.len == endoffset-offset && i == wantedcrc) {
	/* ok, save block */
	if (buffer_put(&fileout,out.s,out.len)) {
writeerror:
	  buffer_putmflush(buffer_1,"write error: ",strerror(errno),"\n");
	  return 1;
	}
      } else {
	out.len=0;
	for (i=0; i<yencpart.len; ) {
	  unsigned long x,scanned;
	  if (yencpart.s[i]=='.' && yencpart.s[i+1]=='.') ++i;
	  x=scan_yenc(yencpart.s+i,out.s+out.len,&scanned);
	  i+=x+1; out.len+=scanned;
	}
	i=crc32(0,out.s,out.len);
	if (!gotcrc) wantedcrc=i;
	if (out.len == endoffset-offset && i == wantedcrc) {
	  if (buffer_put(&fileout,out.s,out.len)) goto writeerror;
	  ++reconstructed;
	} else {
	  buffer_puts(buffer_2,"warning: part ");
	  buffer_putulong(buffer_2,part);
	  buffer_putsflush(buffer_2," corrupt; reconstruction failed.\n");
	  buffer_puts(buffer_2,"  -> ");
	  buffer_putulong(buffer_2,offset);
	  buffer_puts(buffer_2,"-");
	  buffer_putulong(buffer_2,endoffset);
	  buffer_puts(buffer_2," (want crc ");
	  buffer_putxlong(buffer_2,wantedcrc);
	  buffer_puts(buffer_2,", got crc ");
	  buffer_putxlong(buffer_2,i);
	  buffer_putsflush(buffer_2,")\n");
	  if (buffer_put(&fileout,out.s,out.len)) goto writeerror;
	}
      }
      stralloc_free(&out);
      if (buffer_flush(&fileout) || close(ofd)) goto writeerror;
      ofd=-1;
      if (endoffset==totalsize && reconstructed) {
	buffer_puts(buffer_2,"warning: had to reconstruct ");
	buffer_putulong(buffer_2,reconstructed);
	buffer_putsflush(buffer_2," parts!\n");
      }
      ++found;
      state=BEFOREBEGIN;
      continue;
    } else {
      unsigned long scanned,x=0;
      char tmp[1000];
      switch (mode) {
      case MIME:
	switch (mimeenc) {
	case BASE64: x=scan_base64(line,tmp,&scanned); break;
	case QP: x=scan_quotedprintable(line,tmp,&scanned); break;
	default:
		 break;
	}
	if (line[x]) x=0;
	break;
      case UUDECODE:
	x=scan_uuencoded(line,tmp,&scanned);
	break;
      case YENC:
	stralloc_cats(&yencpart,line);
	stralloc_cats(&yencpart,"\n");
	continue;
      }
      if (!x) {
	if (state==AFTERBEGIN) {
	  buffer_puts(buffer_1,"parse error in line ");
	  buffer_putulong(buffer_1,lineno);
	  buffer_puts(buffer_1,": \"");
	  buffer_puts(buffer_1,line);
	  buffer_putsflush(buffer_1,"\"\n");
	  state=SKIPHEADER;
	}
      } else {
	if (ofd>=0)
	  if (buffer_put(&fileout,tmp,scanned)) goto writeerror;
      }
    }
  }
}


