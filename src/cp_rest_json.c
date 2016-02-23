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
#include "cp_rest_json.h"
#include "cp_internals.h"
#include "cp_constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

static char* get_http_query(cp_session cps, json_t* data) {
    CURL* curl = cps->curl;
    char* result = (char*) malloc(CP_MAX_URL_LENGTH* sizeof(char));
    if(!result) {
        cps->err = CP_ERR_OUT_OF_MEMORY;
        return NULL;
    }
    strcpy(result, "");
    if(!json_is_object(data)) {
        cps->err = CP_ERR_QUERY_IS_NOT_AN_OBJECT;
        return NULL;
    }
    const char* key;
    json_t* value;

    json_object_foreach(data, key, value) {
        char* ekey = curl_easy_escape(curl, key, strlen(key));
        strcat(result, ekey);
        free(ekey);
        strcat(result, "=");
        if(json_is_string(value)) {
            const char* s = json_string_value(value);
            char* es = curl_easy_escape(curl, s, strlen(s));
            strcat(result, es);
            free(es);
        } else if(json_is_number(value)) sprintf(result+strlen(result), "%g", json_number_value(value));
        else if(json_is_true(value)) strcat(result, CP_JSON_STRING_TRUE);
        else if(json_is_false(value)) strcat(result, CP_JSON_STRING_FALSE);
        //TODO else if(json_is_array(value)) {}
        else {
            cps->err = CP_ERR_QUERY_INVALID_TYPE;
            return NULL;
        }
        strcat(result, "&");
    }
    result[strlen(result)-1] = '\0';
    return result;
}

static void free_http_headers(char** h_array){
    if(h_array) {
        int i = 0;
        while(h_array[i]) {
            free(h_array[i]);
            i++;
        }
        free(h_array);
    }
}

static char** get_http_headers(cp_session cps, json_t* headers){
    char**h_array = NULL;
    const char* key;
    json_t* value;
    int i = 0;
    h_array = malloc(sizeof (char* )*  (json_object_size(headers) + 1));
    if(!h_array){
        cps->err = CP_ERR_OUT_OF_MEMORY;
        return NULL;
    }
    json_object_foreach(headers, key, value) {
        if(!json_is_string(value)) {
            cps->err = CP_ERR_HEADERS_MUST_BE_STRING;
            free_http_headers(h_array);
            return NULL;
        }
        const char* v = json_string_value(value);
        h_array[i] = cloudplugs_concat(cps, 3, key, ": ", v);
        if(!h_array[i]) {
            free_http_headers(h_array);
            return NULL;
        }
        i++;
    }
    return h_array;
}

/**
  This function execute a generic http request

  @param cps The connection reference used for the request
  @param http_method Enum that indicate the desired action to be performed on the identified resource
  @param path Relative path of the requested resource
  @param headers JSON object converted to "<key>: <value>"
  @param query JSON object converted to "?<key1>=<value1>&<key2>=<value2>"
  @param body JSON object sent as body
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
//CP_RES cloudplugs_request_json(cp_session cps, CP_HTTP_METHOD http_method, const char* path, json_t* headers, json_t* query, json_t* body, json_t** result);

static cp_res cloudplugs_request_json(cp_session cps, cp_bool auth, CP_HTTP_METHOD http_method, const char* path, json_t* headers, json_t* query, json_t* body, json_t** result)
{
    cp_res cp_res = CP_FAIL;
    if(!cps) return cp_res;

    char**h_array = headers ? get_http_headers(cps, headers) : NULL;
    if(headers && !h_array) return cp_res;

    char* squery = query ? get_http_query(cps, query) : NULL;
    if(query && !squery) {
        free_http_headers(h_array);
        return cp_res;
    }

    char* sbody = body ? json_dumps(body, JSON_ENCODE_ANY) : NULL;
    if(body && !sbody) {
        free_http_headers(h_array);
        if(squery) free(squery);
        cps->err = CP_ERR_JSON_ENCODE;
        return cp_res;
    }

    size_t len = 0;
    char* sres = NULL;
    cp_res = cloudplugs_request_exec(cps, auth, http_method, path, h_array, squery, sbody, result ? &sres : NULL, result ? &len : NULL);

    if(len && result) {
        json_error_t error;
        *result = json_loads(sres, JSON_DECODE_ANY, &error);
        if(!*result) {
            *result = json_object();
            json_object_set_new(*result, CP_JSON_ERR, json_string(error.text));
            json_object_set_new(*result, CP_JSON_BODY, json_string(sres));
            cps->err = CP_ERR_JSON_PARSE;
        }
        free(sres);
    } else if(result) {
        *result = NULL;
    }

    free_http_headers(h_array);
    if(squery) free(squery);
    if(sbody) free(sbody);
    return cp_res;
}

cp_res cloudplugs_publish_data_json(cp_session cps, const char* channel, json_t* body, json_t** result) {
    char* url = channel ? cloudplugs_url_encode_data(cps, channel) : PATH_DATA;
    if(!url) return CP_FAIL;

    cp_res res;
    json_t* obj;
    if(json_is_array(body)) {
        obj = body;
    } else if(json_is_object(body)) {
        if(!json_object_get(body, DATA)) {
            obj = json_object();
            if(!obj) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);
            json_object_set(obj, DATA, body);
        } else {
            obj = body;
        }
    } else {
        SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    }

    res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_PUT, url, NULL, NULL, obj, result);

    if(channel) free(url);
    return res;
}

cp_res cloudplugs_enroll_json(cp_session cps, json_t* obj, json_t** result) {
    if(!obj) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    CP_HTTP_METHOD http = json_object_get(obj, CTRL) ? CP_HTTP_PUT : CP_HTTP_POST;
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, http, PATH_DEVICE, NULL, NULL, obj, result);
    return res;
}

cp_res cloudplugs_enroll_product_json(cp_session cps, const char* model, const char* hwid, const char* pass, json_t* props, json_t** result) {
    if(!cps) return CP_FAIL;
    if(!model || !hwid || !pass) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);
    json_object_set_new(body, MODEL, json_string(model));
    json_object_set_new(body, HWID, json_string(hwid));
    json_object_set_new(body, PASS, json_string(pass));
    if(props) json_object_set(body, PROPS, props);
    cp_res res = cloudplugs_request_json(cps, CP_FALSE, CP_HTTP_POST, PATH_DEVICE, NULL, NULL, body, result);

    if(res == CP_OK)
        cloudplugs_set_auth(cps, json_string_value(json_object_get(*result, ID)), json_string_value(json_object_get(*result, AUTH)), CP_FALSE);

    json_decref(body);
    return res;
}


cp_res cloudplugs_enroll_prototype_json(cp_session cps, const char* name, const char* hwid, const char* pass, json_t* perm, json_t* props, json_t** result) {
    if(!cps) return CP_FAIL;
    if(!name) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    if(!cps->is_master) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_LOGIN);

    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);
    json_object_set_new(body, NAME, json_string(name));
    if(hwid) json_object_set_new(body, HWID, json_string(hwid));
    if(perm) json_object_set(body, PERM, perm);
    if(props) json_object_set(body, PROPS, props);
    if(pass) json_object_set_new(body, PASS, json_string(pass));
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_POST, PATH_DEVICE, NULL, NULL, body, result);

    json_decref(body);
    return res;
}

cp_res cloudplugs_enroll_ctrl_json(cp_session cps, const char* model, const char* ctrl, const char* pass, const char* hwid, const char* name, json_t** result) {
    if(!cps) return CP_FAIL;
    if(cps->id && !strchr(cps->id,'@')) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_LOGIN);
    if(!model || !ctrl || !pass) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);
    json_object_set_new(body, MODEL, json_string(model));
    json_object_set_new(body, CTRL, json_string(ctrl));
    json_object_set_new(body, PASS, json_string(pass));
    if(hwid) json_object_set_new(body, HWID, json_string(hwid));
    if(name) json_object_set_new(body, NAME, json_string(name));
    cp_res res = cloudplugs_request_json(cps, CP_FALSE, CP_HTTP_PUT, PATH_DEVICE, NULL, NULL, body, result);

    if(!cps->id && res == CP_OK)
        cloudplugs_set_auth(cps, json_string_value(json_object_get(*result, ID)), json_string_value(json_object_get(*result, AUTH)), CP_FALSE);

    json_decref(body);
    return res;
}

cp_res cloudplugs_control_device_json(cp_session cps, const char* model, const char* ctrl, const char* pass, json_t** result) {
    if(!cps) return CP_FAIL;
    if(!model || !ctrl || !pass) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);
    json_object_set_new(body, MODEL, json_string(model));
    json_object_set_new(body, CTRL, json_string(ctrl));
    json_object_set_new(body, PASS, json_string(pass));
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_PUT, PATH_DEVICE, NULL, NULL, body, result);

    if(!cps->id && res == CP_OK)
        cloudplugs_set_auth(cps, json_string_value(json_object_get(*result, ID)), json_string_value(json_object_get(*result, AUTH)), CP_FALSE);

    json_decref(body);
    return res;
}

cp_res cloudplugs_retrieve_data_json(cp_session cps, const char* channel_mask, cp_time before, cp_time after, cp_time at, const char* of, int offset, int limit, json_t** result) {
    if(!channel_mask) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    char* url = cloudplugs_url_encode_data(cps, channel_mask);
    if(!url) return CP_FAIL;

    json_t* query = json_object();
    if(!query) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);

    if(before) json_object_set_new(query, BEFORE, json_real(before));
    if(after) json_object_set_new(query, AFTER, json_real(after));
    if(at) json_object_set_new(query, AT, json_real(at));
    if(of) json_object_set_new(query, OF, json_string(of));
    if(offset) json_object_set_new(query, OFFSET, json_integer(offset));
    if(limit) json_object_set_new(query, LIMIT, json_integer(limit));

    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_GET, url, NULL, query, NULL, result);
    json_decref(query);
    free(url);
    return res;
}

cp_res cloudplugs_remove_data_json(cp_session cps, const char* channel_mask, json_t *id, json_t* before, json_t* after, json_t* at, json_t* of, json_t** result) {
    if(!channel_mask || (!id && !before && !after && !at)) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    char* url = cloudplugs_url_encode_data(cps, channel_mask);

    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);

    if(id) json_object_set(body, ID, id);
    if(before) json_object_set(body, BEFORE, before);
    if(after) json_object_set(body, AFTER, after);
    if(at) json_object_set(body, AT, at);
    if(of) json_object_set(body, OF, of);

    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, body, result);
    json_decref(body);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_uncontrol_device_json(cp_session cps, const char* plugid, json_t* plugid_controlled, json_t** result) {
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);

    if(!json_is_string(plugid_controlled) || !json_is_array(plugid_controlled)) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, plugid_controlled, result);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_unenroll_json(cp_session cps, json_t* plugid, json_t** result) {
    json_t* id = plugid ? plugid : json_string(cloudplugs_get_plug_id(cps));
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_DELETE, PATH_DEVICE, NULL, NULL, id, result);
    if(!plugid) json_decref(id);
    return res;
}

cp_res cloudplugs_get_channel_json(cp_session cps, const char* channel_mask, json_t* before, json_t* after, json_t* at, json_t* of, int offset, int limit, json_t** result) {
    if(!channel_mask) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    char* url = channel_mask ? cloudplugs_url_encode_channel(cps, channel_mask) : PATH_CHANNEL;

    json_t* query = json_object();
    if(!query) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);

    if(before) json_object_set(query, BEFORE, before);
    if(after) json_object_set(query, AFTER, after);
    if(at) json_object_set(query, AT, at);
    if(of) json_object_set(query, OF, of);
    if(offset>0) json_object_set_new(query, OFFSET, json_integer(offset));
    if(limit>0) json_object_set_new(query, LIMIT, json_integer(limit));

    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_GET, url, NULL, query, NULL, result);
    json_decref(query);
    if(url && channel_mask) free(url);
    return res;
}

cp_res cloudplugs_get_device_json(cp_session cps, const char* plugid, json_t** result) {
    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_GET, url, NULL, NULL, NULL, result);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_set_device_json(cp_session cps, const char* plugid, json_t* value, json_t** result) {
    if(!value) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 2, PATH_DEVICE "/", id);
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_PATCH, url, NULL, NULL, value, result);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_get_device_prop_json(cp_session cps, const char* plugid, const char* prop, json_t** result) {
    const char *id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = prop ? cloudplugs_concat(cps, 4, PATH_DEVICE "/", id, "/", prop) : cloudplugs_concat(cps, 3, PATH_DEVICE "/", id, "/");
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_GET, url, NULL, NULL, NULL, result);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_set_device_prop_json(cp_session cps, const char* plugid, const char* prop, json_t* value) {
    if(!value) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = prop ? cloudplugs_concat(cps, 4, PATH_DEVICE "/", id, "/", prop) : cloudplugs_concat(cps, 3, PATH_DEVICE "/", id, "/");
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_PATCH, url, NULL, NULL, value, NULL);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_remove_device_prop_json(cp_session cps, const char* plugid, const char* prop) {
    if(!prop) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    const char* id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 4, PATH_DEVICE "/", id, "/", prop);
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_DELETE, url, NULL, NULL, NULL, NULL);
    if(url) free(url);
    return res;
}

cp_res cloudplugs_set_device_location_json(cp_session cps, const char* plugid, double longitude, double latitude, double altitude, double accuracy, cp_time timestamp) {
  //longitude,  latitude,  altitude,  accuracy,  timestamp
    if(longitude > MAX_LONGITUDE || longitude < MIN_LONGITUDE) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);
    if(latitude > MAX_LATITUDE || latitude < MIN_LATITUDE) SET_ERROR_AND_RETURN(cps, CP_ERR_INVALID_PARAMETER);

    json_t* body = json_object();
    if(!body) SET_ERROR_AND_RETURN(cps, CP_ERR_OUT_OF_MEMORY);

    json_object_set_new(body, LONGITUDE, json_real(longitude));
    json_object_set_new(body, LATITUDE, json_real(latitude));
    if(accuracy >= 0) json_object_set_new(body, ACCURACY, json_real(accuracy));
    if(altitude >= 0) json_object_set_new(body, ALTITUDE, json_real(altitude));
    if(timestamp >= 0) json_object_set_new(body, TIMESTAMP, json_real(timestamp));

    cp_res res = cloudplugs_set_device_prop_json(cps, plugid, LOCATION, body);
    json_decref(body);
    return res;

}

cp_res cloudplugs_get_device_location_json(cp_session cps, const char* plugid, json_t** result) {
    const char *id = plugid ? plugid : cloudplugs_get_plug_id(cps);
    char* url = cloudplugs_concat(cps, 4, PATH_DEVICE "/", id, "/", LOCATION);
    cp_res res = cloudplugs_request_json(cps, CP_TRUE, CP_HTTP_GET, url, NULL, NULL, NULL, result);
    if(url) free(url);
    return res;
}
