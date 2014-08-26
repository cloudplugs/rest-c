lib_LTLIBRARIES = libcprest.la
libcprest_ladir = $(includedir)
if JSON
libcprest_la_SOURCES = cp_internals.c cp_rest.c cp_rest_json.c
libcprest_la_HEADERS = cp_rest.h cp_rest_json.h
libcprest_la_CPPFLAGS = $(CURL_CFLAGS) $(JANSSON_CFLAGS)
libcprest_la_LDFLAGS = $(CURL_LIBS) $(JANSSON_LIBS)
else
libcprest_la_SOURCES = cp_internals.c cp_rest.c
libcprest_la_HEADERS = cp_rest.h
libcprest_la_CPPFLAGS = $(CURL_CFLAGS)
libcprest_la_LDFLAGS = $(CURL_LIBS)
endif
