
/*------------------------------------------------------------------
   PRINTDC.C -- Function to open device context for default printer
  ------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <mt\string.h>   // note: multithread header file
#pragma pack(1)          // align structure fields on 1-byte boundaries

HDC OpenDefaultPrinterDC (HAB hab)
     {
     static CHAR     achPrnData[256] ;
     static DRIVDATA driv = { sizeof (DRIVDATA) } ;
     CHAR            achDefPrnName[34], *pchDelimiter ;
     DEVOPENSTRUC    dop ;

               // Obtain default printer name and remove semicolon

     WinQueryProfileString (hab, "PM_SPOOLER", "PRINTER", ";",
                            achDefPrnName, sizeof achDefPrnName) ;

     if ((pchDelimiter = strchr (achDefPrnName, ';')) != NULL)
          *pchDelimiter = '\0' ;

     if (achDefPrnName[0] == '\0')
          return DEV_ERROR ;

               // Obtain information on default printer

     WinQueryProfileString (hab, "PM_SPOOLER_PRINTER", achDefPrnName,
                            ";;;;", achPrnData, sizeof achPrnData) ;

               // Parse printer information string

     if ((pchDelimiter = strchr (achPrnData, ';')) == NULL)
          return DEV_ERROR ;

     dop.pszDriverName = pchDelimiter + 1 ;

     if ((pchDelimiter = strchr (dop.pszDriverName, ';')) == NULL)
          return DEV_ERROR ;

     dop.pszLogAddress = pchDelimiter + 1 ;

     *(dop.pszLogAddress + strcspn (dop.pszLogAddress, ",;")) = '\0' ;
     *(dop.pszDriverName + strcspn (dop.pszDriverName, ",;")) = '\0' ;

               // Fill DRIVDATA structure if necessary

     if ((pchDelimiter = strchr (dop.pszDriverName, '.')) != NULL)
          {
          *pchDelimiter = '\0' ;
          strncpy (driv.szDeviceName, pchDelimiter + 1,
                                      sizeof (driv.szDeviceName)) ;
          dop.pdriv = &driv ;
          }
     else
          dop.pdriv = NULL ;

               // Set data type to "raw"

     dop.pszDataType = "PM_Q_RAW" ;

               // Open printer device context

     return DevOpenDC (hab, OD_QUEUED, "*", 4L, (PDEVOPENDATA) &dop, NULL) ;
     }
