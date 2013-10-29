/*
msudo - Execute a command as another user.

Jaroslav Rohel, 2013
email: jaroslav.rohel@ais-brno.cz
*/

#define CFG_FILE_NAME "/etc/msudo.conf"
//#define CFG_FILE_NAME "msudo.conf"

#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>

int main(int argc, char *argv[])
{
	FILE *cfgFile;
	uid_t my_uid;
	struct passwd *pw;

	if (argc<2 || argv[1]=='\0')
	{
		fprintf(stdout, "msudo version 0.3     Jaroslav Rohel, 2013\n\n", argv[0]);
		fprintf(stdout, "Usage:\n");
		fprintf(stdout, "%s pathToPrgForExec_with_arguments\n\n", argv[0]);
		fprintf(stdout, "Example:\n");
		fprintf(stdout, "%s /usr/sbin/arping -i eth0 192.168.10.100\n\n", argv[0]);
		return -1;
	}

	// get my real name
	my_uid = getuid();
	pw = getpwuid(my_uid);
//	printf("%s\n", pw->pw_name);

	cfgFile = fopen(CFG_FILE_NAME, "r");
	if (cfgFile)
	{
		const char *wantPrgName = argv[1];
		char username[1024];
		char prgname[1024];
		char prgargspattern[1024];
		char wantArgs[10000];
//		char *pattern = "a[a-c]u";
		int params;
		int found = 0;
		int exit = 0;

		wantArgs[0] = '\0';
		if (argc >= 3)
		{
			int i;
			int firstArg = 2;
			int freeLen = sizeof(wantArgs) - 1;
			for (i=firstArg; i<argc; i++)
			{
				int srcLen = strlen(argv[i]);
				if (srcLen+1 > freeLen)
				{
					fprintf(stderr, "Error: Too long arguments.\n");
					exit = 1;
					break;
				}
				if (i > firstArg)
				{
					strcat(wantArgs, " ");
					freeLen--;
				}
//				strcat(wantArgs, "\"");
				strncat(wantArgs, argv[i], srcLen);
//				strcat(wantArgs, "\"");
				freeLen -= srcLen;// + 2;
			}
		}

		while (!exit && (params=fscanf(cfgFile, "%1023s%*10000[ \t]%1023s%*10000[ \t]%1023[^\n\r]%*10000[ \t\n\r]", username, prgname, prgargspattern)) >= 2)
		{
//			printf("UserName: %s\n", username);
//			printf("PrgName: %s\n", prgname);
//			printf("pattern: %s\n", prgargspattern);
			if (strcmp(username, pw->pw_name)==0 && strcmp(wantPrgName, prgname)==0)
			{

				if (params==2 && argc==2)
				{
					found = 1;
					break;
				}
				else if (params > 2)
				{
					regex_t preg;
					int rc;

					rc = regcomp(&preg, prgargspattern, 0);
					if (rc)
					{
						fprintf(stderr, "regcomp() failed, returning nonzero (%d)\n", rc);
						break;
					}
					rc = regexec(&preg, wantArgs, 0, NULL, 0);
					regfree(&preg);
					if (!rc)
					{
						found = 1;
						break;
					}
				}
			}
		}
		fclose(cfgFile);
		if (found)
		{
			printf("Starting\n");
//			system(wantPrgName);
			execv(wantPrgName, &argv[1]);
		}
		else fprintf(stdout, "User \"%s\" is not allowed to start program \"%s\" with arguments \"%s\"\n", pw->pw_name, wantPrgName, wantArgs);
	}
	else fprintf(stderr, "Can't open config file %s\n", CFG_FILE_NAME);
	return 0;
}
