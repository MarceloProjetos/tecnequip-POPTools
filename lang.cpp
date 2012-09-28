#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "poptools.h"

typedef struct LangTableTag {
    char *from;
    char *to;
} LangTable;

typedef struct LangTag {
    LangTable   *tab;
    int          n;
} Lang;

// These are the actual translation tables, so should be included in just
// one place.
#include "lang-tables.h"

char *_(char *in)
{
    Lang *l;

#if defined(LDLANG_EN)
    return in;
#elif defined(LDLANG_DE)
    l = &LangDe;
#elif defined(LDLANG_FR)
    l = &LangFr;
#elif defined(LDLANG_ES)
    l = &LangEs;
#elif defined(LDLANG_IT)
    l = &LangIt;
#elif defined(LDLANG_TR)
    l = &LangTr;
#elif defined(LDLANG_PT)
    l = &LangPt;
#else
#   error "Unrecognized language!"
#endif

    int i;

    for(i = 0; i < l->n; i++) {
        if(_stricmp(in, l->tab[i].from)==0) {
            return l->tab[i].to;
        }
    }

#ifdef _DEBUG
//	Error("No match found: '%s'", in);
#endif
	return in;
}
