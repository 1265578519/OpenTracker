#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scan.h"
#include <assert.h>

#include "scan/scan_ulong.c"
#include "scan/scan_ulongn.c"
#include "fmt/fmt_utf8.c"
#include "fmt/fmt_tohex.c"
#include "fmt/fmt_escapecharc.c"

char tmp[20];
char tmp2[20];
size_t n,m;
unsigned long l;

struct entity {
  const char* entity;
  char utf8[10];
  struct entity* next;
}* root,** cur=&root;

struct letter {
  char c;
  struct letters* weiter;
  uint32_t marshaled;	// lower 8 bits: char. rest: ofs from start of marshaled blob
};

struct letters {
  size_t n;
  struct letter liste[256];
};

struct letters* d;
size_t nodes,datasize;

void addword(struct letters** s,const char* t, void* pointer) {
  size_t i;
  if (!*s) {
    *s=malloc(sizeof(**s));
    memset(*s,0,sizeof(**s));
    (*s)->liste[0].c='?';
  }
  i=(unsigned char)*t;
  if ((*s)->liste[i].c==*t) {
    if (!*t) {
      datasize+=strlen((char*)pointer)+1;
      (*s)->liste[i].weiter=pointer;
    } else
      addword(&(*s)->liste[i].weiter,t+1,pointer);
    return;
  }

  ++nodes;
  (*s)->n++;
  (*s)->liste[i].c=*t;
  if (!*t) {
    datasize+=strlen((char*)pointer)+1;
    (*s)->liste[i].weiter=pointer;
  } else {
    (*s)->liste[i].weiter=0;
    addword(&(*s)->liste[i].weiter,t+1,pointer);
  }
}

void dump(struct letters* s,size_t depth) {
  size_t i,j;
  if (!s) return;
  for (i=0; i<256; ++i) {
    if (s->liste[i].c!=i) continue;
    for (j=0; j<depth; ++j) printf("  ");
    printf("'%c' -> {\n",s->liste[i].c);
    if (s->liste[i].c)
      dump(s->liste[i].weiter,depth+1);
    for (j=0; j<depth; ++j) printf("  ");
    printf("}\n");
  }
}

size_t used;
size_t useddata;
char* heap;
uint32_t* marshaled;
char* data;

void marshalhelper(struct letters* s) {
  size_t i;
  uint32_t myindex=used;
  if (!s) return;
  used+=s->n;
  assert(used<nodes+2);
  for (i=1; i!=0; ++i) {	// start at 1, go to 256, then access modulo 256; effect: sort but put 0 last
    uint32_t x;
    i&=0xff;
//    printf("%c ",i);
    if (s->liste[i].c!=i) {
      if (i==0) return;
      continue;
    }
//    printf("marshalhelper: %c\n",i);
    x=(unsigned char)s->liste[i].c;
    if (!x) {
      size_t l=strlen((char*)s->liste[i].weiter)+1;
//      puts((char*)s->liste[i].weiter);
      x|=useddata<<8;
      assert(useddata+l<=datasize);
      memcpy(data+useddata,s->liste[i].weiter,l);
      useddata+=l;
      marshaled[++myindex]=x;
      return;
    } else {
      x|=(used+1)<<8;
      marshalhelper(s->liste[i].weiter);
    }
    marshaled[++myindex]=x;
  }
//  printf("return\n");
}

void marshal(struct letters* s) {
  fprintf(stderr,"nodes=%lu, datasize=%lu\n",nodes,datasize);
  heap=malloc((nodes+1)*sizeof(uint32_t)+datasize);
  if (!heap) return;
  marshaled=(uint32_t*)heap;
  marshaled[0]=nodes+1;
  data=heap+(nodes+1)*sizeof(uint32_t);
  marshalhelper(s);
  fprintf(stderr,"actually used: %lu nodes, %lu bytes data\n",used,useddata);
}

char* lookup(char* ds,size_t ofs,const char* t) {
  uint32_t* tab=(uint32_t*)ds;
  if (ofs>tab[0]) return 0;
  while (ofs<tab[0]) {
    unsigned char ch=tab[ofs]&0xff;
    if (ch==(unsigned char)*t) {
      if (!ch)
	return ds+tab[0]*sizeof(uint32_t)+(tab[ofs]>>8);
      else
	return lookup(ds,tab[ofs]>>8,t+1);
    } else
      ++ofs;
    if (!ch) break;
  }
  return NULL;
}

int main() {
  FILE* f=fopen("entities.json","r");
  char buf[256];
  if (!f) return 1;
#if 0
  puts("struct { const char* entity; const char* utf8; } codepoints[] = {");
#endif
  while (fgets(buf,sizeof(buf),f)) {
    char* s,* entity;
    size_t ul;
    if (!isspace(buf[0])) continue;
    for (s=buf; *s && *s!='"'; ++s) ;	// skip whitespace
    if (!(*s=='"')) continue;
    ++s;
    entity=s;
    if (*entity!='&') continue; ++entity; ++s;
    for (; *s && *s!='"'; ++s) ;	// skip to end of entity
    if (!(*s=='"')) continue;
    if (s[-1]!=';') continue;
    s[-1]=0; ++s;
    s=strchr(s,'[');
    if (!s) continue;
    n=0;
#if 0
    printf("  { \"%s\", \"",entity);
#endif
    ++s;
    *cur=malloc(sizeof(**cur));
    (*cur)->next=0;
    if (!((*cur)->entity=strdup(entity))) return 1;
    ul=0;
    do {
      while (isspace(*s)) ++s;
      m=scan_ulong(s,&l);
      if (!m) return 2;
      s+=n;
      n=fmt_utf8(tmp,l);
      if (ul+n>sizeof((*cur)->utf8)) return 3;
      memcpy((*cur)->utf8+ul,tmp,n);
      ul+=n;
#if 0
      {
	size_t i;
	for (i=0; i<n; ++i) {
	  fwrite(tmp2,fmt_escapecharc(tmp2,(unsigned char)tmp[i]),1,stdout);
	}
      }
#endif
      if (*s==']') break;
    } while (*s==',');
#if 0
    puts("\" },");
#endif
    addword(&d,(*cur)->entity,(*cur)->utf8);
  }
  fclose(f);
//  dump(d,0);
  marshal(d);
  {
    FILE* f=fopen("entities.h","w");
    size_t i;
    fprintf(f,"struct {\n  uint32_t tab[%u];\n  char data[%lu];\n} entities = {\n  {",marshaled[0],datasize);
    for (i=0; i<marshaled[0]; ++i) {
      if (i%8 == 0) fprintf(f,"\n    ");
      fprintf(f,"0x%x,",marshaled[i]);
    }
    fprintf(f,"\n  } , {");
    for (i=0; i<datasize; ++i) {
      if (i%16 == 0) fprintf(f,"\n    ");
      fprintf(f,"0x%x,",data[i]&0xff);
    }
    fprintf(f,"\n  }\n};");
    fclose(f);
  }
//  puts(lookup(heap,1,"zwnj"));
#if 0
  puts("};");
#endif
  return 0;
}
