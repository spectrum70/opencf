AC_PREREQ([2.69])
AC_INIT([opencf], [0.1], [angelo@kernel-space.org], [openocf], [http://www.kernel-space.org/])
AM_INIT_AUTOMAKE(subdir-objects)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CHECK_TOOL(CC, gcc, gcc)
AC_CHECK_TOOL(CXX, g++, g++)

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno"; then
	AC_MSG_ERROR([You need to install pkg-config])
fi

CXXFLAGS="-O3 --pipe -Wall"

if test -z "$CXXFLAGS"; then
	CXXFLAGS="-O3 --pipe -Wall"
fi

CXXFLAGS="$CXXFLAGS -Wno-deprecated-declarations"

AC_CONFIG_FILES([Makefile])

# Checks for libraries.

PKG_CHECK_MODULES([LIBUSB1], [libusb-1.0], [
	use_libusb1=yes
	AC_DEFINE([HAVE_LIBUSB1], [1], [Define if you have libusb-1.x])
	PKG_CHECK_EXISTS([libusb-1.0 >= 1.0.26],
			 [AC_DEFINE([HAVE_LIBUSB_ERROR_NAME],
			 [1], [Define if your libusb has libusb_error_name()])],
			 [AC_MSG_WARN([libusb-1.x older than 1.0.26 detected,
				      consider updating])])
	LIBUSB1_CFLAGS=`echo $LIBUSB1_CFLAGS | sed 's/-I/-isystem /'`
	AC_MSG_NOTICE([libusb-1.0 header bug workaround: LIBUSB1_CFLAGS changed to "$LIBUSB1_CFLAGS"])
	PKG_CHECK_EXISTS([libusb-1.0 >= 1.0.26],
			 [AC_DEFINE([HAVE_LIBUSB_GET_PORT_NUMBERS], [1], [Define if your libusb has libusb_get_port_numbers()])])
], [
	use_libusb1=no
	AC_MSG_WARN([libusb-1.x not found, trying legacy libusb-0.1 as a fallback; consider installing libusb-1.x instead])
])


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h sys/ioctl.h termios.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_OUTPUT
