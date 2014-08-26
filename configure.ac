AC_INIT([libcprest], [1.0], [bug@cloudplugs.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
m4_pattern_allow([AM_PROG_AR])
AC_CONFIG_MACRO_DIR([m4])
PKG_CHECK_MODULES(CURL, libcurl >= 7.0)
AC_ARG_ENABLE([json],
[  --enable-json    enabled the dependence from the Jansson library],
[case "${enableval}" in
  yes) json=true ;;
  no)  json=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-json]) ;;
esac],[json=true])
AM_CONDITIONAL([JSON], [test "x$json" = xtrue])
AM_COND_IF([JSON], [PKG_CHECK_MODULES(JANSSON, jansson >= 2.4,,[AC_MSG_ERROR([Jansson library not found, run ./configure --enable-json=no or install it])])])
AM_PROG_AR
AC_PROG_CC
PKG_PROG_PKG_CONFIG
LT_INIT
AC_ENABLE_SHARED
AC_ENABLE_STATIC
AC_CONFIG_FILES([
	Makefile
	src/Makefile
	basic_example/Makefile
])
AC_CONFIG_FILES(cprest.pc)
AC_OUTPUT
