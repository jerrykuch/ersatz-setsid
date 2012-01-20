#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 *  An ersatz setsid command for those Unixes (e.g. Mac OS X) that
 *  don't have one.  Intended to work like (or enough like) the one
 *  from the util-linux-ng package that I can run the RabbitMQ test
 *  suite post summer 2011 on a Mac OS X dev machine.
 *
 *  Synopsis:
 *      setsid program [arg...]
 *
 *  Description:
 *      setsid runs a program in a new session.
 *
 *  See Also:
 *      setsid(2)
 *
 */
int
main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        fprintf(stderr,
                "usage: %s program [arg ...]\n",
                (char *)argv[0]);
    }

    // Are we the leader of a process group (i.e. is our process id
    // equal to our process group id?)?
    if (getpgrp() == getpid())
    {

        switch(fork())
        {
            case 0:
                // We're in the child process...
                break;
            case -1:
                // We're in the parent process and child creation
                // failed, so squawk and exit with error code...
                perror("fork");
                exit(1);
            default:
                // We're in the parent process (setsid itself), child
                // creation was OK...
                exit(0);
        }
    }

    // Create a new session and make this setsid process the session
    // leader for the new session; This setsid process also becomes
    // the process group leader of a new process group and has no
    // controlling terminal...
    if (setsid() < 0)
    {
        perror("setsid");
        exit(1);
    }

    // Execute the requested command with the given arguments,
    // replacing our current process image (which now has its own
    // happy session and group) with a new process image...
    execvp(argv[1], argv + 1);

    // If we got to here, squawk and exit with an error code...
    perror("execvp");
    exit(1);
}
