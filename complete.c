/*  $Revision: 1.5 $
**
**  History and file completion functions for editline library.
*/
#include "editline.h"


/*
**  strcmp-like sorting predicate for qsort.
*/
STATIC int
compare(p1, p2)
    const void	*p1;
    const void	*p2;
{
    char * const *v1;
    char * const *v2;

    v1 = (char * const *)p1;
    v2 = (char * const *)p2;
    return strcmp(*v1, *v2);
}

/*
**  Fill in *avp with an array of names that match file, up to its length.
**  Ignore . and .. .
*/
STATIC int
FindMatches(dir, file, avp)
    char	*dir;
    char	*file;
    char	***avp;
{
    char	**av;
    char	**new;
    char	*p;
    DIR		*dp;
    DIRENTRY	*ep;
    size_t	ac;
    size_t	len;

    if ((dp = opendir(dir)) == NULL)
	return 0;

    av = NULL;
    ac = 0;
    len = strlen(file);
    while ((ep = readdir(dp)) != NULL) {
	p = ep->d_name;
	if (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0')))
	    continue;
	if (len && strncmp(p, file, len) != 0)
	    continue;

	if ((ac % MEM_INC) == 0) {
	    if ((new = NEW(char*, ac + MEM_INC)) == NULL)
		break;
	    if (ac) {
		COPYFROMTO(new, av, ac * sizeof (char **));
		DISPOSE(av);
	    }
	    *avp = av = new;
	}

	if ((av[ac] = strdup(p)) == NULL) {
	    if (ac == 0)
		DISPOSE(av);
	    break;
	}
	ac++;
    }

    /* Clean up and return. */
    (void)closedir(dp);
    if (ac)
	qsort(av, ac, sizeof (char **), compare);
    return ac;
}

/*
**  Split a pathname into allocated directory and trailing filename parts.
*/
STATIC int
SplitPath(path, dirpart, filepart)
    char	*path;
    char	**dirpart;
    char	**filepart;
{
    static char	DOT[] = ".";
    char	*dpart;
    char	*fpart;

    if ((fpart = strrchr(path, '/')) == NULL) {
	if ((dpart = strdup(DOT)) == NULL)
	    return -1;
	if ((fpart = strdup(path)) == NULL) {
	    DISPOSE(dpart);
	    return -1;
	}
    }
    else {
	if ((dpart = strdup(path)) == NULL)
	    return -1;
	dpart[fpart - path + 1] = '\0';
	if ((fpart = strdup(++fpart)) == NULL) {
	    DISPOSE(dpart);
	    return -1;
	}
    }
    *dirpart = dpart;
    *filepart = fpart;
    return 0;
}

/*
**  Attempt to complete the pathname, returning an allocated copy.
**  Fill in *unique if we completed it, or set it to 0 if ambiguous.
*/
char *
rl_complete(pathname, unique)
    char	*pathname;
    int		*unique;
{
    char	**av;
    char	*dir;
    char	*file;
    char	*new;
    char	*p;
    size_t	ac;
    size_t	end;
    size_t	i;
    size_t	j;
    size_t	len;

    if (SplitPath(pathname, &dir, &file) < 0)
	return NULL;
    if ((ac = FindMatches(dir, file, &av)) == 0) {
	DISPOSE(dir);
	DISPOSE(file);
	return NULL;
    }

    p = NULL;
    len = strlen(file);
    if (ac == 1) {
	/* Exactly one match -- finish it off. */
	*unique = 1;
	j = strlen(av[0]) - len + 2;
	if ((p = NEW(char, j + 1)) != NULL) {
	    COPYFROMTO(p, av[0] + len, j);
	    if ((new = NEW(char, strlen(dir) + strlen(av[0]) + 2)) != NULL) {
		(void)strcpy(new, dir);
		(void)strcat(new, "/");
		(void)strcat(new, av[0]);
		rl_add_slash(new, p);
		DISPOSE(new);
	    }
	}
    }
    else {
	/* Find largest matching substring. */
	for (*unique = 0, i = len, end = strlen(av[0]); i < end; i++)
	    for (j = 1; j < ac; j++)
		if (av[0][i] != av[j][i])
		    goto breakout;
breakout:
	if (i > len) {
	    j = i - len + 1;
	    if ((p = NEW(char, j)) != NULL) {
		COPYFROMTO(p, av[0] + len, j);
		p[j - 1] = '\0';
	    }
	}
    }

    /* Clean up and return. */
    DISPOSE(dir);
    DISPOSE(file);
    for (i = 0; i < ac; i++)
	DISPOSE(av[i]);
    DISPOSE(av);
    return p;
}

/*
**  Return all possible completions.
*/
int
rl_list_possib(pathname, avp)
    char	*pathname;
    char	***avp;
{
    char	*dir;
    char	*file;
    int		ac;

    if (SplitPath(pathname, &dir, &file) < 0)
	return 0;
    ac = FindMatches(dir, file, avp);
    DISPOSE(dir);
    DISPOSE(file);
    return ac;
}
