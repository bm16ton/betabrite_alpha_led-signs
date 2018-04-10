/*
   betabrite.c -- command line utility to display a message on a BetaBrite LED sign
 
   Copyright (C) 2002 boB Rudis, bob@rudis.net
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "betabrite.h"

#define DEFAULT_SERIAL_PORT "/dev/ttya" /* Solaris */

/* OpenPort() - try to open the serial port */

int OpenPort(char *serial_port) {

   int fd ;

   fd = open(serial_port, O_RDWR|O_NOCTTY|O_NDELAY) ;

   if (fd == -1) {
      perror("OpenPort: Unable to open port - ") ;
   } else {
      fcntl(fd,F_SETFL,0) ;
   } /* if */
   
   return(fd) ;

} /* OpenPort() */


/* SetupSerial() - open the serial port and setup comm parameters */

int SetupSerial(char *serial_port) {

   struct termios options ;

   int fd = OpenPort(serial_port) ;

   tcgetattr(fd,&options) ;

   /* 9600 baud */

   cfsetispeed(&options,B9600) ;
   cfsetospeed(&options,B9600) ;

   options.c_cflag |= (CLOCAL|CREAD) ;

   tcsetattr(fd,TCSANOW,&options) ;

   /* 7 bits */

   options.c_cflag &= ~CSIZE ;
   options.c_cflag |= CS7 ;

   /* even parity */

   options.c_cflag |= PARENB ;
   options.c_cflag &= ~PARODD ;
   options.c_cflag &= ~CSTOPB ;
   options.c_cflag &= ~CSIZE ;
   options.c_cflag |= CS7 ;

   /* h/w flow */

   options.c_cflag |= CRTSCTS ;

   options.c_oflag &= ~OPOST;
   options.c_oflag &= ~ONLCR;
   options.c_oflag &= ~OCRNL;

   tcsetattr(fd,TCSANOW,&options);

   return(fd) ;

} /* SetupSerial() */


/* GetMessage() - use contents of 'msg_filename' as the message */

char *GetMessage(char *msg_filename) {

   struct stat statbuf ;
   char *msg = NULL ;

   int file_status = stat(msg_filename,&statbuf) ;

   if (file_status == 0) {

      long file_size = (long)statbuf.st_size ;

      FILE *msgfile = NULL ;

      int count = 0 ;

      msgfile = fopen(msg_filename,"r") ;

      if (msgfile == NULL) {
	 perror("GetMessage() : cannot open file -") ;
	 return NULL ;
      } /* if */

      msg = (char *)malloc(file_size+1) ;

      count = fread(msg,1,file_size,msgfile) ;

      if (count < 0) {
         if (msg) { free(msg) ; }
	 return(NULL) ;
      } else {
	 return(msg) ;
      } /* if */

   } else {

      perror("GetMessage() - cannot stat file - ") ;

   } /* if */


} /* GetMessage() */

void DisplayUsage() {
      
   printf("Usage: betabrite [-s speed] [-b memfile ] [-h] [-v]\n");
   printf("                 [-m text|-f filename] [-p port]\n\n");
   printf("Options:\n\n");
   printf(" -s speed	: how long to hold text; 0=no delay, 1-5=longest to shortest\n ");
   printf("          	  (default=1)\n") ;
   printf(" -b memfile	: BetaBrite memory file to store text; single char (e.g. \"C\")\n");
   printf("          	  (default=A)\n") ;
   printf(" -h		: this help screen\n");
   printf(" -v		: display input and status - if any\n") ;
   printf(" -m text	: text of message to display; (default - blank [""])\n");
   printf(" -f filename	: filename to read message from\n" );
   printf(" -p port	: serial port to use (e.g. \"/dev/ttya\")\n");
   printf("          	  (default=\"/dev/ttya\" on Solaris)\n\n") ;
   printf("Report bugs to betabrite-bugs@rudis.net\n" );

} /* DisplayUsage() */


/* main() - parse options, display message */

int main(int argc, char *argv[]) {

   int c ;
   extern char *optarg ;
   extern int optind ;

   char *betafile = NULL ;
   char *defaultbetafile = "A" ;

   char *msg = "" ;

   char *msgfilename = NULL ;

   char *serial_port = DEFAULT_SERIAL_PORT ;

   int speedsel = 1 ; /* 0 = none, 1-5 = slowest to fastest */
   char *holdspeed = SPEED_SLOWEST ;

   int verbose_flag = 0 ;
   int msgtext_flag = 0 ;
   int msgfile_flag = 0 ;
   int port_flag    = 0 ;
   int options      = 0 ;
 
   int errflag = 0 ;

   int n ;
   int fd ;

   while ((c = getopt(argc,argv,"p:s:b:m:f:hv")) != -1) {

      switch (c) {

         case 's' : speedsel = atoi(optarg) ;

                    switch (speedsel) {

                    case 0 : holdspeed = NO_HOLD_SPEED ; break ;
                    case 1 : holdspeed = SPEED_SLOWEST ; break ;
                    case 2 : holdspeed = SPEED_LOW ;     break ;
                    case 3 : holdspeed = SPEED_MEDIUM ;  break ;
                    case 4 : holdspeed = SPEED_HIGH ;    break ;
                    case 5 : holdspeed = SPEED_FASTEST ; break ;

                    default: errflag++ ; break ;
 
                    } /* switch */
                    
                    options++ ; break ;

         case 'p' : serial_port = optarg ; port_flag++ ;
                    options++ ; break ;

         case 'b' : betafile = optarg ;
                    if (strlen(betafile) > 1) { errflag++ ; } /* if */
                    options++ ; break ;

         case 'm' : msg = optarg ; msgtext_flag++ ;
                    options++ ; break ;

         case 'f' : msgfilename = optarg ; msgfile_flag++ ;
                    options++ ;break ;

         case 'h' : errflag++ ;
                    options++ ; break ;

         case 'v' : verbose_flag++ ;
                    break ;

         case '?' : errflag++ ;
                    break ;

         default  : errflag++ ;
                    break ;

      } /* switch */

   } /* while */

   if (!options) { errflag++ ; } /* if */

   /* cannot have a msg cmd line and a msg file */

   if ((msgtext_flag > 0) && (msgfile_flag > 0)) { errflag++ ; } /* if */

   if (msgfile_flag) {
      msg = GetMessage(msgfilename) ;
   } /* if */

  if (msg == NULL) { errflag++ ; } /* if */


   /* if we need help or if there's a cmd line error or a processing error... */

   if (errflag) {
      DisplayUsage() ;
      exit(2) ;
   } /* if */


   /* if no betabrite memory file is given, use the default */

   if (betafile == NULL) { betafile = defaultbetafile ; } /* if */


   /* say what we're doing if asked */

   if (verbose_flag) {
      printf("port: [%s]\nmsg: [%s]\nbetafile: [%s]\nspeed: [%d]\n",serial_port,msg,betafile,speedsel) ;
      if (msgfilename) {
         printf("msg file: [%s]\n",msgfilename) ;
      } /* if */
   } /* if */


   /* Setup serial communications */

   fd = SetupSerial(serial_port) ;


   /* Get the BetaBrite's attention */

   n = write(fd,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",20) ;


   /* Start of Header */

   n = write(fd,"\001",1) ;


   /* Type Code - 'Z' = 'All Signs' */

   n = write(fd, "Z",1) ;


   /* Sign Address - '00' */

   n = write(fd,"00",2) ;


   /* Start of Text */
   
   n = write(fd,"\002",1) ;


   /* Command Code - 'A' = 'Write TEXT' */

   n = write(fd,"A",1) ;


   /* File Label - e.g. 'A' */

   n = write(fd,&betafile[0],1) ;


   /* Command code info: ESCape SPACE 'a' */

   n = write(fd,"\x1B \x6F",3) ;
   n = write(fd,"\x07",1) ;


   /* Set hold speed (I don't trust the betabrite) */

   n = write(fd,holdspeed,1) ;
   n = write(fd,holdspeed,1) ;
   n = write(fd,holdspeed,1) ;
   n = write(fd,holdspeed,1) ;
   n = write(fd,holdspeed,1) ;


   /* just in case - no wide characters */

   n = write(fd,"\x11",1) ;


   /* set color of the whole line - will be more granular */

   n = write(fd,"\x1C\x43",2) ;


   /* The data */

   n = write(fd,&msg[0],strlen(msg)) ;


   /* End of transmission */

   n = write(fd,"\004",1) ;


   close(fd) ;

   return(0) ;

} /* main() */
