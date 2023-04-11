#include <libgen.h>
#include <string.h>
#include "utils.h"
#include <stdlib.h>
#include<stdio.h>
void set_app_folder(const char *app, char *folder)
{
    char *ts1 = strdup(app);
    char *ts2 = strdup(app);

    char *dir = dirname(ts1);
    char *filename = basename(ts2);
    free(ts1);
    free(ts2);
}