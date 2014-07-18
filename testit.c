/*  $Revision: 1.4 $
**
**  A "micro-shell" to test editline library.
**  If given any arguments, commands aren't executed.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char	*readline();
extern void	add_history();



int
main(ac, av)
    int		ac;
    char	*av[];
{
    char	*prompt;
    char	*p;
    int		doit;

    doit = ac == 1;
    if ((prompt = getenv("TESTPROMPT")) == NULL)
	prompt = "testit>  ";

    while ((p = readline(prompt)) != NULL) {
	(void)printf("\t\t\t|%s|\n", p);
	if (doit) {
	    if (strncmp(p, "cd ", 3) == 0) {
		if (chdir(&p[3]) < 0)
		    perror(&p[3]);
	    }
	    else if (system(p) != 0)
		perror(p);
	}
	add_history(p);
	free(p);
    }
    exit(0);
    /* NOTREACHED */
}
