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
#include "cp_rest.h"
#include "cp_internals.h"
#include "cp_constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

cp_res cloudplugs_global_init() {
    return curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK ? CP_OK : CP_FAIL;
}

cp_res cloudplugs_global_shutdown() {
    curl_global_cleanup();
    return CP_OK;
}

cp_res cloudplugs_set_base_url(cp_session cps, const char* url) {
    if(!cps || !url) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    int http = !strncmp(url, CP_HTTP_STR, LIT_STR_LEN(CP_HTTP_STR));
    int https = !strncmp(url, CP_HTTPS_STR, LIT_STR_LEN(CP_HTTPS_STR));
    if(http || https) {
        free(cps->base_url);
        cps->base_url = url[strlen(url)-1] == '/' ? cloudplugs_concat(cps, 1, url) : cloudplugs_concat(cps, 2, url, "/");
        return cps->base_url ? CP_OK : CP_FAIL;
    } else SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
}

const char* cloudplugs_get_base_url(cp_session cps) {
    if(!cps) return NULL;
    return cps->base_url ? cps->base_url : CP_URL;
}

cp_res cloudplugs_set_timeout(cp_session cps, int timeout) {
    if(cps && timeout >= 0) {
        cps->timeout = timeout ? timeout : CP_TIMEOUT;
        return CP_OK;
    }
    return CP_FAIL;
}

int cloudplugs_get_timeout(cp_session cps) {
    return cps ? cps->timeout : -1;
}

cp_res cloudplugs_set_cacert(cp_session cps, const char* filename) {
    if(!cps) return CP_FAIL;
    if(cps->ca) free(cps->ca);
    cps->ca = cloudplugs_concat(cps, 1, filename);
    return CP_OK;
}

const char* cloudplugs_get_last_err_string(cp_session cps) {
    switch(cps->err) {
        case CP_ERR_INTERNAL_ERROR:	return "Internal Library Error";
        case CP_ERR_OUT_OF_MEMORY: return "Out of memory";
        case CP_ERR_INVALID_SESSION: return "Invalid session";
        case CP_ERR_QUERY_IS_NOT_AN_OBJECT: return "Query is not an object";
        case CP_ERR_QUERY_INVALID_TYPE: return "Query contain invalid type";
        case CP_ERR_HEADERS_MUST_BE_STRING: return "Header value must be a string";
        case CP_ERR_INVALID_PARAMETER: return "Invalid parameter";
        case CP_ERR_INVALID_LOGIN: return "Invalid login";
        case CP_ERR_JSON_PARSE: return "JSON parse error";
        case CP_ERR_JSON_ENCODE: return "JSON encode error";
        case CP_ERR_HTTP: return "HTTP error";
        default: return NULL;
   }
}

CP_ERR_CODE cloudplugs_get_last_err_code(cp_session cps){
    return cps->err;
}

const char* cloudplugs_get_last_http_result_string(cp_session cps){
    switch(cps->http_res) {
      case CP_HTTP_OK:			return "Ok";
      case CP_HTTP_CREATED:		return "Created";
      case CP_HTTP_MULTI_STATUS:		return "Multi-Status";
      case CP_HTTP_BAD_REQUEST:		return "Bad Request";
      case CP_HTTP_UNAUTHORIZED:		return "Unauthorized";
      case CP_HTTP_PAYMENT_REQUIRED:	return "Payment Required";
      case CP_HTTP_FORBIDDEN:		return "Forbidden";
      case CP_HTTP_NOT_FOUND:		return "Not found";
      case CP_HTTP_NOT_ALLOWED:		return "Method Not Allowed";
      case CP_HTTP_NOT_ACCEPTABLE:	return "Not Acceptable";
      case CP_HTTP_SERVER_ERROR:		return "Internal Server Error";
      case CP_HTTP_NOT_IMPLEMENTED:	return "Not Implemented";
      case CP_HTTP_BAD_GATEWAY:		return "Bad Gateway";
      case CP_HTTP_SERVICE_UNAVAILABLE:	return "Service Unavailable";
      default: return NULL;
    }
}

CP_HTTP_RESULT cloudplugs_get_last_http_result(cp_session cps){
    return cps->http_res;
}

cp_session cloudplugs_create_session() {
  cp_session cps = malloc(sizeof(struct _cloudplugs_session));
  if(!cps) return NULL;
  cps->curl = curl_easy_init();
  if(!cps->curl) {
      free(cps);
      return NULL;
  }
  cps->timeout = CP_TIMEOUT;
  cps->id = NULL;
  cps->auth = NULL;
  cps->is_master = CP_FALSE;
  cps->base_url = (char*) malloc(LIT_STR_LEN(CP_URL)+1);
  strcpy(cps->base_url, CP_URL);
  cps->http_res = 0;
  cps->err = 0;
  cps->verify_ssl = CP_TRUE;
  cps->ca = NULL;
  return cps;
}

cp_res cloudplugs_ssl_verify(cp_session cps, cp_bool is_verified){
    if(!cps) return CP_FAIL;
    cps->verify_ssl = is_verified;
    return CP_OK;
}

cp_bool cloudplugs_has_ssl_verify(cp_session cps){
    if(!cps) return CP_FALSE;
    return cps->verify_ssl;
}

cp_res cloudplugs_enable_ssl(cp_session cps, cp_bool is_enabled){
    int https = strncmp(cps->base_url, CP_HTTP_STR, LIT_STR_LEN(CP_HTTP_STR));
    if((https && is_enabled) || (!https && !is_enabled)) return CP_OK;
    char* protocol = is_enabled ? CP_HTTPS_STR : CP_HTTP_STR;
    int start = (is_enabled ? LIT_STR_LEN(CP_HTTP_STR) : LIT_STR_LEN(CP_HTTPS_STR));
    char* tmp = cloudplugs_concat(cps, 2, protocol, cps->base_url+start);
    if(tmp){
        free(cps->base_url);
        cps->base_url = tmp;
        return CP_OK;
    }
    return CP_FAIL;
}

cp_bool cloudplugs_has_ssl(cp_session cps){
    if(!cps) return CP_FALSE;
    return strncmp(cps->base_url, CP_HTTP_STR, LIT_STR_LEN(CP_HTTP_STR));
}

cp_res cloudplugs_set_auth(cp_session cps, const char* id, const char* pass, cp_bool is_master) {
    if(!cps || !id || !pass) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    if(cps->id) free(cps->id);
    if(cps->auth) free(cps->auth);
    cps->id = strchr(id,'@') ? cloudplugs_concat(cps, 2, PLUG_EMAIL_HEADER, id) : cloudplugs_concat(cps, 2, PLUG_ID_HEADER, id);
    cps->auth = is_master ? cloudplugs_concat(cps, 2, PLUG_MASTER_HEADER, pass) : cloudplugs_concat(cps, 2, PLUG_AUTH_HEADER, pass);
    cps->is_master = is_master;
    return (cps->id && cps->auth) ? CP_TRUE : CP_FALSE;
}

cp_res cloudplugs_get_auth_id(cp_session cps, char* id, size_t size) {
   if(!cps || !cps->id) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
   char* index = strchr(cps->id,' ');
   strncpy(id, index+1, size);
   return id[size] == '\0' ? CP_TRUE : CP_FALSE;
}

cp_res cloudplugs_get_auth_pass(cp_session cps, char* pass, size_t size) {
   if(!cps || !cps->auth) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
   char* index = strchr(cps->auth,' ');
   strncpy(pass, index+1, size);
   return pass[size] == '\0' ? CP_TRUE : CP_FALSE;
}

cp_bool cloudplugs_is_auth_master(cp_session cps) {
    return (cps && cps->is_master) ? CP_TRUE : CP_FALSE;
}


cp_res cloudplugs_destroy_session(cp_session cps) {
    if(!cps) return CP_FAIL;
    curl_easy_cleanup(cps->curl);
    if(cps->id) free(cps->id);
    if(cps->auth) free(cps->auth);
    free(cps->base_url);
    if(cps->ca) free(cps->ca);
    free(cps);
    return CP_OK;
}

cp_res cloudplugs_uncontrol_device(cp_session cps, const char* plugid, const char* plugid_controlled, char** result, size_t* result_length) {
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, plugid_controlled, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_unenroll(cp_session cps, const char* plugid, char** result, size_t* result_length) {
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    if(!id) return CP_FAIL;
    char json_id[50] = "";
    snprintf(json_id, 50, "\"%s\"", id);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_DELETE, PATH_DEVICE, NULL, NULL, json_id, result, result_length);
    return cp_res;
}

cp_res cloudplugs_get_channel(cp_session cps, const char* channel_mask, const char* query, char** result, size_t* result_length) {
    if(!result) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    char* url = channel_mask ? cloudplugs_url_encode_channel(cps, channel_mask) : PATH_CHANNEL;
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_GET, url, NULL, query, NULL, result, result_length);
    if(url && channel_mask) free(url);
    return cp_res;
}

cp_res cloudplugs_publish_data(cp_session cps, const char *channel, const char *body, char** result, size_t* result_length) {
    char* url = channel ? cloudplugs_url_encode_data(cps, channel) : PATH_DATA;
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_PUT, url, NULL, NULL, body, result, result_length);
    if(url && channel) free(url);
    return cp_res;
}

cp_bool cloudplugs_extract_string_from_json(char** json, const char* key, char* res, const int n) {
    char*  start = strstr(*json, key);
    if(!start) return CP_FALSE;
    start = strchr(start,':');
    if(!start) return CP_FALSE;
    start = strchr(start,'\"');
    if(!start) return CP_FALSE;
    start++;
    char* end = strchr(start,'\"');
    if(!end) return CP_FALSE;

    int len = (int) (end-start);
    if(len>n) return CP_FALSE;
    strncpy(res, start, (size_t) len);
    return CP_TRUE;
}

cp_res cloudplugs_enroll_prototype(cp_session cps, const char* body, char** result, size_t* result_length) {
    if(!cps || !body) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    if(!cps->is_master) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_LOGIN);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_POST, PATH_DEVICE, NULL, NULL, body, result, result_length);
    return cp_res;
}

cp_res cloudplugs_enroll_product(cp_session cps, const char* body, char** result, size_t* result_length) {
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_FALSE, CP_HTTP_POST, PATH_DEVICE, NULL, NULL, body, result, result_length);
    cloudplugs_internal_set_auth(cps, cp_res, result);
    return cp_res;
}

cp_res cloudplugs_control_device(cp_session cps, const char* body, char** result, size_t* result_length) {
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_PUT, PATH_DEVICE, NULL, NULL, body, result, result_length);
    if(!cps->id) cloudplugs_internal_set_auth(cps, cp_res, result);
    return cp_res;
}

cp_res cloudplugs_enroll_ctrl(cp_session cps, const char* body, char** result, size_t* result_length) {
    if(cps->id && !strchr(cps->id,'@')) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_LOGIN);
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_FALSE, CP_HTTP_PUT, PATH_DEVICE, NULL, NULL, body, result, result_length);
    if(!cps->id) cloudplugs_internal_set_auth(cps, cp_res, result);
    return cp_res;
}

cp_res cloudplugs_retrieve_data(cp_session cps, const char* channel_mask, const char* query, char** result, size_t* result_length) {
    if(!channel_mask || !result) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    char* url = cloudplugs_url_encode_data(cps, channel_mask);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_GET, url, NULL, query, NULL, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_remove_data(cp_session cps, const char* channel_mask, const char* body, char** result, size_t* result_length) {
    if(!body || !channel_mask) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    char* url = cloudplugs_url_encode_data(cps, channel_mask);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, body, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_get_device(cp_session cps, const char* plugid, char** result, size_t* result_length) {
    if(!result) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_GET, url, NULL, NULL, NULL, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_set_device(cp_session cps, const char* plugid, const char* value, char** result, size_t* result_length) {
    if(!value) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_PATCH, url, NULL, NULL, value, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_get_device_prop(cp_session cps, const char* plugid, const char* prop, char** result, size_t* result_length) {
    if(!result) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = prop ? cloudplugs_url_encode_prop(cps, id, prop) : cloudplugs_concat(cps, 3, PATH_DEVICE "/", id, "/");
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_GET, url, NULL, NULL, NULL, result, result_length);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_set_device_prop(cp_session cps, const char* plugid, const char* prop, const char* value) {
    if(!value) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = prop ? cloudplugs_url_encode_prop(cps, id, prop) : cloudplugs_concat(cps, 3, PATH_DEVICE "/", id, "/");
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_PATCH, url, NULL, NULL, value, NULL, 0);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_remove_device_prop(cp_session cps, const char* plugid, const char* prop){
    if(!prop) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_url_encode_prop(cps, id, prop);
    cp_res cp_res = cloudplugs_request_exec(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, NULL, NULL, 0);
    if(url) free(url);
    return cp_res;
}

cp_res cloudplugs_set_device_location(cp_session cps, const char* plugid, double longitude, double latitude, double altitude, double accuracy, double timestamp) {
    if(!longitude || !latitude) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
  //longitude,  latitude,  altitude,  accuracy,  timestamp
    if(longitude > MAX_LONGITUDE || longitude < MIN_LONGITUDE) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    if(latitude > MAX_LATITUDE || latitude < MIN_LATITUDE) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    const int size = 200;
    const int s_size = 50;
    char body[size];
    char s_acc[s_size];
    char s_all[s_size];
    char s_tim[s_size];
    if(accuracy >= 0) snprintf(s_acc, s_size, ",\""ACCURACY"\":%g", accuracy); else s_acc[0]='\0';
    if(altitude >= 0) snprintf(s_all, s_size, ",\""ALTITUDE"\":%g", altitude); else s_all[0]='\0';
    if(timestamp >= 0) snprintf(s_tim, s_size, ",\""TIMESTAMP"\":%g", timestamp); else s_tim[0]='\0';

    snprintf(body, size, "{\""LONGITUDE"\":%g,\""LATITUDE"\":%g%s%s%s}", longitude, latitude, s_acc, s_all, s_tim);

    cp_res cp_res = cloudplugs_set_device_prop(cps, plugid, LOCATION, body);
    return cp_res;
}

cp_res cloudplugs_get_device_location(cp_session cps, const char* plugid, char** result, size_t* result_length) {
    return cloudplugs_get_device_prop(cps, plugid, LOCATION, result, result_length);
}
