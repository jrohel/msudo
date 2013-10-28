#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//#define CFG_FILE_NAME "/etc/msudo.conf"
#define CFG_FILE_NAME "msudo.conf"

int main(int argc, const char *argv[])
{
	FILE *cfgFile;
	const char *wantPrgName;
	char username[1024];
	char prgname[1024];
	uid_t my_uid;
	struct passwd *pw;

	if (argc!=2 || argv[1]=='\0')
	{
		fprintf(stdout, "msudo version 0.1     Jaroslav Rohel, 2013\n", argv[0]);
		fprintf(stdout, "Usage:\n");
		fprintf(stdout, "%s pathToPrgForExec\n\n", argv[0]);
		return -1;
	}

	// get my real name
	my_uid = getuid();
	pw = getpwuid(my_uid);
//	printf("%s\n", pw->pw_name);

	wantPrgName = argv[1];
	cfgFile = fopen(CFG_FILE_NAME, "r");
	if (cfgFile)
	{
		int found = 0;
		while (fscanf(cfgFile, "%1023s%*10000[ \t]%1023s%*10000[ \t\n\r]", username, prgname) >= 2)
		{
//			printf("UserName: %s\n", username);
//			printf("PrgName: %s\n", prgname);
			if (strcmp(username, pw->pw_name)==0 && strcmp(wantPrgName, prgname)==0)
			{
				found = 1;
				break;
			}
		}
		fclose(cfgFile);
		if (found)
		{
			printf("Starting\n");
			system(wantPrgName);
		}
		else fprintf(stdout, "User \"%s\" is not allowed to start program %s\n", pw->pw_name, wantPrgName);
	}
	else fprintf(stderr, "Can't open config file %s\n", CFG_FILE_NAME);
	return 0;
}
