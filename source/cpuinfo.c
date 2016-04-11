/*
Copyright (c) 2015 Lemaker Team

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include "cpuinfo.h"

int f_a20=0;
int f_s500=0;

int is_a20_platform(void)
{
	return (1 == f_a20) ? 1 : 0;
}

int is_s500_platform(void)
{
	return(1== f_s500) ? 1 : 0;
}

int get_lmk_revision(void)
{
	if (is_a20_platform()) 
       {
		return BANANAPRO;
	} 
       else if (is_s500_platform())
       {
		return LEMAKER_GUITAR;
	}
	
	return -1;
}

char *get_cpuinfo_revision(char *revision)
{
	FILE *fp;
	char buffer[1024];
	char hardware[1024];
	int  findID = 0;

	if ((fp = fopen("/proc/cpuinfo", "r")) == NULL)
		return 0;

	while(!feof(fp)) {
		if(fgets(buffer, sizeof(buffer) , fp) != NULL)
		{
			sscanf(buffer, "Hardware	: %s", hardware);
			if (strcmp(hardware, "sun7i") == 0) {
				f_a20=1;
				findID = 1;
			}else if (strcmp(hardware, "gs705a") == 0){
				f_s500 = 1;
				findID = 1;
			}
			sscanf(buffer, "Revision	: %s", revision);
		}
	}
	fclose(fp);

	if (!findID)
		revision = NULL;
	
	return revision;
}

