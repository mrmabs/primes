/* Simple program go find prime numbers 
 *
 * GPL 2.0, 2004, 2005 Marcus Berglund.
 *
 * v2.1: took out primeness test, as it becomes
 * 	 a spiraling loop that can waste cycles.
 * 	 Estimated improvement, 30x.
 *
 * v2.0: improved prime search by only needing 
 *   bitwise %, considered fmodl & remainderl, 
 *   but both may not give speed improvement.
 *   Also only %-ing by prime number.
 *   Estimated improvement, 30x.
 * 
 * v1.0: used / & * algo to test for primeness
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#define SAVFILE		"primes.sav"
#define PRIMESFILE	"primesplus.dat"
#define BLOCK		10000000

extern int errno;
long primecnt = 0;
long blockstarttime;
unsigned long long whichblock;

/* progress bar:
 * numbers (0-100%) take up 22 chars,
 * we assume width is 80 chars,
 * leaves 58 chars for dots,
 * 5 dots per 10% = 50 dots,
 * % signs = 10 chars,
 * no 0? means,
 * 9 % chars, and no 0 char (21 chars)
 */
void progressbar(unsigned long long number) {
	unsigned long long block;
	long	blockpart;
	short	percent;
	short	i;
	char	str[256] = "";
	short	strptr = 0;
	//char	spinchars[] = {'/', '-', '\\', '|'};
	//short	spin;

	block = number / BLOCK;
	blockpart = number % BLOCK;
	//spin = blockpart % sizeof(spinchars);

	if(block == whichblock) {
		percent = (blockpart * 100) / BLOCK;
//		if(fmod(number, (blockpart * 100) / BLOCK) == 0) {
		for(i = 0; i < percent; i = i + 2) {
			switch (i) {
				case 10:
					str[strptr++] = '1';
					str[strptr++] = '0';
					break;
				case 20:
					str[strptr++] = '2';
					str[strptr++] = '0';
					break;
				case 30:
					str[strptr++] = '3';
					str[strptr++] = '0';
					break;
				case 40:
					str[strptr++] = '4';
					str[strptr++] = '0';
					break;
				case 50:
					str[strptr++] = '5';
					str[strptr++] = '0';
					break;
				case 60:
					str[strptr++] = '6';
					str[strptr++] = '0';
					break;
				case 70:
					str[strptr++] = '7';
					str[strptr++] = '0';
					break;
				case 80:
					str[strptr++] = '8';
					str[strptr++] = '0';
					break;
				case 90:
					str[strptr++] = '9';
					str[strptr++] = '0';
					break;
				default:
					str[strptr++] = '.';
			}
		}
		//str[strptr++] = spinchars[spin];
		str[strptr++] = '\0';
		printf("\r%s", str);
//		}
	} else {
		long blocktime = time(NULL) - blockstarttime;
		printf("100%%\nTime (secs): %ld, Primes: %ld\n\n",
				blocktime, primecnt);
		printf("Starting Block %lld\n", block * BLOCK);
		blockstarttime = time(NULL);
		primecnt = 0;
		whichblock = block;
	}

	fflush(stdout);

	return;
}

int saveprime(unsigned long long number) {
	int		fd;
	short	wdata;
	char	wstr[256];
	short	wstrlen;

	fd = open(PRIMESFILE, O_WRONLY|O_APPEND|O_CREAT, 0640);
	if(fd == -1) {
		printf("Unable to open primes file: [%s]\n", strerror(errno));
		return(-1);
	}

	sprintf(wstr, "%lld\n", number);
	wstrlen = strlen(wstr);
	wdata = write(fd, wstr, wstrlen);
	if(wdata < 1) {
		printf("Error writing to primes file: [%s]\n", strerror(errno));
		return(-1);
	}

	close(fd);
	return(1);
}

int savedata(unsigned long long number) {
	int		fd;
	int		wdata;
	char	wstr[256];
	short	wstrlen;

	fd = open(SAVFILE, O_WRONLY|O_TRUNC, 0640);
	if(fd == -1) {
		printf("Unable to open savedata file: [%s]\n", strerror(errno));
		return(-1);
	}

	sprintf(wstr, "last=%lld\n", number);
	wstrlen = strlen(wstr);
	wdata = write(fd, wstr, wstrlen);
	if(wdata < 5) {
		printf("Error writing to savedata file: [%s]\n", strerror(errno));
		return(-1);
	}

	close(fd);
	return(1);
}

int loaddata(unsigned long long *number) {
	int     fd;
	int     rdata;
	char    rstr[256];

	fd = open(SAVFILE, O_RDONLY);
	if(fd == -1) {
		printf("Unable to open savedata file: [%s]\n", strerror(errno));
		return(-1);
	}

	rdata = read(fd, rstr, 255);
	if(rdata < 5) {
		printf("Error reading savedata file: [%s]\n", strerror(errno));
		return(-1);
	}
	sscanf(rstr, "last=%lld\n", number);

	close(fd);
	return(1);
}

// New and improved
int isprime(unsigned long long number) {
	unsigned long long	a, max;

	max = sqrt(number);

	for(a = 2; a < max; a++) {
		//if(isprime(a) == 1) {
			if((number % a) == 0)
				return 0;
		//}
	}

	return 1;
}

int main(void) {
	unsigned long long number = 1;

	if(loaddata(&number) < 0) {
		printf("Exiting due to error, last number processed: %lld\n", number);
		return(1);
	}
	printf("Starting from %lld\n", number);

	blockstarttime = time(NULL);
	progressbar((floor(number / BLOCK) * BLOCK) + 1);

	for(number = number + 2; number < -1; number = number + 2) {
		//double	n;

		//n = ((number + 1.0) / 6.0);
		if(isprime(number) != 0) {
			primecnt++;
			if(saveprime(number) < 0) {
				printf("Exiting due to error, last number processed: %lld\n", number);
				return(1);
			}
			if(savedata(number) < 0) {
				printf("Exiting due to error, last number processed: %lld\n", number);
				return(1);
			}
			//printf("%lld\n", number);
			progressbar(number);
		}
	}

	printf("\n");

	return(0);
}
