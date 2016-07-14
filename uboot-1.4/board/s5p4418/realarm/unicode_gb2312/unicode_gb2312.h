#ifndef UNICODE_GB2312_H
#define UNICODE_GB2312_H

#ifndef  USG_TYPE
# define USG_TYPE
  typedef unsigned char  USGC;
  typedef unsigned short USGS;
  typedef unsigned int   USGI;
  typedef unsigned long  USGL;
#endif

#ifndef  YES
# define YES             1
# define NO              0
#endif

#ifndef  TRUE
# define TRUE            1
# define FALSE           0
#endif

#ifndef  ON
# define ON              1
# define OFF             0
#endif

#ifndef  NUL
# define NUL             '\0'
#endif


int  cvt_Unicode2GB_one(USGC *strfrom, USGC *strto);
#endif
