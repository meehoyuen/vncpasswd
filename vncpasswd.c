#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "d3des.h"

static unsigned char d3desObfuscationKey[] = {23,82,107,6,35,78,88,7};

char *decrypt(char *ibuf, char *obuf) {
  if (strlen(ibuf) < 8)
  {
    fprintf(stderr, "bad password length\n");
	return NULL;
  }
  deskey(d3desObfuscationKey, DE1);
  des(ibuf, obuf);
  obuf[8] = 0;
  return obuf;
}
   
char *encrypt(char *ibuf, char *obuf) {
  int i = strlen(ibuf);

  for (; i<8; i++)
    ibuf[i] = 0;

  deskey(d3desObfuscationKey, EN0);
  des(ibuf, obuf);
  return obuf;
}

static char* readpassword(char *buf) {
  char ibuf[9]={0};
  char tbuf[9]={0};

  while (1) {
    printf("Password:\n");
    scanf("%8s",ibuf);
    if (strlen(ibuf) < 6) {
      if (strlen(ibuf) == 0) {
        fprintf(stderr,"Password not changed\n");
        exit(1);
      }
      fprintf(stderr,"Password must be at least 6 characters - try again\n");
      continue;
    }

    printf("Verify:\n");
    scanf("%8s",tbuf);
    if (strcmp(ibuf, tbuf) != 0) {
      fprintf(stderr,"Passwords don't match - try again\n");
      continue;
    }

    return encrypt(ibuf, buf);
  }
}

static void usage(char *prog)
{
  fprintf(stderr,"usage: %s [file]\n", prog);
  exit(1);
}

int main(int argc, char** argv)
{
  int encode = 0;
  int i = 0;
  char *fname = "/home/shiyanlou/.vnc/passwd";

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0) {
      encode = 1;
    } else {
      fname = argv[i];
    }
  }

  if(encode)
  {
    char ch;
    char buf[9];
    char bufonly[9];
    char *obfuscated = readpassword(buf);
    char *obfuscatedReadOnly = NULL;

    if(!obfuscated)
    {
      exit(1);
    }
    fprintf(stderr, "enter a view-only password?[y/n]\n");
    if(scanf("%c%c", &ch,&ch)==2 && ch=='y')
      obfuscatedReadOnly = readpassword(bufonly);

    FILE* fp = fopen(fname,"w");
    if (!fp) {
      fprintf(stderr,"Couldn't open %s for writing\n",fname);
      exit(1);
    }
    chmod(fname, S_IRUSR|S_IWUSR);

    if (fwrite(obfuscated, 8, 1, fp) != 1) {
      fprintf(stderr,"Writing to %s failed\n",fname);
      exit(1);
    }

    if (obfuscatedReadOnly) {
      if (fwrite(obfuscatedReadOnly, 8, 1, fp) != 1) {
        fprintf(stderr,"Writing to %s failed\n",fname);
        exit(1);
      }
    }

    fclose(fp);
  }
  else
  {
    char ibuf[9];
    char obuf[9];

    FILE* fp = fopen(fname,"r");
    if (!fp)
    {
       fprintf(stderr,"Couldn't open %s for reading\n",fname);
       exit(1);
    }

    if (fread(ibuf, 8, 1, fp) != 1) {
       fprintf(stderr,"reading from %s failed\n",fname);
       exit(1);
    }
    if(!decrypt(ibuf, obuf))
    {
	fprintf(stderr,"decrypt failed\n");
	exit(1);
    }
    fprintf(stderr,"passwd:%-8s\n",obuf);
    if (fread(ibuf, 1, 1, fp) != 1)
    {
        exit(0);
    }
    if (fread(ibuf+1, 7, 1, fp) != 1)
    {
        fprintf(stderr,"reading view-only passwd from %s failed\n",fname);
        exit(1);
    }

    if(!decrypt(ibuf, obuf))
    {
	fprintf(stderr,"decrypt view-only passwd failed\n");
	exit(1);
    }
    fprintf(stderr,"view-only passwd:%-8s\n",obuf);

    fclose(fp);
  }

  return 0;
}
