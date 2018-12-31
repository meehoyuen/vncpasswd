#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "d3des.h"

static unsigned char d3desObfuscationKey[] = {23,82,107,6,35,78,88,7};

char *decrypt(char *ibuf, char *obuf) {
  if (strlen(ibuf) < 8)
  {
    fprintf(stderr, "bad password length\n");
	return NULLL;
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

  while (true) {
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
  fprintf(stderr,"       %s -f\n", prog);
  exit(1);
}

int main(int argc, char** argv)
{
  int encode = 0;
  char *fname = ".vnc/passwd";

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0) {
		encode = 1;
    } else if (strncmp(argv[i], "-f", 2) == 0) {
      return encrypt_pipe();
    } else if (argv[i][0] == '-') {
      usage(argv[0]);
    } else if (!fname) {
      fname = argv[i];
    } else {
      usage(argv[0]);
    }
  }

  if(encode) {
	char buf[9];
	char bufonly[9];
    char *obfuscated = readpassword(buf);
    char *obfuscatedReadOnly = NULL;

    fprintf(stderr, "Would you like to enter a view-only password (y/n)? ");
    char yesno[3];
    if (fgets(yesno, 3, stdin) != NULL && (yesno[0] == 'y' || yesno[0] == 'Y')) {
      obfuscatedReadOnly = readpassword(bufonly);
    }

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
    if (!fp) {
      fprintf(stderr,"Couldn't open %s for reading\n",fname);
      exit(1);
    }

    if (fread(buf, 8, 1, fp) != 1) {
      fprintf(stderr,"reading from %s failed\n",fname);
      exit(1);
    }
	if(!decrypt(ibuf, obuf))
	{
		fprintf(stderr,"decrypt failed\n");
		exit(1);
	}
	fprintf(stderr,"passwd:%8s\n",obuf);
    if (fwrite(buf, 8, 1, fp) != 1) {
        fprintf(stderr,"reading from %s failed\n",fname);
        exit(0);
      }
    }
	fprintf(stderr, "view-only password\n");
	if(!decrypt(ibuf, obuf))
	{
		fprintf(stderr,"decrypt failed\n");
		exit(1);
	}
	fprintf(stderr,"passwd:%8s\n",obuf);

    fclose(fp);
  }

  return 0;
}
