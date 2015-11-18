/*-
 * Copyright (c) 1995-2015 Ederson de Moura <ederbsd@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 */

#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <err.h>     /* Functions display a formatted error message */
#include <termios.h> /* POSIX terminal control definitions */
#include <syslog.h>  /* Definitions for system error logging */
#include <stdlib.h>  /* General std functions */

#define PROG        "UpsDaemon"
#define VERSION     "1.1"

#define WAIT        5*60
#define PORT        "/dev/cuad0"
#define PID         "/var/run/upsdaemon.pid"

#define SH_BATTERY  "/usr/local/libexec/upsdaemon/upsdaemon-alerts -battery"
#define SH_DOWN	    "/usr/local/libexec/upsdaemon/upsdaemon-alerts -down"
#define SH_FAILED   "/usr/local/libexec/upsdaemon/upsdaemon-alerts -failed"
#define SH_RECOVERY "/usr/local/libexec/upsdaemon/upsdaemon-alerts -recovery"

int makepid(char *s);
static void runcommand();

int main(int argc, char **argv)
{
        volatile register int fd asm("eax");
        fd = 0;

        int lstatus = 0;	
        int rts_bit = TIOCM_RTS;
        int dtr_bit = TIOCM_DTR;
        int pfail = 0;
	
        volatile register int pfcount asm("eax");
        volatile register int prcount asm("eax");
        volatile register int blcount asm("eax");
        pfcount = 0; prcount = 0; blcount = 0;
	
        char *port = PORT;
        char *down      = SH_DOWN;
        char *battery   = SH_BATTERY;
        char *failed    = SH_FAILED;
        char *recovery  = SH_RECOVERY;

	if(argc > 2) {
		errx(EXIT_FAILURE, "Usage: upsdaemon <port serial open>: %s\n", strerror(errno));
	}

	if(argc == 2) {
		port = argv[1];
	}

	/*
	 *
	 * SGI Pinouts:
	 *        
	 *               	   \ o o o o o /
	 *                          \ o o o o /    
	 *                  
	 *                  Table SGI 9-Pin DSUB Signals              
	 * -----------------------------------------------------------------
	 * | 1 -> Earth Ground           | 6 -> DSR  - Data Set Ready      |
	 * | 2 -> TXD - Transmitted Data | 7 -> GND  - Logic Ground        |
	 * | 3 -> RXD - Received Data    | 8 -> DCD  - Data Carrier Detect |
	 * | 4 -> RTS - Request To Send  | 9 -> DTR  - Data Terminal Ready |
	 * | 5 -> CTS - Clear To Send    |                                 |
	 * -----------------------------------------------------------------
	 *              
	 */	
	if((fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {
		perror("Uspdaemon Error: Unable to open " PORT);
		exit(1);		
	}
	
	ioctl(fd, TIOCMBIC, &rts_bit);
	ioctl(fd, TIOCMBIS, &dtr_bit);
	
	if(daemon(1,1) < 0) {
		syslog(LOG_ERR, "Alert: It was not possible to be daemon: %m");
		exit(1);
	}

	/* Set PID */	
	(void)makepid(PID);
	
	while(1) {
		ioctl(fd, TIOCMBIC, &rts_bit);
		ioctl(fd, TIOCMBIC, &dtr_bit);
		ioctl(fd, TIOCMBIS, &dtr_bit);
		ioctl(fd, TIOCMGET, &lstatus);
               
		if(!(lstatus & TIOCM_CAR)) {
			
			++blcount;
			if(blcount == 6) {
				
			syslog(LOG_ALERT, "Alert: Without contact with Ups. %s %s", PROG, VERSION);
			
			ioctl(fd, TIOCMBIC, &dtr_bit);
			ioctl(fd, TIOCMBIS, &dtr_bit);
			ioctl(fd, TIOCMGET, &lstatus);
			
			if(!(lstatus & TIOCM_CTS)) {
				
				ioctl(fd, TIOCMBIC, &dtr_bit);
				ioctl(fd, TIOCMBIS, &dtr_bit);
				ioctl(fd, TIOCMBIS, &rts_bit);
				
				/* It fell in the battery Ups */
				runcommand(battery);
				sleep(100);
				}
			}
		}
		else if(!(lstatus & TIOCM_CTS)) {
			++pfcount;
			prcount = 0;
			blcount = 0;
			if(pfcount == 3) {				
				syslog(LOG_WARNING, "Alert: Failed in the communication with the Ups. %s %s", PROG, VERSION);
				
				/* Run failed communication Ups */
				runcommand(failed);
				pfail = 1;
			}
			
                        /* Time (wait - 5) minutes defined */			
			if(pfcount == WAIT) {
				syslog(LOG_ALERT, "Alert: Failed down server. %s %s", PROG, VERSION);
				
				ioctl(fd, TIOCMBIC, &dtr_bit);
				ioctl(fd, TIOCMBIS, &dtr_bit);
				ioctl(fd, TIOCMBIS, &rts_bit);
				
				/* Run shutdown server, now */
				runcommand(down);
				
				sleep(100);
			}
		}
		else if(pfail == 1) {
			
			++prcount;
			pfcount = 0;
			if(prcount == 3) {
				
				/* Run recovery energy */				
				runcommand(recovery);
				
				pfail = 0;
				blcount = 0;
				ioctl(fd, TIOCMBIC, &rts_bit);
				ioctl(fd, TIOCMBIC, &dtr_bit);
				ioctl(fd, TIOCMBIS, &dtr_bit);
			}
		else blcount = 0;
		}
	sleep(1);
	}
}

/* 
 * Check Routines Shell 
 */
void runcommand(shell_script)
char *shell_script;
{
	switch(fork()) {
	case 0:
		system(shell_script);
		exit(0);
	case -1:
		errx(EXIT_FAILURE, "Error: Not found the routine: %s", strerror(errno));
		break;
	default:
		break;
	}
}

/*
 * To Create PID
 */
int makepid(s)
char *s;
{
	FILE *fp;
	pid_t pid;

	pid = getpid();
	if((fp =  fopen(s, "w")) == NULL)
		return -1;
	fprintf(fp, "%lu\n", (u_long)pid);
	fclose(fp);
	return 0;
}
