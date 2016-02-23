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
#ifndef CP_REST_H
#define CP_REST_H

#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif


typedef struct _cloudplugs_session* cp_session; /**<Reference to a session */

#define CP_OK 0
#define CP_FAIL 1
typedef int cp_res; /**<An integer representing the result of a request */

typedef int cp_bool; /**<Library internal booleans */
#define CP_TRUE 1
#define CP_FALSE 0



enum _CP_HTTP_RESULT {   
                CP_HTTP_OK = 200,
                CP_HTTP_CREATED = 201,
                CP_HTTP_MULTI_STATUS  =  207,
                CP_HTTP_BAD_REQUEST = 400,
                CP_HTTP_UNAUTHORIZED = 401,
                CP_HTTP_PAYMENT_REQUIRED = 402,
                CP_HTTP_FORBIDDEN = 403,
                CP_HTTP_NOT_FOUND = 404,
                CP_HTTP_NOT_ALLOWED = 405,
                CP_HTTP_NOT_ACCEPTABLE = 406,
                CP_HTTP_SERVER_ERROR = 500,
                CP_HTTP_NOT_IMPLEMENTED = 501,
                CP_HTTP_BAD_GATEWAY = 502,
                CP_HTTP_SERVICE_UNAVAILABLE = 503 };
typedef enum _CP_HTTP_RESULT CP_HTTP_RESULT;   /**<HTTP results returned by the server */



enum _CP_ERR_CODE { CP_ERR_INTERNAL_ERROR = -1,
                    CP_ERR_OUT_OF_MEMORY = -2,
                    CP_ERR_INVALID_SESSION = -3,
                    CP_ERR_QUERY_IS_NOT_AN_OBJECT = -4,
                    CP_ERR_QUERY_INVALID_TYPE = -5,
                    CP_ERR_HEADERS_MUST_BE_STRING = -6,
                    CP_ERR_INVALID_PARAMETER = -7,
                    CP_ERR_INVALID_LOGIN = -8,
                    CP_ERR_JSON_PARSE = -9,
                    CP_ERR_JSON_ENCODE = -10,
                    CP_ERR_INVALID_CONTENT_LENGTH = -11,
                    CP_ERR_HTTP = -12
                  };


typedef enum _CP_ERR_CODE CP_ERR_CODE; /**<Library internal error codes */

/**
 Must be called at least once within a program (a program is all the code that shares a memory space) before the program calls any other function of CloudPlugs library. The environment it sets up is constant for the life of the program and is the same for every program, so multiple calls have the same effect as one call.
 This function is not thread safe. You must not call it when any other thread in the program (i.e. a thread sharing the same memory) is running. This doesn't just mean no other thread that is using CloudPlugs library.

 @return CP_OK on success, CP_FAIL otherwise.
*/
cp_res cloudplugs_global_init();

/**
 Must be called once for each call made to cloudplugs_global_init(), after you are done using CloudPlugs library.
 This function is not thread safe. You must not call it when any other thread in the program (i.e. a thread sharing the same memory) is running. This doesn't just mean no other thread that is using CloudPlugs library.

 @return CP_OK on success, CP_FAIL otherwise.
*/
cp_res cloudplugs_global_shutdown();

/**
 Change the default base url.

 @param cps The session reference.
 @param url Http or https url string.
 @return CP_OK if the new base url is set correctly, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_base_url(cp_session cps, const char* url);

/**
 Set the certification authority for verify ssl connections.

 @param cps The session reference.
 @param filename String naming a file holding one or more certificates to verify the peer with.
 @return CP_OK if the certificate authority is set correctly, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_cacert(cp_session cps, const char* filename);

/**
 Get the current base url.

 @param cps The session reference.
 @return The base url.
*/
const char* cloudplugs_get_base_url(cp_session cps);

/**
 Change the default timeout.

 @param cps The session reference.
 @param timeout Timeout in seconds.
 @return CP_OK if the new timeout is set correctly, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_timeout(cp_session cps, int timeout);

/**
 Get the current timeout.

 @param cps The session reference.
 @return The current timeout in seconds.
*/
int cloudplugs_get_timeout(cp_session cps);

/**
 Return a human-readable string that describes the last error.

 @param cps The session reference.
 @return A human-readable string that describes the last error.
*/
const char* cloudplugs_get_last_err_string(cp_session cps);

/**
 Return a human-readable string that describes the last error.

 @param cps The session reference.
 @return A human-readable string that describes the last error.
*/
CP_ERR_CODE cloudplugs_get_last_err_code(cp_session cps);

/**
 Return a human-readable string that describes the last error.

 @param cps The session reference.
 @return A human-readable string that describes the last error.
*/
const char* cloudplugs_get_last_http_result_string(cp_session cps);

/**
 Return a human-readable string that describes the last error.

 @param cps The session reference.
 @return A human-readable string that describes the last error.
*/
CP_HTTP_RESULT cloudplugs_get_last_http_result(cp_session cps);


/**
 Initializes a HTTP persistent connection.

 @result The session reference, NULL if occurred an error.
*/
cp_session cloudplugs_create_session();

/**
 Set the session authentication credentials.

 @param cps The session reference.
 @param id A string containing the @ref details_PLUG_ID or the master email.
 @param pass A string containing the authentication code.
 @param is_master CP_TRUE for using master authentication in the session; CP_FALSE for using regular authentication.
 @return CP_OK if the authentication is set correctly, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_auth(cp_session cps, const char* id, const char* pass, cp_bool is_master);

/**
 Enable or disable ssl verification.

 @param cps The session reference.
 @param is_verified Boolean state of ssl verification.
 @return CP_OK if success, CP_FAIL otherwise.
*/
cp_res cloudplugs_ssl_verify(cp_session cps, cp_bool is_verified);

/**
 State of ssl verification.

 @param cps The session reference.
 @return CP_TRUE if ssl is enable, CP_FALSED otherwise.
*/
cp_bool cloudplugs_has_ssl_verify(cp_session cps);

/**
 Enable or disable ssl protocol.

 @param cps The session reference.
 @param is_enabled Boolean state of ssl.
 @return CP_OK if success, CP_FAIL otherwise.
*/
cp_res cloudplugs_enable_ssl(cp_session cps, cp_bool is_enabled);

/**
 State of ssl protocol.

 @param cps The session reference.
 @return CP_TRUE if ssl is enable, CP_FALSED otherwise.
*/
cp_bool cloudplugs_has_ssl(cp_session cps);

/**
 Get the authentication id (@ref details_PLUG_ID or email) of the session.

 @param cps The session reference.
 @param id The destination buffer where the authentication id will be written.
 @param size Size of the destination buffer.
 @return CP_OK if the buffer was correctly written, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_auth_id(cp_session cps, char* id, size_t size);

/**
 Get the authentication password of the session.

 @param cps The session reference.
 @param password The destination buffer where the authentication password will be written.
 @param size Size of the destination buffer.
 @return CP_OK if the buffer was correctly written, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_auth_pass(cp_session cps, char* password, size_t size);

/**
 Return the authentication mode in the session.

 @param cps The session reference.
 @return CP_TRUE if using master authentication in the session; CP_FALSE if using regular authentication.
*/
cp_bool cloudplugs_is_auth_master(cp_session cps);

/**
 Closes and cleans a session.

 @param cps The session reference.
 @return CP_OK if the session is correctly closed, CP_FAIL otherwise.
*/
cp_res cloudplugs_destroy_session(cp_session cps);

/**
 Extract a string value from a json object

 @param json The JSON string.
 @param key The key.
 @param res The destination buffer where the string value will be written.
 @param n Size of the destination buffer.
 @return CP_TRUE if succeeds, CP_FALSE otherwise.
*/

cp_bool cloudplugs_extract_string_from_json(char **json, const char* key, char *res, const int n);

/**
 This function performs an HTTP request to the server  for enrolling a new production device and place the response in *result and *result_length.

 @param cps The session reference without authentication.
 @param body A buffer containing a json object like this:\n
 {\n
        \b "model" : @ref details_PLUG_ID,	// the model of this device\n
        \b "hwid"  : @ref details_HWID,		// the serial number\n
        \b "pass"  : String,		\n
        \b "props" : JSON		// optional, to initialize the custom properties\n
    }
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_enroll_product(cp_session cps, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for enrolling a prototype and place the response in *result and *result_length.

 @param cps The session reference without authentication.
 @param body A buffer containing a json object like this:\n
 {\n
        \b "hwid"  : @ref details_HWID,		// optional, if absent it will be set as a random unique string\n
        \b "pass"  : String,		// optional, if absent set as the X-Plug-Master of the company\n
        \b "name"  : String,\n
        \b "perm"  : @ref details_PERM_FILTER,	// optional, if absent permit all\n
        \b "props" : JSON		// optional, to initialize the custom properties\n
    }
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_enroll_prototype(cp_session cps, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for enrolling a new or already existent controller device and place the response in *result and *result_length.

 @param cps The session reference without authentication if it's the first enroll.
 @param body A buffer containing a json object like this: \n
 {\n
        \b "model" : @ref details_PLUG_ID,	// model id of the device to control\n
        \b "ctrl"  : @ref details_HWID,	// serial number (hwid) of the device to control\n
        \b "pass"  : String,\n
        \b "hwid"  : @ref details_HWID,	// unique string to identify this controller device\n
        \b "name"  : String	//  the name of this device\n
    }
 @param result If not NULL, then a pointer such that  *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_enroll_ctrl(cp_session cps, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for controlling a device and place the response in *result and *result_length.

 @param cps The session reference.
 @param body A buffer containing a json object like this:\n
 {\n
        \b "model" : @ref details_PLUG_ID,	// model id of the device to control\n
        \b "ctrl"  : @ref details_HWID,	// serial number (hwid) of the device to control\n
        \b "pass"  : String,\n
    }
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_control_device(cp_session cps, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for uncontrol a device and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
 @param plugid_controlled If not NULL, then the device(s) to uncontroll (default all associated devices).
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_uncontrol_device(cp_session cps, const char* plugid, const char* plugid_controlled, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for reading a device and place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_device(cp_session cps, const char* plugid, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for reading the device properties  and place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param prop If NULL, then all properties value; otherwise the single property value.
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_device_prop(cp_session cps, const char* plugid, const char* prop, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for writing or deleting device properties and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param prop If NULL, then value must be an object; otherwise the single property value is written.
 @param value A json value, use null to delete one or all device properties.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_device_prop(cp_session cps, const char* plugid, const char* prop, const char* value);

/**
 This function performs an HTTP request to the server for deleting device property and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param prop The single property to be remove.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_remove_device_prop(cp_session cps, const char* plugid, const char* prop);

/**
 This function performs an HTTP request to the server for modifying device and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param value A json object like this:\n
 {\n
        \b "perm"   : @ref details_PERM_FILTER,	// optional, it contains just the sharing filters to modify\n
        \b "name"   : String,		// optional\n
        \b "status" : @ref details_STATUS,		// optional\n
        \b "props"  : Object		// optional, it contains just the properties to modify\n
    }
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_device(cp_session cps, const char* plugid, const char* value, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for removing any device (development, product or controller) and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID_CSV of the device(s) to remove; if NULL then remove the device referenced in the session.
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_unenroll(cp_session cps, const char* plugid, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for retrieving list channels/channels about already published data and place the response in *result and *result_length.

 @param cps The session reference.
 @param channel_mask @ref details_CHMASK The channel mask.
 @param query If not NULL, must be a url-encode string containing those values:\n
 before: Optional, @ref details_TIMESTAMP or @ref details_OBJECT_ID timestamp valid if greater than zero\n
 after: Optional, @ref details_TIMESTAMP or @ref details_OBJECT_ID timestamp valid if greater than zero\n
 at: Optional, @ref details_TIMESTAMP_CSV timestamp valid if greater than zero\n
 of: Optional, @ref details_PLUG_ID_CSV \n
 offset: Optional Number: positive integer (including 0) \n
 limit: Optional Number: positive integer (including 0)\n
 @param result A pointer such that *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_channel(cp_session cps, const char* channel_mask, const char* query, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for retrieving already published data and place the response in *result and *result_length.

 @param cps The session reference.
 @param channel_mask @ref details_CHMASK The channel mask.
 @param query If not NULL, must be a url-encode string containing those values:\n
 before: Optional, @ref details_TIMESTAMP or @ref details_OBJECT_ID timestamp valid if greater than zero\n
 after: Optional, @ref details_TIMESTAMP or @ref details_OBJECT_ID timestamp valid if greater than zero\n
 at: Optional, @ref details_TIMESTAMP_CSV timestamp valid if greater than zero\n
 of: Optional,@ref details_PLUG_ID_CSV \n
 offset: Optional Number: positive integer (including 0)  \n
 limit: Optional Number: positive integer (including 0)  \n
 @param result A pointer such that *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_retrieve_data(cp_session cps, const char* channel_mask, const char* query, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for publishing data and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param channel An optional @ref details_CHANNEL , if NULL data need to contain a couple "channel":"channel"
 @param body A json object or an array of objects like this:\n
{\n
    \b "id"        : “PLUG_ID”,\n
    \b "channel"     : @ref details_CHANNEL,	// optional, to override the channel in the url\n
    \b "data"      : JSON,\n
    \b "at"        : @ref details_TIMESTAMP,\n
    \b "of"        : @ref details_PLUG_ID,	// optional, check if the X-Plug-Id is authorized for setting this field\n
    \b "is_priv"   : Boolean,	// optional, default false\n
    \b "expire_at" : @ref details_TIMESTAMP,	// optional, expire date of this data entry\n
    \b "ttl"       : Number		// optional, how many *seconds* this data entry will live (if "expire_at" is present, then this field is ignored)\n
}
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_publish_data(cp_session cps, const char* channel, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for deleting already published data and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param channel_mask The @ref details_CHMASK
 @param body A buffer containing a json object like this (at least one param is required: id, before, after or at):\n
{\n
    \b "id"     : @ref details_OBJECT_ID_CSV ,\n
    \b "before" : @ref details_TIMESTAMP or @ref details_PLUG_ID,\n
    \b "after"  : @ref details_TIMESTAMP or @ref details_PLUG_ID,\n
    \b "at"     : @ref details_TIMESTAMP_CSV or ,\n
    \b "of"     : @ref details_PLUG_ID_CSV or \n
}
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_remove_data(cp_session cps, const char* channel_mask, const char* body, char** result, size_t* result_length);

/**
 This function performs an HTTP request to the server for writing or deleting device location and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid If not NULL, then the @ref details_PLUG_ID of the device, otherwise the device referenced in the session.
 @param longitude
 @param latitude
 @param altitude //optional
 @param accuracy //optional
 @param timestamp //optional
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_set_device_location(cp_session cps, const char* plugid, double longitude, double latitude, double altitude, double accuracy, double timestamp);
/**
 This function performs an HTTP request to the server for writing or deleting device location and place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid If not NULL, then the @ref details_PLUG_ID of the device, otherwise the device referenced in the session.
 @param result If not NULL, then *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @param result_length The length of the string stored in *result.
 @return CP_OK if the request succeeds, CP_FAIL otherwise.
*/
cp_res cloudplugs_get_device_location(cp_session cps, const char* plugid, char** result, size_t* result_length);

#ifdef  __cplusplus
}
#endif

#endif //CP_REST_H
