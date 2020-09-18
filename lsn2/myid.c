#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXLEN 100


typedef struct group Group;
typedef struct passwd Passwd;


typedef struct Info
{
    Group * gr;
    Passwd * pwd;

    int lst_len;
    gid_t * gid_lst;
} Info;


void print_info( Info * usr )
{
    printf("uid=%d(%s) ", usr->pwd->pw_uid, usr->pwd->pw_name);
    printf("gid=%d(%s) ", usr->gr->gr_gid,  usr->gr->gr_name);
    printf("groups=");

    for (int i = 0; i < usr->lst_len; ++i)
    {
        Group * gr_i = getgrgid(usr->gid_lst[i]);
        printf("%d(%s)%s", usr->gid_lst[i], gr_i->gr_name,
                                  (i + 1 == usr->lst_len) ? "\n" : ",");
    }
}

bool get_custom( char * arg, Info * usr )
{
    if (isdigit(arg[0]))
    {
        uid_t uid = strtol(arg, NULL, 10);
        usr->pwd = getpwuid(uid);
    }
    else
        usr->pwd = getpwnam(arg);


    if (usr->pwd == NULL)
    {
        printf("%s: no such user\n", arg);
        return false;
    }

    usr->gr = getgrgid(usr->pwd->pw_gid);

    if (usr->gr == NULL)
        return false;

    usr->lst_len = getgrouplist(usr->pwd->pw_name,
                                usr->pwd->pw_gid,
                                usr->gid_lst,
                                &(usr->lst_len));

    return true;
}

bool get_cur( Info * usr )
{
    usr->pwd = getpwuid(getuid());
    usr->gr  = getgrgid(getgid());

    if (usr->pwd == NULL || usr->gr == NULL)
    {
        printf("I can't get information about you\n");
        return false;
    }

    usr->lst_len = getgroups(MAXLEN, usr->gid_lst);

    return true;
}

int main( int argc, char ** argv )
{
    Info usr;
    bool condition = false;

    usr.lst_len = MAXLEN;
    usr.gid_lst = (gid_t *)calloc(MAXLEN, sizeof(gid_t));

    switch (argc)
    {
    case 1:
        condition = get_cur(&usr);
        break;

    case 2:
        condition = get_custom(argv[1], &usr);
        break;

    default:
        printf("Wrong number of agruments\n"
               "Exit...\n");
        condition = false;
        break;
    }

    if (!condition)
    {
        free(usr.gid_lst);
        exit(1);
    }

    print_info(&usr);

    free(usr.gid_lst);

    return 0;
}
