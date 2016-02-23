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
#include <stdlib.h>
#include <string.h>

char* cloudplugs_concat(cp_session cps, int num, ... ) {
    if(!cps) return NULL;
    va_list arguments;
    int sum = 0;
    va_start ( arguments, num );
    int x;
    for(x = 0; x < num; x++) {
        char* k = va_arg(arguments, char*);
        if(!k) {
            va_end(arguments);
            return NULL;
        }
        sum += strlen(k);
    }
    va_end(arguments);

    char* s = (char*) malloc((sum+1)* sizeof(char));
    if(!s) {
        if(cps) cps->err = CP_ERR_OUT_OF_MEMORY;
        return s;
    }
    va_start(arguments, num);
    strcpy(s, va_arg(arguments, char*));

    for(x = 1; x < num; x++) {
        strcat(s, va_arg(arguments, char*));
    }
    va_end(arguments);

    return s;
}

static const char* CP_HTTP_METHODS[] = { "GET", "POST", "PUT", "DELETE", "PATCH" };

struct _cp_req_buffer {
  char* body;
  size_t len;
  size_t offset;
  cp_session cps;
};

typedef struct _cp_req_buffer cp_req_buffer;

static size_t writefunc(void* ptr, size_t size, size_t nmemb, cp_req_buffer* b) {
    if(!b->body) {
        double sz;
        b->len = 0;
        CURLcode res = curl_easy_getinfo(b->cps->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &sz);
        if(res == CURLE_OK && sz != -1) {
            b->offset = 0;
            b->len = (size_t) sz;
            if(sz > 0){
                b->body = (char*) malloc((b->len+1)*sizeof(char));
                b->body[b->len] = '\0';
            }
        } else {
            b->cps->err = CP_ERR_INVALID_CONTENT_LENGTH;
            return 0;//If that amount differs from the amount passed to your function, it'll signal an error to the library
        }
    }

    size_t tot = size * nmemb;
    size_t off = b->offset + tot;
    if(off > b->len) {
        b->cps->err = CP_ERR_INVALID_CONTENT_LENGTH;
        b->len = 0;
    } else {
        memcpy(b->body+b->offset, ptr, tot);
        b->offset = off;
    }
    return tot;//Return the number of bytes actually taken care of.
}

cp_res cloudplugs_request_exec(cp_session cps, cp_bool auth, CP_HTTP_METHOD http_method, const char* path, char* headers[], const char* query, const char* body, char** result, size_t* result_length) {
    if(!cps) return CP_FAIL;
    if(!path) return CP_FAIL;

    if(auth && !cps->auth) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_LOGIN);

    CURLcode curl_res;
    struct curl_slist* chunk = NULL;
    CURL* curl = NULL;
    cps->http_res = 0;
    cps->err = 0;
    curl = cps->curl;

    //already default: curl_easy_setopt(cps->curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(cps->curl, CURLOPT_CONNECTTIMEOUT, cps->timeout);
    if(!cps->verify_ssl)
        curl_easy_setopt(cps->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    if(cps->ca)
        curl_easy_setopt(cps->curl, CURLOPT_CAINFO, cps->ca);

    char* full_url = query ? cloudplugs_concat(cps, 4, cps->base_url , path, "?", query) : cloudplugs_concat(cps, 2, cps->base_url, path);
    if(!full_url) {
        curl_easy_reset(curl);
        if(chunk) curl_slist_free_all(chunk);
        return CP_FAIL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, full_url);
    free(full_url);

    /* is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    cp_req_buffer b;
    if(result) {
        b.body = NULL;
        b.cps = cps;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &b);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    }
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, CP_HTTP_METHODS[http_method]);

    if(headers) {
        int i = 0 ;
        while(headers[i] != 0) {
            chunk = curl_slist_append(chunk, headers[i]);
            i++;
        }
    }
    chunk = curl_slist_append(chunk, CONTENT_TYPE_JSON);

    if(cps->id && cps->auth) {
        chunk = curl_slist_append(chunk, cps->id);
        chunk = curl_slist_append(chunk, cps->auth);
    }

    if(chunk) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

    if(body) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    /* Perform the request, res will get the return code */
    curl_res = curl_easy_perform(curl);
    /* Check for errors */
    if(curl_res != CURLE_OK) {
        const char*error = curl_easy_strerror(curl_res);
        int resl = strlen(error);
        if(result_length) *result_length = resl;
        if(result) {
            *result = (char*) malloc(resl * sizeof(char));
            if(*result) strcpy(*result, error);
        }
    } else {
        if(result) *result = b.body;
        if(result_length) {
            double sz;
            CURLcode res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &sz);
            if(res == CURLE_OK) *result_length = (size_t) sz;
        }
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &cps->http_res);

    /* always cleanup */

    curl_easy_reset(curl);
    if(chunk) curl_slist_free_all(chunk);
    if(cps->http_res == CP_HTTP_OK || cps->http_res == CP_HTTP_CREATED) {
        return CP_OK;
    } else {
        cps->err = CP_ERR_HTTP;
        return CP_FAIL;
    }
}

const char* cloudplugs_get_plug_id(cp_session cps) {
    if(!cps) return NULL;
    if(!cps->id || strchr(cps->id,'@')) {
        cps->err = CP_ERR_INVALID_LOGIN;
        return NULL;
    }
    return cps->id+LIT_STR_LEN(PLUG_ID_HEADER);
}

void cloudplugs_internal_set_auth(cp_session cps, cp_res cp_res, char** result){
    if(cp_res == CP_OK) {
        char plugid[32] = "";
        char auth[128] = "";
        if(cloudplugs_extract_string_from_json(result, ID, plugid, 32) && cloudplugs_extract_string_from_json(result, AUTH, auth, 128)){
            cloudplugs_set_auth(cps, plugid, auth, CP_FALSE);
        }
    }
}

char* cloudplugs_url_encode_channel(cp_session cps, const char* channel_mask){
    char* ecm = curl_easy_escape(cps->curl, channel_mask, strlen(channel_mask));
    if(!ecm) {
        cps->err = CP_ERR_OUT_OF_MEMORY;
        return NULL;
    }
    char* res = cloudplugs_concat(cps, 2, PATH_CHANNEL "/", ecm);
    curl_free(ecm);
    return res;
}

char* cloudplugs_url_encode_data(cp_session cps, const char* channel_mask){
    char* ecm = curl_easy_escape(cps->curl, channel_mask, strlen(channel_mask));
    if(!ecm) {
        cps->err = CP_ERR_OUT_OF_MEMORY;
        return NULL;
    }
    char* res = cloudplugs_concat(cps, 2, PATH_DATA "/", ecm);
    curl_free(ecm);
    return res;
}

char* cloudplugs_url_encode_prop(cp_session cps, const char* id, const char* prop){
    char* ep = curl_easy_escape(cps->curl, prop, strlen(prop));
    if(!ep) {
        cps->err = CP_ERR_OUT_OF_MEMORY;
        return NULL;
    }
    char* res = cloudplugs_concat(cps, 4, PATH_DEVICE "/", id, "/", ep);
    curl_free(ep);
    return res;
}
