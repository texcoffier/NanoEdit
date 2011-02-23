/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2004 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/
/* Developped by Hexasoft (Y.Perret, yperret@ligim.univ-lyon1.fr) */

/*  Hexasoft, Mon December 4, 2000.
December 7,  2000: updating to V1.1.1
December 20, 2000: updating to V1.1.2
December 22, 2000: updating to V1.1.3
January 2,   2001: updating to V1.1.4
January 8,   2001: updating to V1.1.5
*/

/* this module add treatments for enter/exit of all of your functions */

/*
   usage: see the README.txt
*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "fnccheck.h"


/* to prevent compilation if not using GCC */
#ifdef __GNUC__

/* need to declare it here */
void __cyg_profile_func_exit(void *this_fn, void *call_site);

/* compare times */
/* I think a macro to do this exists in time.h... */
#define HX_IS_GREATER(sec,usec,lsec,lusec)  (sec>lsec?1:(sec<lsec?0:(usec>lusec?1:0)))
#define HX_IS_LOWER(sec,usec,lsec,lusec)    (sec<lsec?1:(sec>lsec?0:(usec<lusec?1:0)))

/* string buffers */
#define HX_MAX_BUFFER 1024

/* hum, not nice, but usefull for me... */
int hx_debug_level=0;
#define hx_printf    if(hx_debug_level)    printf
#define hx_printf1   if(hx_debug_level&1)  printf
#define hx_printf2   if(hx_debug_level&2)  printf
#define hx_printf4   if(hx_debug_level&4)  printf
#define hx_printf8   if(hx_debug_level&8)  printf
#define hx_printf16  if(hx_debug_level&16) printf
#define hx_printf32  if(hx_debug_level&32) printf

#define EPS 0.000001  /* 1 usec */

/* exec name: how to get it ? because I cant access argv[] here! */
/* perhaps using /proc/<PID>/ but is it portable ? */ 
char *hx_exec_name="<no_idea>";

/* the output file */
static char *HX_DROP_FILE=FNCCHK_DEFAULT_FILE;

/* the kind of time used */
#define HX_TIME_EXT 0   /* absolute time */
#define HX_TIME_CPU 1   /* internal time (CPU) */
#define HX_TIME_SYS 2   /* CPU+SYSTEM times */
static int hx_time_type=HX_TIME_EXT;
clock_t hx_last_given;  /* used for time loops */

/* if true, indicate a REAL exit without treatments */
/* because as we register an atexit function, it
   will be called even for our internal exits, but
   these exits are emergency exits and may not be
   treated in the same way than a real exit */
int hx_internal_exit=0;
/* if true, indicate that exit is still done.
   this is used because C++ destructors are
   called AFTER the atexit call, and so after
   every data were freed.  */
int hx_exit_done=0;


/* not really nice. The max number of callers for each
   function that are registered. */
#define HX_MAX_CALLER 64

/* structure of a function registration */
typedef struct
{
  void *function;  /* the pointer to the function */
  unsigned long int nb_calls; /* total # of calls */
  unsigned long int nb_sec;   /* total # of secs */
  unsigned long int nb_usec;  /* total # of usecs */
  unsigned int recurs_count;  /* recursive counts */
  /* local time of the function */
  unsigned long int nb_locsec;   /* # of secs */
  unsigned long int nb_locusec;  /* # of usecs */
  /* last active time for the function */
  unsigned long int nb_latsec;   /* # of secs */
  unsigned long int nb_latusec;  /* # of usecs */
  /* min and max time for the function */
  unsigned long int min_sec;   /* # of secs */
  unsigned long int min_usec;       /* # of usecs */
  unsigned long int max_sec;   /* # of secs */
  unsigned long int max_usec;       /* # of usecs */
  void * called_by[HX_MAX_CALLER];  /* list of callers */
  void * rcalled_by[HX_MAX_CALLER]; /* list of real callers (for file/line) */
  unsigned int nb_of_callers;  /* pos in previous tab */
  unsigned int nb_in_stack;    /* # of insert in the stack */
}HX_FInfo;

/* info table */
#define HX_MAX_FUNC 1024
HX_FInfo *hx_hx_dfinfo=NULL;
unsigned int hx_nb_finfos=0;
unsigned int hx_max_nb_finfos=0;  /* for realloc */

/* stack of callers and entering times */
#define HX_MAX_STACK 1000000
unsigned int hx_pos_in_stack=0;
unsigned int hx_pos_max_in_stack=0;  /* for realloc */
unsigned int hx_max_in_stack=0;      /* the MAX stack used */
struct timeval *hx_entering=NULL;
unsigned int *hx_who_is_before=NULL;

int hx_dynamic_mode=0;  /* dynamique reallocations */
int hx_nb_of_realloc=0; /* count of reallocations */
int hx_add_pid=0;       /* add PID to stat-file name */

/* average time spend in treatments */
long hx_avg_sec, hx_avg_usec;

/* for time */
unsigned long int hx_last_time=0;
struct timeval hx_current_time={0,0};
unsigned long int hx_time_loop=0;

/* hash table management */
typedef struct
{
  void *fnc;        /* function stored */
  unsigned int pos; /* pos int the list */
}HX_HashEl;
/* got a better way to prevent mallocs ? */
#define HX_MAX_HASH_EL 32  /* max collisions */
typedef HX_HashEl HX_HashEntry[HX_MAX_HASH_EL];
HX_HashEntry *hx_hash_list=NULL;
unsigned int hx_nb_hash_list=0;

/* my 'gettimeofday' */
static void hx_gettimeofday(struct timeval *tv)
{
  unsigned long int result;
  unsigned long int delta;

  /* standard case */
  if (hx_time_type == HX_TIME_EXT)
    {
    gettimeofday(tv, NULL);
    return;
    }
  /* CPU/SYS time */
  result = (unsigned long int)clock();
  /* over the limit ? */
  if (result < hx_last_time)
    {
    delta = (result + hx_time_loop - hx_last_time);
    }
  else
    {
    delta = (result - hx_last_time);
    }
  /* ask to P.S. to test this part in this site */  
  /* can only be used if CLOCK_PER_SEC == 1000000 */
  /* some troubles reported... still checking */
  hx_current_time.tv_usec += delta % CLOCKS_PER_SEC;
  hx_current_time.tv_sec += ((delta - (delta % CLOCKS_PER_SEC)) / 1000000);
  if (hx_current_time.tv_usec >= 1000000)
    {
    hx_current_time.tv_usec -= 1000000;
    hx_current_time.tv_sec += 1;
    }
  tv->tv_sec  = hx_current_time.tv_sec;
  tv->tv_usec = hx_current_time.tv_usec;
  
  hx_last_time = result;
}

/* divide given sec/usec by 'nb' */
inline void hx_divide_time(unsigned long int *sec,
                           unsigned long int *usec,
                           unsigned int nb)
{
  double rt;
printf("# %lu %lu %u\n", *sec, *usec, nb);
  /* nothing to do */
  if (nb <= 1)
    return;

  /* divide the time */
  if (*sec == 0)
    {
    *usec /= nb;
    }
  else
    {
    rt = (double)*sec + (*usec/1000000.);
    rt /= (double)nb;
    /* re-compute the sec/usec */
    *sec = (unsigned long int)(floor(rt));
    *usec = (unsigned long int)((rt-(*sec))*1000000.);
    }
}

/** the function which dump the stat file **/
/* everything is freed after. it is so the last function */
static void hx_dump_to_file()
{
  FILE *f;
  unsigned int pos, j;
  void *fncfake=NULL;
  char buffer[HX_MAX_BUFFER];  /* to be changed */
#ifdef FNCCHK_DEBUG
  int i;
#endif

  /* if true, it is a call from a C++ destructor
     AFTER the prog exit. just forget it */
  if (hx_exit_done)
    return;

  /* is internal exit, no dump. it's an error exit */
  if (hx_internal_exit == 1)
    {
    if ((f = fopen(HX_DROP_FILE, "w")) != NULL)
      {/* make an empty stat file */
      fclose(f);
      }
    return; /* that's all */
    }
  else
  if (hx_internal_exit == 2)
    {/* hard exit! */
    return;
    }

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: hx_dump_to_file()\n");
#endif

  /* debug level 2: display hashtable colisions */
#ifdef FNCCHK_DEBUG
  if (hx_debug_level & 2)
    {
    printf("# Hash table colisions:\n");
    for(i=0; i<hx_nb_hash_list; i++)
      {
      j = 0;
      while((j <HX_MAX_HASH_EL)&&(hx_hash_list[i][j].fnc != NULL))
        j++;
      printf("(%d:%d) ", i, j);
      }
    printf("\n# End\n");
    }
#endif

  /* if stack is not empty -> quit using 'exit()' */
  /* we call __..._leave() in order to make coherent
     informations available */
  if (hx_pos_in_stack > 0)
    {
    while(hx_pos_in_stack > 0)
      {
      pos = hx_who_is_before[hx_pos_in_stack-1];
      fncfake = hx_hx_dfinfo[pos].function;
      __cyg_profile_func_exit(fncfake, NULL);
      }
    }

  if (hx_add_pid)
    {
    sprintf(buffer, "%s_%d", HX_DROP_FILE, (int)getpid());
    }
  else
    {
    sprintf(buffer, HX_DROP_FILE);
    }
  if ((f = fopen(buffer, "w")) == NULL)
    {
    fprintf(stderr, "fnccheck: Cant create '%s' stat file!\n", buffer);
    return;
    }
  fprintf(f, "%s\n", FNCCHK_HEADER);
  fprintf(f, "%d\n", hx_nb_finfos);
  for(pos=0; pos<hx_nb_finfos; pos++)
    {
    /* output format:
	 HEADER
	 # of elements
	 f1_ptr  #calls sec usec locsec locusec minsec minusec maxsec maxusec
	 f2_ptr  #calls sec usec locsec locusec minsec minusec maxsec maxusec
	 ...
	 # of realloc
	 stack_size func_table_size
	 nb_of_callers_f1 caller1_ptr rc1_ptr caller2_ptr rc2_ptr ...
	 nb_of_callers_f2 caller1_ptr rc1_ptr caller2_ptr rc2_ptr ...
	 ...
	 time_mode
	 exec_name
    */
    fprintf(f, "%p %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",
                        	 hx_hx_dfinfo[pos].function,
                        	 hx_hx_dfinfo[pos].nb_calls,
                        	 hx_hx_dfinfo[pos].nb_sec,
                        	 hx_hx_dfinfo[pos].nb_usec,
                        	 hx_hx_dfinfo[pos].nb_locsec,
                        	 hx_hx_dfinfo[pos].nb_locusec,
                        	 hx_hx_dfinfo[pos].min_sec,
                        	 hx_hx_dfinfo[pos].min_usec,
                        	 hx_hx_dfinfo[pos].max_sec,
                        	 hx_hx_dfinfo[pos].max_usec);
    }
  fprintf(f, "%d\n", hx_nb_of_realloc);
  fprintf(f, "%u %u\n", hx_max_in_stack, hx_nb_finfos);
  for(pos=0; pos<hx_nb_finfos; pos++)
    {
    fprintf(f, "%u ", hx_hx_dfinfo[pos].nb_of_callers);
    for(j=0; j<hx_hx_dfinfo[pos].nb_of_callers; j++)
      {
      if (j+1 < hx_hx_dfinfo[pos].nb_of_callers)
        fprintf(f, "%p %p ", hx_hx_dfinfo[pos].called_by[j], hx_hx_dfinfo[pos].rcalled_by[j]);
      else
        fprintf(f, "%p %p", hx_hx_dfinfo[pos].called_by[j], hx_hx_dfinfo[pos].rcalled_by[j]);
      }
    fprintf(f, "\n");
    }
  fprintf(f, "%d\n", hx_time_type);
  fprintf(f, "%s\n", hx_exec_name);
  fclose(f);
  /* that was last call. freeing datas */
  if (hx_hx_dfinfo != NULL)
    free(hx_hx_dfinfo);
  if (hx_entering != NULL)
    free(hx_entering);
  if (hx_who_is_before != NULL)
    free(hx_who_is_before);

  /* exit is done. never come again! */
  hx_exit_done = 1;

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: leaving hx_dump_to_file()\n");
#endif


  printf("FunctionCheck: all done (sfile: %s).\n", buffer);
}
/* used to redirect signals to atexit function */
static void hx_sig_dump_to_file(int dummy)
{
  hx_dump_to_file();
  hx_internal_exit = 2;
  exit(0);
}


/** init of the module system **/

static unsigned long int hx_2pow(unsigned int v)
{
  unsigned long int ret=1;
  unsigned int i;

  for(i=0; i<v; i++)
    ret *= 2;
  return(ret);
}

static void hx_init_checker(void)
{
  char *def_stack, *def_fnc, *buf, *buf2;
  int def_istack=HX_MAX_STACK, def_ifnc=HX_MAX_FUNC;
  int i, j;

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: hx_init_checker()\n");
#endif

  /* just said that we are here... */
  printf("FunctionCheck: starting (V%s by Hexasoft).\n", FNCCHK_FULL_VERSION);

  /* read env for user-defined values */
  if ((def_stack = getenv("FNCCHK_STOP")) != NULL)
    {
    printf("FunctionCheck: stopped by FNCCHK_STOP.\n");
    hx_exit_done = 1;
    return;
    }

  /* register exit function */
  if (atexit(hx_dump_to_file) != 0)
    {
    fprintf(stderr, "fnccheck:init: Cant register exit function!\n");
    fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
    hx_internal_exit = 1;
    exit(1);
    }
  /* register kill signals to make emergency dumps */
  /* which signals need to be catched ? */
  signal(SIGHUP,  hx_sig_dump_to_file);
  signal(SIGINT,  hx_sig_dump_to_file);
  signal(SIGTERM, hx_sig_dump_to_file);
  signal(SIGQUIT, hx_sig_dump_to_file);
  signal(SIGPIPE, hx_sig_dump_to_file);
  signal(SIGTERM, hx_sig_dump_to_file);

  /* read env for user-defined values */
  if ((def_stack = getenv("FNCCHK_STACKSIZE")) != NULL)
    {
    sscanf(def_stack, "%d", &def_istack);
    if (def_istack < 16)  /* invalid value */
      def_istack = HX_MAX_STACK;
    }
  if ((def_fnc = getenv("FNCCHK_NBFUNC")) != NULL)
    {
    sscanf(def_fnc, "%d", &def_ifnc);
    if (def_ifnc < 16)  /* invalid value */
      def_ifnc = HX_MAX_FUNC;
    }
  if ((def_fnc = getenv("FNCCHK_DEBUG")) != NULL)
    {
    sscanf(def_fnc, "%d", &hx_debug_level);
    if (hx_debug_level < 0)  /* invalid value */
      hx_debug_level = 0;
    }
  if ((buf = getenv("FNCCHK_OUTFILE")) != NULL)
    {
    if ((buf2 = strdup(buf)) == NULL)
      {
      fprintf(stderr, "fnccheck:init: Memory error!\n");
      fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
      hx_internal_exit = 1;
      exit(1);
      }
    HX_DROP_FILE = buf2;
    }
  if ((buf = getenv("FNCCHK_TIME")) != NULL)
    {
    if (strcasecmp(buf, "ext") == 0)
      {
      hx_time_type = HX_TIME_EXT;
      }
    else
    if (strcasecmp(buf, "cpu") == 0)
      {
      hx_time_type = HX_TIME_CPU;
      }
    else
    if (strcasecmp(buf, "sys") == 0)
      {
      hx_time_type = HX_TIME_CPU;
      }
    else
      {
      fprintf(stderr, "fnccheck:init: Invalid value for 'FNCCHK_TIME' (%s).\n", buf);
      fprintf(stderr, "fnccheck:init: Ignored (EXT used).\n");
      }
    }
  if ((def_fnc = getenv("FNCCHK_DYNAMIC")) != NULL)
    {
    hx_dynamic_mode = 1;
    }
  if ((def_fnc = getenv("FNCCHK_PID")) != NULL)
    {
    hx_add_pid = 1;
    }
  hx_pos_max_in_stack = def_istack;
  hx_max_nb_finfos = def_ifnc;

  /* data allocations */
  if ((hx_hx_dfinfo = malloc(sizeof(HX_FInfo)*def_ifnc)) == NULL)
    {/* fatal error */
    fprintf(stderr, "fnccheck:init: Cant allocate memory (%d bytes)!\n", sizeof(HX_FInfo)*def_ifnc);
    fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
    hx_internal_exit = 1;
    exit(1);
    }
  if ((hx_entering = malloc(sizeof(struct timeval)*def_istack)) == NULL)
    {/* fatal error */
    fprintf(stderr, "fnccheck:init: Cant allocate memory (%d bytes)!\n", sizeof(struct timeval)*def_istack);
    fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
    hx_internal_exit = 1;
    exit(1);
    }
  if ((hx_who_is_before = malloc(sizeof(unsigned int)*def_istack)) == NULL)
    {/* fatal error */
    fprintf(stderr, "fnccheck:init: Cant allocate memory (%d bytes)!\n", sizeof(unsigned int)*def_istack);
    fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
    hx_internal_exit = 1;
    exit(1);
    }
  hx_nb_hash_list = (unsigned int)(def_ifnc*1.6667);
  if ((hx_nb_hash_list % 2) == 0)
    hx_nb_hash_list++; /* dont ask why... I prefer odd size :) */
  if ((hx_hash_list = malloc(sizeof(HX_HashEntry)*hx_nb_hash_list)) == NULL)
    {/* fatal error */
    fprintf(stderr, "fnccheck:init: Cant allocate memory (%d bytes)!\n", sizeof(HX_HashEntry)*hx_nb_hash_list);
    fprintf(stderr, "fnccheck:init: Fatal error! Abort!\n");
    hx_internal_exit = 1;
    exit(1);
    }
  /* init of hashtable */
  for(i=0; i<hx_nb_hash_list; i++)
    {
    for(j=0; j<HX_MAX_HASH_EL; j++)
      {
      hx_hash_list[i][j].fnc = NULL;
      hx_hash_list[i][j].pos = 0;
      }
    }
  /* init time */
  hx_last_time = (unsigned long int)clock();
  /* compute time-loop for 'times' */
  hx_time_loop = hx_2pow((unsigned int)(8*sizeof(clock_t))) / 2;

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: leaving hx_init_checker()\n");
#endif
}


/** functions that manage profiles **/

/* add a new function in the table */
inline unsigned int hx_register_function(void *fnc)
{
#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: hx_register_function(%p)\n", fnc);
#endif
  /* too many functions */
  if (hx_nb_finfos >= hx_max_nb_finfos)
    {/* 0 -> error */
    if (hx_dynamic_mode)
      {
      if ((hx_hx_dfinfo = realloc(hx_hx_dfinfo, sizeof(HX_FInfo)*(hx_max_nb_finfos*2))) == NULL)
        {
	fprintf(stderr, "fnccheck: Memory error!\n");
	fprintf(stderr, "fnccheck: Fatal error! Abort!\n");
      hx_internal_exit = 1;
        exit(1);
	}
      hx_nb_of_realloc++;
      hx_max_nb_finfos *= 2;
      }
    else
      {
      fprintf(stderr, "fnccheck: Max # of functions reached!\n");
      fprintf(stderr, "fnccheck: Switch to dynamic allocation by setting FNCCHK_DYNAMIC=1!\n");
      fprintf(stderr, "fnccheck: Fatal error! Abort!\n");
      hx_internal_exit = 1;
      exit(1);
      }
    }
  /* initializations */
  hx_hx_dfinfo[hx_nb_finfos].function   = fnc;
  hx_hx_dfinfo[hx_nb_finfos].nb_calls   = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_sec     = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_usec    = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_locsec  = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_locusec = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_latsec  = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_latusec = 0;
  hx_hx_dfinfo[hx_nb_finfos].min_sec    = 0;
  hx_hx_dfinfo[hx_nb_finfos].min_usec   = 0;
  hx_hx_dfinfo[hx_nb_finfos].max_sec    = 0;
  hx_hx_dfinfo[hx_nb_finfos].max_usec   = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_of_callers = 0;
  hx_hx_dfinfo[hx_nb_finfos].recurs_count  = 0;
  hx_hx_dfinfo[hx_nb_finfos].nb_in_stack   = 0;
  
  hx_nb_finfos++;
#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: leaving hx_register_function(%p)\n", fnc);
#endif
  /* return the number of functions */
  return(hx_nb_finfos);
}

/* search a function (+1) (or 0) */
inline unsigned int hx_search_function(void *fnc)
{
  unsigned int i, key, pos;

  key = (((unsigned int)fnc)/4) % hx_nb_hash_list;
  for(i=0; i<HX_MAX_HASH_EL; i++)
    {
    if (hx_hash_list[key][i].fnc == NULL)
      {/* no more entries. register */
      pos = hx_register_function(fnc);
      hx_hash_list[key][i].fnc = fnc;
      hx_hash_list[key][i].pos = pos-1;
      return(pos);
      }
    if (hx_hash_list[key][i].fnc == fnc)
      return(hx_hash_list[key][i].pos+1);
    }
  /* error! full! */
  fprintf(stderr, "fnccheck:hashtable: max # of colision reached!\n");
  fprintf(stderr, "fnccheck:Fatal error!\n");
  exit(1);
}

/* get the 'entering' time and add it in the stack */
inline void hx_get_entering_time()
{
  /* stack error! */
  if (hx_pos_in_stack >= hx_pos_max_in_stack)
    {
    if (hx_dynamic_mode)
      {
      if ((hx_entering = realloc(hx_entering, sizeof(struct timeval)*(hx_pos_max_in_stack*2))) == NULL)
        {
	fprintf(stderr, "fnccheck: Memory error!\n");
	fprintf(stderr, "fnccheck: Fatal error! Abort!\n");
        hx_internal_exit = 1;
        exit(1);
	}
      hx_nb_of_realloc++;
      if ((hx_who_is_before = realloc(hx_who_is_before, sizeof(unsigned int)*(hx_pos_max_in_stack*2))) == NULL)
        {
	fprintf(stderr, "fnccheck: Memory error!\n");
	fprintf(stderr, "fnccheck: Fatal error! Abort!\n");
        hx_internal_exit = 1;
        exit(1);
	}
      hx_nb_of_realloc++;
      hx_pos_max_in_stack *= 2;
      }
    else
      {
      fprintf(stderr, "fnccheck: Stack is full!\n");
      fprintf(stderr, "fnccheck: Switch to dynamic allocation by setting FNCCHK_DYNAMIC=1!\n");
      fprintf(stderr, "fnccheck: Fatal error! Abort!\n");
      hx_internal_exit = 1;
      exit(1);
      }
    }
  /* read the time */
  hx_gettimeofday(&(hx_entering[hx_pos_in_stack])); 
  hx_pos_in_stack++;
  /* check the max used size of stack */
  if (hx_pos_in_stack > hx_max_in_stack)
    hx_max_in_stack = hx_pos_in_stack;
}


/* compute elapsed time between the two times */
inline void hx_compute_elapsed_time(unsigned long int sec1, unsigned long int usec1,
                         unsigned long int sec2, unsigned long int usec2,
			 long *secr, long *usecr)
{
  if (sec1 == sec2)
    {
    *secr  = 0;
    *usecr = usec2 - usec1;
    }
  else
    {
    *secr  = sec2 - sec1 - 1;
    *usecr = usec2 + 1000000-usec1;
    }
  if (*usecr >= 1000000)
    {
    *secr  += 1;
    *usecr -= 1000000;
    }
}


/* read the 'leaving' time and compute the delay elapsed
   (sec/usec) from the corrsponding 'enter' */
inline void hx_get_leaving_time(long *sec, long *usec,
                                unsigned long int *asec,
				unsigned long int *ausec)
{
  struct timeval tv;

  /* stack pop */
  if (hx_pos_in_stack > 0)
    hx_pos_in_stack--;

  /* read time */
  hx_gettimeofday(&tv);
  *asec  = (unsigned long int)tv.tv_sec;
  *ausec = (unsigned long int)tv.tv_usec;
  /* compute delay */
  if (tv.tv_sec == hx_entering[hx_pos_in_stack].tv_sec)
    {
    *sec = 0;
    *usec = tv.tv_usec - hx_entering[hx_pos_in_stack].tv_usec;
    }
  else
    {
    *sec = tv.tv_sec - hx_entering[hx_pos_in_stack].tv_sec - 1;
    *usec = tv.tv_usec + 1000000-hx_entering[hx_pos_in_stack].tv_usec;
    }
  if (*usec >= 1000000)
    {
    *sec += 1;
    *usec -= 1000000;
    }
}

inline void hx_substract_time(unsigned long int *sec,
                              unsigned long int *usec,
                              unsigned long int dsec,
                              unsigned long int dusec)
{
  unsigned long int sup=0;
  if (dusec > *usec)
    {
    *usec = (1000000+*usec) - dusec;
    sup = 1;
    }
  else
    {
    *usec = *usec - dusec;
    }
  *sec = *sec - dsec - sup;
}

/* trying MACRO version -> got a compilation error. I'll check it later
#define hx_substract_time(sec, usec, dsec, dusec) \
    if (dusec > *(usec)) {*(usec)=1000000+*(usec))-dusec; *(sec)=*(sec)-dsec-1;} \
    else\
                       {*(usec) = *(usec) - dusec; *(sec)=*(sec)-dsec;}
*/


/* add the caller reference in function */
/* CHANGES: add the call_site for display purpose */
inline void hx_register_caller(unsigned int who, unsigned int from, void *rfrom)
{
  unsigned int i;

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: hx_register_caller(%d, %d, %p)\n", who, from, rfrom);
#endif
  if (hx_hx_dfinfo[who].nb_of_callers >= HX_MAX_CALLER)
    return; /* list is full */
  for(i=0; i<hx_hx_dfinfo[who].nb_of_callers; i++)
    {
    if (hx_hx_dfinfo[who].called_by[i] == hx_hx_dfinfo[from].function)
      return;  /* still referenced */
    }
  hx_hx_dfinfo[who].called_by[hx_hx_dfinfo[who].nb_of_callers] =
        hx_hx_dfinfo[from].function;
  hx_hx_dfinfo[who].rcalled_by[hx_hx_dfinfo[who].nb_of_callers] =
        rfrom;
  hx_hx_dfinfo[who].nb_of_callers++;

#ifdef FNCCHK_DEBUG
hx_printf4("fnccheck: leaving hx_register_caller(%d, %d, %p)\n", who, from, rfrom);
#endif
}


/** functions which treat entering/leaving **/



/* function called when entering a function */
void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
  static int first=1;
  unsigned int pos;
  long sec, usec;

#ifdef FNCCHK_DEBUG
  hx_printf4("fnccheck: __cyg_profile_func_enter(%p, %p)\n", this_fn, call_site);
#endif

  /* if true, it is a call from a C++ destructor
     AFTER the prog exit. just forget it */
  if (hx_exit_done)
    return;

  /* at first call, prepare datas for treatments */
  if (first)
    {
    first = 0;
    hx_init_checker();
    /* if FNCCHK_STOP, exit_done is set */
    if (hx_exit_done)
      return;
    }

  /* is this function still recorded ? */
  if ((pos = hx_search_function(this_fn)) == 0)
    {
    fprintf(stderr, "fnccheck: 'entering' treatment aborted.\n");
    return; /* exit treatments */
    }
  pos--;
  hx_hx_dfinfo[pos].nb_in_stack++;
#ifdef FNCCHK_DEBUG
hx_printf8("fnccheck:__cyg_enter: function %u, %u times in the stack.\n",
             pos, hx_hx_dfinfo[pos].nb_in_stack);
#endif
  if (hx_pos_in_stack > 0)
    {/* we know the caller */
    hx_register_caller(pos, hx_who_is_before[hx_pos_in_stack-1], call_site-1);
    }

  /* just insert the caller */
  hx_who_is_before[hx_pos_in_stack] = pos;
  /* get the entering time (increase stack position) */
  hx_get_entering_time();
  /* NEW: treatments to compute local time for parent function.
     entering a new function means that the previous one is not
     curently active. So we can compute the elapsed local time
     for it (diff of "last active time" and current time) which
     is added to local time.  */
  if (hx_pos_in_stack > 1)  /* >1 and not >0 because stack is still ++ */
    {
    hx_compute_elapsed_time(  /* last active time for calling function */
         hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-2]].nb_latsec,
         hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-2]].nb_latusec,
	 /* current time */
         hx_entering[hx_pos_in_stack-1].tv_sec,
         hx_entering[hx_pos_in_stack-1].tv_usec,
	 &sec, &usec);
    /* add this time to local time of calling function */
    hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-2]].nb_locsec  += sec;
    hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-2]].nb_locusec += usec;
    }
  /* init the last active time for the function */
  hx_hx_dfinfo[pos].nb_latsec  = (unsigned int)hx_entering[hx_pos_in_stack-1].tv_sec;
  hx_hx_dfinfo[pos].nb_latusec = (unsigned int)hx_entering[hx_pos_in_stack-1].tv_usec;

#ifdef FNCCHK_DEBUG
  hx_printf4("fnccheck: leaving __cyg_profile_func_enter(%p, %p)\n", this_fn, call_site);
#endif

}

/* function called when exiting a function */
void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
  unsigned int pos;
  long sec, usec;
  unsigned long int asec, ausec;
  unsigned long int nasec, nausec;

#ifdef FNCCHK_DEBUG
  hx_printf4("fnccheck: __cyg_profile_func_exit(%p, %p)\n", this_fn, call_site);
#endif  
  /* if true, it is a call from a C++ destructor
     AFTER the prog exit. just forget it */
  if (hx_exit_done)
    return;

  /* which function ? (posinstack-1 because stack points on next entry) */
  if (hx_pos_in_stack > 0)
    pos = hx_who_is_before[hx_pos_in_stack-1];
  else
    pos = 0;  /* exiting the MAIN() function */

  /* we compute the elapsed time */
  /* dont put this before 'pos' computation because
     'hx_get_leaving_time' change stack value */
  hx_get_leaving_time(&sec, &usec, &asec, &ausec);

  /* NEW: set the new "last active time" to 'asec'/'ausec'
       because now the current function is performing local
       treatments (until we not enter a new sub-function) */
  /* -1 and not -2 because 'get_leaving_time' decreases stack */
  if (hx_pos_in_stack > 0)
    {
    hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-1]].nb_latsec  = asec;
    hx_hx_dfinfo[hx_who_is_before[hx_pos_in_stack-1]].nb_latusec = ausec;
    }

  /* NEW: compute last part of local time for the current function */
  hx_compute_elapsed_time(
       hx_hx_dfinfo[pos].nb_latsec, hx_hx_dfinfo[pos].nb_latusec,
       asec, ausec, &nasec, &nausec);
  hx_hx_dfinfo[pos].nb_locsec  += nasec;
  hx_hx_dfinfo[pos].nb_locusec += nausec;

#ifdef FNCCHK_DEBUG
  hx_printf8("fnccheck:__cyg_exit: function is %u. elapsed time is %ld sec & %ld usec\n",
                 pos, sec, usec);
#endif

  /* we register info in the structure */
  /* MIN/MAX management. It is done BEFORE removing subtime for
     recursive functions. So the MAX time will indicate the real
     MAX time spend in this function. */
  if (hx_hx_dfinfo[pos].nb_calls == 0)
    { /* first call: init min and max */
    hx_hx_dfinfo[pos].min_sec  = sec;
    hx_hx_dfinfo[pos].min_usec = usec;
    hx_hx_dfinfo[pos].max_sec  = sec;
    hx_hx_dfinfo[pos].max_usec = usec;
    }
  else /* test for new min / max */
    {
    if (HX_IS_GREATER(sec, usec, hx_hx_dfinfo[pos].max_sec, hx_hx_dfinfo[pos].max_usec))
      {
      hx_hx_dfinfo[pos].max_sec  = sec;
      hx_hx_dfinfo[pos].max_usec = usec;
      }
    else /* if it is the MAX, it may not be the MIN ! */
    if (HX_IS_LOWER(sec, usec, hx_hx_dfinfo[pos].min_sec, hx_hx_dfinfo[pos].min_usec))
      {
      hx_hx_dfinfo[pos].min_sec  = sec;
      hx_hx_dfinfo[pos].min_usec = usec;
      }
    }
  /* one more call */
  hx_hx_dfinfo[pos].nb_calls++;
  /* remove an occurence in the stack */
  hx_hx_dfinfo[pos].nb_in_stack--;

  /* if it is not a recursive function, we perform standard treatments */
  if (hx_hx_dfinfo[pos].nb_in_stack == 0)
    {
    /* add the (total) time to the function */
    hx_hx_dfinfo[pos].nb_sec  += sec;
    hx_hx_dfinfo[pos].nb_usec += usec;
    }
  else /* several occurence of the function in the stack */
    {
    /* nothing to do, I think... beeing checked */
    }

#ifdef FNCCHK_DEBUG
  hx_printf4("fnccheck: leaving __cyg_profile_func_exit(%p, %p)\n", this_fn, call_site);
#endif

}


#endif  /* __GNUC__ */
