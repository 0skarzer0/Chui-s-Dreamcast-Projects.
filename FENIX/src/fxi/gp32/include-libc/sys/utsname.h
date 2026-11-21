#ifndef _UTSNAME_H_
#define _UTSNAME_H_

struct utsname
  {
    /* Name of the implementation of the operating system.  */
    char sysname[8];

    /* Name of this node on the network.  */
    char nodename[8];

    /* Current release level of this implementation.  */
    char release[8];
    /* Current version level of this release.  */
    char version[8];

    /* Name of the hardware type the system is running on.  */
    char machine[8];
    char domainname[8];
  };


static inline int uname(struct utsname *name)
{
	name->sysname[0]='G';
	name->sysname[1]='P';
	name->sysname[2]='3';
	name->sysname[3]='2';
	name->sysname[4]=0;

	name->nodename[0]='0';
	name->nodename[1]=0;

	name->release[0]='2';
	name->release[1]='4';
	name->release[2]='6';
	name->release[3]=0;

	name->version[0]='2';
	name->version[1]=0;

	name->machine[0]='a';
	name->machine[1]=0;

	name->domainname[0]='b';
	name->domainname[1]=0;

	return 0;
}

#endif
