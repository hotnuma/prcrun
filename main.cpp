// https://stackoverflow.com/questions/6898337/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wordexp.h>

int pexec(const char *cmd)
{
    setbuf(stdout, nullptr);

    if (!cmd || !*cmd)
        return EXIT_FAILURE;

    wordexp_t p;
    wordexp(cmd, &p, 0);
    char **w = p.we_wordv;

    pid_t childpid = fork();

    if (childpid < 0)
    {
        // failed to fork

        wordfree(&p);

        return EXIT_FAILURE;
    }

    if (childpid == 0)
    {
        umask(0);
        setsid();

        if (chdir("/") < 0)
            exit(EXIT_FAILURE);

        // close out the standard file descriptors.
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        execve(w[0], (char**) w, __environ);

        exit(EXIT_FAILURE);
    }

    wordfree(&p);

    return EXIT_SUCCESS;
}

pid_t find_prc(const char *name)
{
    DIR *dir;

    if (!(dir = opendir("/proc")))
    {
        perror("can't open /proc");
        return -1;
    }

    struct dirent *ent;
    char *endptr;
    char buf[512];

    while ((ent = readdir(dir)) != NULL)
    {
        // if endptr is not a null character, the directory is not
        // entirely numeric, so ignore it

        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0')
        {
            continue;
        }

        // try to open the cmdline file

        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE *fp = fopen(buf, "r");

        if (fp)
        {
            if (fgets(buf, sizeof(buf), fp) != NULL)
            {
                // check the first token in the file, the program name

                char *first = strtok(buf, " ");

                char *pname = basename(first);
                if (pname)
                    first = pname;

                if (!strcmp(first, name))
                {
                    fclose(fp);
                    closedir(dir);

                    return (pid_t) lpid;
                }
            }

            fclose(fp);
        }

    }

    closedir(dir);

    return -1;
}

int main()
{
    const char *name = "compton";

    pid_t pid = find_prc(name);

    if (pid == -1)
    {
        printf("not found, execute compton\n");

        pexec("/usr/bin/compton");
    }
    else
    {
        printf("killall %s\n", name);

        system("killall compton");
    }

    return 0;
}


