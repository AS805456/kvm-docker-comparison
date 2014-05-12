#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>
#include "utils.h"
#define OUTFNAME	"RESULTS"

#define  GiBYTE   1073741824L


int mem_total, mem_free;


void
get_mem_info( void )
/********1*****|***2*********3*********4*********5*********6*********7**!

description:   This function gets the total and free memory.

 invoked by:   routine            description
               --------------     --------------------------------------
               main             - int (mpbs.src)
                                  This is the main driving program for
                                  the Massive Parallel Bucket Sort(MPBS)
                                  system.

    invokes:   routine            description
               ---------------    --------------------------------------
               get_mem_value    - char *
                                  Finds a number in a string delimited
                                  by spaces.

  variables:   varible            description
               --------------     --------------------------------------
               file             - FILE *
                                  File pointer that points to memory
                                  info file.

               line[]           - char
                                  Holds a line of text read in from
                                  /proc/meminfo file.

*********1*****|***2*********3*********4*********5*********6*********7**/
{
   /*
    * variable declaration section
    */
   FILE *file;
   char line[BUFSIZ];

   char *get_mem_value( const char * );

   if( (file = fopen( "/proc/meminfo", "r" )) != NULL )
   {
      /* get first line */
      fgets( line, sizeof( line ), file );
      mem_total = atoi( get_mem_value( line ) );

      /* get second line */
      fgets( line, sizeof( line ), file );
      mem_free = atoi( get_mem_value( line ) );
   } else {
      perror( "fopen" );
   }
   fclose( file );

   return;
}


char *
get_mem_value( const char *line )
/********1*****|***2*********3*********4*********5*********6*********7**!

description:   This function finds a number in a string delimited
               by spaces.

 invoked by:   routine            description
               --------------     --------------------------------------
               get_mem_info     - void
                                  This function gets the total and free
                                  memory.

  variables:   varible            description
               --------------     --------------------------------------
               line             - const char *
                                  String read in from file and passed
                                  in by get_mem_info.

               number[]         - char
                                  Holds the number string found in line.

               mem_value        - char *
                                  Holds the number string from number
                                  allocated to specific length.

               i, j             - int
                                  Position counter in strings. 

*********1*****|***2*********3*********4*********5*********6*********7**/
{
   /*
    * variable declaration section
    */
   char number[20] = { ' ' };
   char *mem_value = NULL;
   int i = 0;
   int j = 0;

   do
   {
      if( isalpha( line[i] ) )
         ;
      if( isspace( line[i] ) )
         ;
      if( isdigit( line[i] ) )
      {
         number[j] = line[i];
         j++;
      }
      i++;
   } while( line[i] != '\n' );

   number[j] = '\0';
   mem_value = malloc( (j+1)*sizeof(char) );
   strcpy( mem_value, number );

   return( mem_value );
}



void
getASCIITime (char *buf, int len) {
   time_t currtime;
   struct tm currtm;

   memset (&currtime, 0, sizeof(currtime));
   time (&currtime);
   gmtime_r (&currtime, &currtm);

   strftime (buf, len, "%F:%T %Z", &currtm);
   // printf ("Generated on %s\n", buf);
}


double
GetTime ()
{
   struct timeval tp;
   (void) gettimeofday( &tp, NULL );
   return (tp.tv_sec + ((double) tp.tv_usec)/1000000.0);
}


HPCC_Params *
initialize () {
    HPCC_Params *params = (HPCC_Params *) malloc (sizeof (HPCC_Params));
	getASCIITime (params->nowASCII, sizeof(params->nowASCII)-1);
	params->outFname = OUTFNAME;
	return params;
}


uint64_t
HPCC_LocalVectorSize (void *unused, int num, int size, int dummy) {
	get_mem_info();
	// printf ("mem_total = %d, mem_free = %d\n", mem_total, mem_free);
	uint64_t totveclen = ((uint64_t) mem_total) / 2 * 1024 / size;

	/* Pick a vector length that is 1/(num + 1)th of this amount */

	uint64_t veclen = totveclen / (num + 1);
	return veclen;
}

