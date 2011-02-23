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
#ifndef _hx_fnccheck_h_
#define _hx_fnccheck_h_


/* the output file header */
/* I need to create a STAT-FILE version number.
   different versions of fnccheck or dump will so
   be able to work together if the stat file version
   is the same. ("fnccheck_file_%s", FNCCHK_FILE_VERSION) */
#define FNCCHK_HEADER "fnccheck_file_1.1.5"

#define FNCCHK_DEFAULT_FILE "./fnccheck.out"

/* version number */
#define FNCCHK_VERSION "1.1"
#define FNCCHK_MINOR   "1"
#define FNCCHK_MAJOR   "1"
/* full (devel) version */
#define FNCCHK_FULL_VERSION "1.1.5"

/* sorting modes for hx_fnccheck_dump */
#define FNCCHK_SORT_NDEF  0
#define FNCCHK_SORT_MIN   1
#define FNCCHK_SORT_LOCAL 1  /* sort by local time */
#define FNCCHK_SORT_TOTAL 2  /* sort by total time */
#define FNCCHK_SORT_CALLS 3  /* sort by number of calls */
#define FNCCHK_SORT_NAME  4  /* sort by function name */
#define FNCCHK_SORT_NO    5  /* dont sort */
#define FNCCHK_SORT_MAX   5
/* use -(sort_type) to reverse sort */


/* options to be ORed */
#define FNCCHK_NONE              (0)
  /* dont display spontaneous called fncts (n/a main) */
#define FNCCHK_NO_SPONTANEOUS    (1 << 0)
  /* dont display unreferenced symbols */
#define FNCCHK_NO_UNDEF          (1 << 1)
  /* displays functions 'calls' instead of 'called by' */
#define FNCCHK_CALLS             (1 << 2)
  /* displays functions 'calls' AND 'called by' */
#define FNCCHK_CALLS_CALLED      (1 << 3)
  /* dont display MIN/MAX stats */
#define FNCCHK_NO_MINMAX         (1 << 4)
  /* use 'nm' instead of 'addr2line' */
#define FNCCHK_USE_NM            (1 << 5)
  /* display file:line for functions */
#define FNCCHK_FUNC_MORE         (1 << 6)
  /* display file:line for function calls */
#define FNCCHK_CALLS_MORE        (1 << 7)
  /* if given, use function and childs only */
#define FNCCHK_USE_CHILDS        (1 << 8)
  /* if given, use function and ancestror only */
#define FNCCHK_USE_CALLERS       (1 << 9)
  /* use 'addr2line' instead of 'libbfd' */
#define FNCCHK_USE_ADDR2LINE     (1 << 10)
  /* get files fullname */
#define FNCCHK_FULLNAME          (1 << 11)
  /* compute MAX time with existing functions */
#define FNCCHK_MAXTIME_EFF       (1 << 12)



/* names for sorts mode */
extern char *fncchk_sort_names[FNCCHK_SORT_MAX+2];


#endif /* _hx_fnccheck_h_ */
