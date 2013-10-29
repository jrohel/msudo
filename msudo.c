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

int main(int argc, const char *argv[])
{
	FILE *cfgFile;
	uid_t my_uid;
	struct passwd *pw;

	if (argc!=2 || argv[1]=='\0')
	{
		fprintf(stdout, "msudo version 0.2     Jaroslav Rohel, 2013\n\n", argv[0]);
		fprintf(stdout, "Usage:\n");
		fprintf(stdout, "%s pathToPrgForExec_with_arguments\n\n", argv[0]);
		fprintf(stdout, "Example:\n");
		fprintf(stdout, "%s \"/usr/sbin/arping -i eth0 192.168.10.100\"\n\n", argv[0]);
		return -1;
	}

	// get my real name
	my_uid = getuid();
	pw = getpwuid(my_uid);

	cfgFile = fopen(CFG_FILE_NAME, "r");
	if (cfgFile)
	{
		const char *wantPrg = argv[1];

		char username[1024];
		char pattern[1024];
//		char *pattern = "a[a-c]u";
		int found = 0;

		while (fscanf(cfgFile, "%1023s%*10000[ \t]%1023[^\n\r]%*10000[\n\r]", username, pattern) >= 2)
		{
//			printf("UserName: %s\n", username);
//			printf("PrgName: %s\n", prgname);
			if (strcmp(username, pw->pw_name) == 0)
			{
				regex_t preg;
				int rc;

				rc = regcomp(&preg, pattern, 0);
				if (rc)
				{
					fprintf(stderr, "regcomp() failed, returning nonzero (%d)\n", rc);
					break;
				}
				rc = regexec(&preg, wantPrg, 0, NULL, 0);
				regfree(&preg);
				if (!rc)
				{
					found = 1;
					break;
				}
			}
		}
		fclose(cfgFile);
		if (found)
		{
			printf("Starting\n");
			system(wantPrg);
		}
		else fprintf(stdout, "User \"%s\" is not allowed to start \"%s\"\n", pw->pw_name, wantPrg);
	}
	else fprintf(stderr, "Can't open config file %s\n", CFG_FILE_NAME);
	return 0;
}
