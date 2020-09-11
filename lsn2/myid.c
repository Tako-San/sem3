#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
// #include <stdbool.h>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXLEN 100

void print_info( uid_t uid, gid_t gid, int counter,  gid_t * gid_lst)
{
    struct group * gr = getgrgid(gid);
    struct passwd * pwd = getpwuid(uid);

    // assert(gr != NULL && pwd != NULL && counter >= 0);

    printf("uid=%d(%s) ", uid, pwd->pw_name);
    printf("gid=%d(%s) ", gid, gr->gr_name);
    printf("groups=");

    for (int i = 0; i < counter; ++i)
    {
        gid_t gid_i = gid_lst[i];
        struct group * gr_i = getgrgid(gid_i);

        printf("%d(%s)%s", gid_i, gr_i->gr_name, (i + 1 == counter) ? "" : ",");
    }

    printf("\n");
}



int main( int argc, char ** argv )
{
    struct passwd * pwd;

    uid_t uid;
    gid_t gid;

    gid_t gid_lst[MAXLEN];
    int counter = MAXLEN;

    if (argc == 2)
    {
        if (isdigit(argv[1][0]))
            pwd = getpwuid(strtol(argv[1]));
        else
            pwd = getpwnam(argv[1]);

        if (pwd == NULL)
        {
            printf("%s: no such user\n", argv[1]);
            exit(1);
        }

        uid = pwd->pw_uid;
        gid = pwd->pw_gid;
        counter = getgrouplist(pwd->pw_name, pwd->pw_gid, gid_lst, &counter);
    }
    else if (argc == 1)
    {
        uid = getuid();
        gid = getgid();
        counter = getgroups(MAXLEN, gid_lst);
    }
    else
    {
        printf("Wrong number of agruments\n"
               "Exit...\n");
        exit(1);
    }

    print_info(uid, gid, counter, gid_lst);

    return 0;
}