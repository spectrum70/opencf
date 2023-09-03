bin_PROGRAMS = opencf
opencf_CXXFLAGS = -I$(top_srcdir)/include
opencf_LDFLAGS = $(LIBUSB1_LIBS)
opencf_SOURCES = src/main.cc \
		 src/core.cc \
	         src/trace.cc \
		 src/getopts.cc \
		 src/utils.cc \
		 src/fs.cc \
		 src/bdm.cc \
		 src/parser.cc \
		 src/elf.cc \
		 src/drivers/driver-core.cc \
		 src/drivers/driver-pemu.cc

