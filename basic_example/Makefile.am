bin_PROGRAMS = basic_example
basic_example_SOURCES = basic_example.c
basic_example_LDFLAGS = -L$(abs_top_builddir)/src/.libs -lcprest
if JSON
bin_jsondir = $(bin_dir)
bin_json_PROGRAMS = basic_example_json
basic_example_json_SOURCES = basic_example_json.c
basic_example_json_CPPFLAGS = $(JANSSON_CFLAGS) -I $(abs_top_builddir)/src
basic_example_json_LDFLAGS = -L$(abs_top_builddir)/src/.libs -lcprest -ljansson
basic_example_CPPFLAGS = $(JANSSON_CFLAGS) -I $(abs_top_builddir)/src
else
basic_example_CPPFLAGS = -I $(abs_top_builddir)/src
endif
