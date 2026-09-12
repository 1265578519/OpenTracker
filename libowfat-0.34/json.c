#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char* argv[]) {
  int i;
  printf("[");
  for (i=1; i<argc; ++i) {
    char* src=argv[i];
    char* dst=strrchr(src,'/');
    char dfn[100];	// enough for our use cases
    if (dst) ++dst; else dst=src;
    size_t j;
    for (j=0; j<90 && dst[j]; ++j) {
      if ((dfn[j]=dst[j])=='.') break;
    }
    dfn[j++]='.';
    dfn[j++]='o';
    dfn[j]=0;
    printf("\n{ \"directory\": \"@\", \"file\": \"%s\", \"output\": \"%s\", \"arguments\": [ \"clang\", \"-c\", \"%s\", \"-I.\" ]}%s ",src,dfn,src,(i+1<argc)?",":"");
  }
  printf("\n]\n");
}
