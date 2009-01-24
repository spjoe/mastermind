#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define MAXCOL 8 /* columns */
#define COLOURS	8
#define COLOURSHIFT 3
#define MAXPOS (1<<(COLOURSHIFT*MAXCOL))


typedef int Colour;
typedef int Position;

const int colours = 8;
int columns;
int maxpos;
double info;

inline static Colour col(int n, Position x)
{
  return ((x>>(3*n))&7);
}

inline static void printpos(Position p)
{
  int i;
  for(i=0; i<columns; i++)
    printf("%d", col(i,p)+1);
}
inline void eval(Position x, Position y, int *blackp, int *whitep)
{
  int blacks, whites, i,j;
  int used[MAXCOL] = {0}; /* which pegs of y have already been used for matching? */
  char colx[MAXCOL], coly[MAXCOL];

  for (i=0, blacks=0; i<columns; i++)
    if (col(i,x) == col(i,y)) {
      blacks++;
      used[i]=1;
    }
  for (i=0, whites=0; i<columns; i++)
    if (col(i,x) != col(i,y))
      for (j=0; j<columns; j++)
	if (!used[j])
	  if (col(i,x) == col(j,y)) {
	    whites++;
	    used[j]=1;
	    break;
	  }
  /* printf("eval: x="); printpos(x); printf(" y="); printpos(y); printf(" result=%d-%d\n",blacks, whites); */
  *blackp = blacks;
  *whitep = whites;
}


inline void eval5(Position x, Position y, int *blackp, int *whitep)
{
  const int columns = 5; 
  int blacks, whites, i,j;
  int used[MAXCOL] = {0}; /* which pegs of y have already been used for matching? */
  char colx[MAXCOL], coly[MAXCOL];


  for (i=0, blacks=0; i<columns; i++)
    if (col(i,x) == col(i,y)) {
      blacks++;
      used[i]=1;
    }
  for (i=0, whites=0; i<columns; i++)
    if (col(i,x) != col(i,y))
      for (j=0; j<columns; j++)
	if (!used[j])
	  if (col(i,x) == col(j,y)) {
	    whites++;
	    used[j]=1;
	    break;
	  }
  /* printf("eval: x="); printpos(x); printf(" y="); printpos(y); printf(" result=%d-%d\n",blacks, whites); */
  *blackp = blacks;
  *whitep = whites;
}

inline int reply(Position try, Position possible[], int npossible, int blacks, int whites)
{
  int i,j,b,w;

  for (i=0, j=0; i<npossible; i++) {
    if(columns == 5)
        eval5(try,possible[i],&b,&w);
    else
        eval(try,possible[i],&b,&w);
    if (b==blacks && w==whites)
      possible[j++]=possible[i];
  }
  return j;
}

inline void evalmove(Position try, Position possible[], int npossible, int in_possible, double *lengthp, double *posp, int *maxp)
{
  int counts[MAXCOL][MAXCOL] = {{0}};
  double value, length, sumsq;
  int i,j,b,w, max;

  for (i=0; i<npossible; i++) {
    if(columns == 5)
        eval5(try, possible[i], &b, &w);
    else
        eval(try, possible[i], &b, &w);
/*    printf("eval(%o,%o,%d-%d)\n",try,possible[i],b,w);*/
    counts[b][w]++;
  }
  value = 0.0;
  sumsq = 0.0;
  max = 0;
  for (i=0; i<MAXCOL; i++)
    for (j=0; j<MAXCOL; j++) {
      if (counts[i][j]!=0) {
	double c = counts[i][j];
/*	printf("%d-%d: %d;%lf;%lf\n",i,j,counts[i][j],c,log\(c));*/
	value += c*log(c);
	sumsq += c*c;
	max = counts[i][j]>max ? counts[i][j] : max;
      }
    }
  /* length is c*m, where m ist the number of moves still necessary */
  length = value/npossible+info;
  if (in_possible)
    length *= (npossible-1)/(double)npossible;
/*  printf("Value of %7o: %f moves, %f positions, %d max. positions\n",try,length,sumsq/npossible,max);*/
  *lengthp = length/info;
  *posp = sumsq/npossible;
  *maxp = max;
}

static int compint(const void *p1, const void *p2)
{
  return *(int *)p1 - *(int *)p2;
}

inline Position makemove(Position possible[], int npossible)
{
  Position p, bestlpos, bestwavpos, bestmaxpos;
  int bestlpos_possible, bestwavpos_possible, bestmaxpos_possible;
  double length, bestlength, wavpositions, bestwavpositions;
  int maxpositions, bestmaxpositions;

  bestlength = 2*log((double)npossible);
  bestlpos = -1;
  bestlpos_possible=0;
  bestwavpositions = npossible+1;
  bestwavpos = -1;
  bestwavpos_possible=0;
  bestmaxpositions = npossible+1;
  bestmaxpos = -1;
  bestmaxpos_possible=0;
  for (p=0; p<maxpos; p++) {
    int in_possible =
      bsearch(&p, possible, npossible, sizeof(possible[0]), compint)!=NULL;
    evalmove(p, possible, npossible, in_possible, &length, &wavpositions, &maxpositions);
    if (length<bestlength || (length == bestlength && !bestlpos_possible && in_possible)) {
      bestlength=length;
      bestlpos_possible = in_possible;
      bestlpos=p;
    }
    if (wavpositions<bestwavpositions || (wavpositions == bestwavpositions && !bestwavpos_possible && in_possible)) {
      bestwavpositions=wavpositions;
      bestwavpos_possible = in_possible;
      bestwavpos=p;
    }
    if (maxpositions<bestmaxpositions || (maxpositions == bestmaxpositions && !bestmaxpos_possible && in_possible)) {
      bestmaxpositions = maxpositions;
      bestmaxpos_possible = in_possible;
      bestmaxpos=p;
    }
  }
#if 0
  putchar('\n');
  printf("Best Length: "); printpos(bestlpos); printf("  Length: %f\n", bestlength);
  printf("Best WAvPos: "); printpos(bestlpos); printf("  Weighted average positions: %f\n", bestwavpositions);
  printf("Best MaxPos: "); printpos(bestlpos); printf(" Max. Positions: %d\n", bestmaxpositions);
#endif
  return bestlpos;
}

inline Position make_guess(int b, int w)
{
  static Position p,move;
  static Position possible[MAXPOS];
  static int npossible,b1,w1;
  static int guesses=0;

  if (guesses==0) {
    guesses++;
    npossible = maxpos;
    move = ((int []){00,010,0210,03210,033210,0332210,03322110,033221100})[columns-1];
  } else {
    if (npossible == maxpos) {
      for (p=0, npossible=0; p<maxpos; p++) {
        if(columns == 5)
	        eval5(move,p,&b1,&w1);
        else
            eval(move,p,&b1,&w1);
	    if (b==b1 && w==w1)
	        possible[npossible++] = p;
      }
    } else
      npossible=reply(move,possible, npossible,b,w);
#if 0
    printf("%d moves possible\n", npossible);
    for (i=0; i<npossible&&i<10; i++) {
      printf("Possible: "); printpos(possible[i]); printf("\n");
    }
#endif
    move = makemove(possible, npossible);
  }
  return move;
}

int main(int argc, char *argv[])
{
  int i, code, guess, b, w;
  char *s;
  if (argc!=2) {
  usage:
    fprintf(stderr, "usage: %s code\ncode consists of 1-8 digits with values 1-8\n", argv[0]);
    exit(1);
  }
  s = argv[1];
  columns = strlen(s);
  for (i=columns-1, code=0; i>=0; i--) {
    char c = s[i];
    if (c<'1' || c>('0'+COLOURS))
      goto usage;
    code = code * COLOURS + c - '1';
  }
  for (maxpos=1, i=0; i<columns; i++)
    maxpos *= colours;
  /* info is the average information gained in each move;
     it is the logarithm of the reduction factor of the number of
     possible positions; */
  info = log((double)columns+2);
  b=w=0; /* unused, but anyway */
  do {
    guess = make_guess(b,w);
    printf("Guess: "); printpos(guess);
    eval(guess,code,&b,&w);
    printf(" Result: %d black %d white\n",b,w);
  } while (guess!=code);
  return 0;
}
