/*
Copyright 2014 CloudPlugs Inc.

Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/
#ifndef CP_INTERNALS_H
#define CP_INTERNALS_H

#include <stdarg.h>
#include <curl/curl.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * Used  HTTP methods
 */

enum _CP_HTTP_METHOD { CP_HTTP_GET, CP_HTTP_POST, CP_HTTP_PUT, CP_HTTP_DELETE, CP_HTTP_PATCH };
typedef enum _CP_HTTP_METHOD CP_HTTP_METHOD;

#define SET_ERROR_AND_RETURN(cps, x) { if(cps) cps->err = x; return CP_FAIL; }


/**
 * Data structure to handle a request session
 */

struct _cloudplugs_session {
   CURL* curl;
   char* base_url;
   int timeout;
   char* id;
   char* auth;
   cp_bool is_master;
   CP_HTTP_RESULT http_res;
   CP_ERR_CODE err;
   cp_bool verify_ssl;
   char* ca;
};


/**
 * Utility function for string concatenation
 */
char* cloudplugs_concat(cp_session cps, int num, ... );

/**
 Execute a generic http request.

 @param cps The session reference.
 @param auth Check if auth is present.
 @param http_method Enum that indicate the desired action to be performed on the identified resource.
 @param path Relative path of the requested resource.
 @param headers If not NULL, is an array of string, last element must be NULL.
 @param query If not NULL, the string is append to the path after a '?' character.
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_request_exec(cp_session cps, cp_bool auth, CP_HTTP_METHOD http_method, const char* path, char* headers[], const char* query, const char* body, char** result, size_t* result_length);

/**
 * Get the authentication ID associated to the current session
 */
const char *cloudplugs_get_plug_id(cp_session cps);

void cloudplugs_internal_set_auth(cp_session cps, cp_res cp_res, char** result);

char* cloudplugs_url_encode_channel(cp_session cps, const char* channel_mask);

char* cloudplugs_url_encode_data(cp_session cps, const char* channel_mask);

char* cloudplugs_url_encode_prop(cp_session cps, const char* id, const char* prop);

#ifdef  __cplusplus
}
#endif

#endif // CP_INTERNALS_H
