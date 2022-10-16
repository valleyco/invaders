# Must init the autoconf setup
# The first parameter is project name
# second is version number
# third is bug report address
AC_INIT([emu-8080], [1.0])

# Safety checks in case user overwritten --srcdir
AC_CONFIG_SRCDIR([src/main.c])

# Store the auxiliary build tools (e.g., install-sh, config.sub, config.guess)
# in this dir (config)
 AC_CONFIG_AUX_DIR(config)
# Init automake, and specify this program use relaxed structures.
# i.e. this program doesn't follow the gnu coding standards, and doesn't have
# ChangeLog, COPYING, AUTHORS, INSTALL, README etc. files.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C/C++ compiler
AC_PROG_CC
#AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
#LT_INIT
AC_CONFIG_MACRO_DIRS([m4])
# We can add more checks in this section

# Tells automake to create a Makefile
# See https://www.gnu.org/software/automake/manual/html_node/Requirements.html
#AC_CONFIG_FILES([Makefile src/Makefile dip/Makefile])
AC_CONFIG_FILES([Makefile 8080/src/Makefile src/Makefile])

PKG_CHECK_MODULES([GTK3],[gtk+-3.0], [], [AC_MSG_ERROR([GTK not in path])])

# Generate the output
AC_OUTPUT
