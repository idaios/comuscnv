/*  microsat.c:   This function takes ms output and converts it is microsatellite
  length variation data.  The output has on each line the set of lengths
  of the nsam individuals (relative to the ancestral length).
  Example usage:   ms 10 5 -t 4.0 | microsat > msat.dat
  To compile:  gcc -o microsat microsat.c rand1.c -lm   
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXTRIES 200000


int maxsites = 1000 ;
//double ran1() ;


int
biggerlist(nsam, nmax, list )
     int nsam ;
     unsigned nmax ;
     char ** list ;
{
  int i;
  
  maxsites = nmax  ;
  for( i=0; i<nsam; i++){
    list[i] = (char *)realloc( list[i],maxsites*sizeof(char) ) ;
    if( list[i] == NULL ) perror( "realloc error. bigger");
  }
}                        



void sortIndexes(double *a, int *inds, int len){

  double *b = calloc(len, sizeof(double));
  int i,j, k, maxk;
  double max = 0;

  for( i = 0; i < len; ++i)
    {
      b[i] = a[i];
      inds[i] = i;
    }
  
  for(i = 0; i < len; ++i){
    max = b[i];
    maxk = inds[i];
    k = i;
    for(j = i; j < len; ++j){
      if( max < b[j] )
	{
	  max = b[j];
	  maxk = inds[j];
	  k = j;
	}
    }
    b[k] = b[i];
    b[i] = max;
    inds[k] = inds[i];
    inds[i] = maxk;
  }
  //fprintf(stderr, "\n");
  
  free(b);
}


void help(){
  fprintf(stderr, "microsat [v3.0] February 2018\n\n");
  fprintf(stderr, "-seed <int1 (0)> <int2 (0)> <int3 (0)>\n");
  fprintf(stderr, "-initialcopies <int>: the number of copies in the MRCA\n");
  fprintf(stderr, "-ms <file>: the ms file\n");
  fprintf(stderr, "-mutationtimes <file>: the file with the time points that mutations occurred. It is generated by comus3 and it is the file mutationTimes.txt\n");
  fprintf(stderr, "-samplesize <int>: the sample size of comus3 (total)\n");
  fprintf(stderr, "-replications <int>: the number of replications, i.e., the number of comus datasets\n");
  fprintf(stderr, "\n-------\n\n");
  exit(1);
  
}

int main(int argc, char** argv)
{
  int nsam = 0, j ,nsites, i,  howmany=0, ntries = MAXTRIES ;
	char **list, **cmatrix(), allele,na, line[10001], tline[10001]  ;
	FILE *pf, *fopen(), *pfin, *tfin ;
	double *posit, *times   ;
	int* indexes;
	int   segsites, count  , nadv, probflag  ;
	double prob ;
	char dum[20], astr[100] ;
	int *nrepeats, step, ind, initialCopies = 0 ;

	char mutationTimesFile[1000];
	mutationTimesFile[0] = '\0';
	char inputFile[1000];
	inputFile[0] = '\0';

	unsigned short int myseed[3];
	for(i = 0; i < 3; ++i)
	  myseed[i] = 0;

	if(argc < 2)
	  help();
	
	for( i = 1; i < argc; ++i){

	  if( strcmp( argv[i], "-seed") == 0){
	    myseed[0] = atoi(argv[++i]);
	    myseed[1] = atoi(argv[++i]);
	    myseed[2] = atoi(argv[++i]);
	    continue;
	  }
	  
	  if( strcmp( argv[i], "-initialcopies") == 0 ){
	    initialCopies = atoi(argv[++i]);
	    continue;
	  }

	  if( strcmp( argv[i], "-ms") == 0){
	    strcpy(inputFile, argv[++i]);
	    continue;
	  }

	  if( strcmp( argv[i], "-mutationtimes") ==  0){
	    strcpy(mutationTimesFile, argv[++i]);
	    continue;
	  }
	  if(strcmp( argv[i], "-samplesize") == 0){
	    nsam = atoi(argv[++i]);
	    continue;
	  }

	  if(strcmp(argv[i], "-replications") == 0){
	    howmany = atoi(argv[++i]);
	    continue;
	  }
				     
	  
	  
	  fprintf(stderr, "Argument %s is invalid\n", argv[i]);
	  exit(0);
	  
	  
	}

	seed48(myseed);
	
	if( howmany == 0 || nsam == 0 ){
	  fprintf(stderr, "Please provide the sample size and the number of datasets in the file\n");
	  exit(0);
	}
	
/* read in first two lines of output  (parameters and seed) */
	if( inputFile[0] == 0 )
	  pfin = stdin ;
	else
	  pfin = fopen(inputFile, "r");

	if( mutationTimesFile[0] != 0 )
	  tfin = fopen(mutationTimesFile, "r");
	else
	  {
	    fprintf(stderr, "please provide a file with times\n");
	    exit(0);
	  }
	
	
	//fgets( line, 1000, pfin);
	//sscanf(line," %s  %d %d", dum,  &nsam, &howmany);
	//fgets( line, 1000, pfin);

	/* if( argc > 1 ) {  */
	/*    nadv = atoi( argv[1] ) ;  */
	/* } */

  list = cmatrix(nsam,maxsites+1);
  posit = (double *)malloc( maxsites*sizeof( double ) ) ;
  times = calloc(maxsites, sizeof(double));
  indexes = calloc(maxsites, sizeof(int));
  nrepeats = (int *)malloc(nsam*sizeof(int) );

  count=0;
  probflag = 0 ;
while( howmany-count++ ) {
  ntries = MAXTRIES;
/* read in a sample */
  do {
     if( fgets( line, 10000, pfin) == NULL ) exit(0);
  }while ( line[0] != '/' );

  do {
    if( fgets( tline, 10000, tfin) == NULL ) exit(0);
  }while (tline[0] != '/');
 
  fscanf(pfin,"  segsites: %d", &segsites );
  if( segsites >= maxsites){
    maxsites = segsites + 10 ;
    posit = (double *)realloc( posit, maxsites*sizeof( double) ) ;
    times = realloc(times, maxsites*sizeof(double));
    indexes = realloc(indexes, maxsites*sizeof(int));
    biggerlist(nsam,maxsites, list) ;
        }
  if( segsites > 0) {
    fscanf(pfin," %s", astr);
    if( astr[1] == 'r' ){
      fscanf(pfin," %lf", &prob ) ;
      probflag = 1;
      fscanf(pfin," %*s");
    }
    for( i=0; i<segsites ; i++) fscanf(pfin," %lf",posit+i) ;
    for( i = 0; i < segsites; ++i) fscanf(tfin, "%lf ", times + i);
    for( i =0; i <segsites; ++i) indexes[i] = i;
    for( i=0; i<nsam;i++) fscanf(pfin," %s", list[i] );
  }

  sortIndexes(times, indexes, segsites);

  while( ntries > 0){
    int repeatFlag = 0;
    ntries--;
    for( ind = 0; ind < nsam; ind++) nrepeats[ind] = initialCopies ;
   
    for( i = 0; i< segsites; i++){

      if( drand48() < .5)
	step = -1 ;
      else
	step = 1 ;
      
      for( ind = 0; ind < nsam; ind++)
	if( list[ind][indexes[i]] == '1' )
	  {
	    nrepeats[ind] += step ;
	    if(nrepeats[ind] < 0 ) // species should not have negative repeats
	      {
		repeatFlag = 1;
		break;
	      }
	  }
    }
    if(repeatFlag == 0)
      break;
  }
  if(ntries > 0)
    {
      fprintf(stderr, "tries: %d\n", MAXTRIES - ntries);
      for( ind=0; ind < nsam-1; ind++) printf("%d\t",nrepeats[ind] );
      printf("%d",nrepeats[nsam-1]);
      printf("\t%s",line+2 );
    }
  else{
    fprintf(stderr, "Failure...\n");
    exit(0);
  }
 }
}

	

/* allocates space for gametes (character strings) */
	char **
cmatrix(nsam,len)
	int nsam, len;
{
	int i;
	char **m;

	if( ! ( m = (char **) malloc( (unsigned)( nsam*sizeof( char* )) ) ) )
	   perror("alloc error in cmatrix") ;
	for( i=0; i<nsam; i++) {
		if( ! ( m[i] = (char *) malloc( (unsigned) (len*sizeof( char )) )))
			perror("alloc error in cmatric. 2");
		}
	return( m );
}


