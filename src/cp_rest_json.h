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

#ifndef CP_REST_JSON_H
#define CP_REST_JSON_H

#include <jansson.h>
#include "cp_rest.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 Number of milliseconds in GMT since the time of Epoch
*/
typedef double cp_time;

/**
 This function performs an HTTP request to the server  for enrolling a new production device or create new (development) device or enroll new or already existent controller device, that depends on the content of the json. Optionally places the response in *result.

  @param cps The session reference such that: if it's an prototype enroll, master authentication is required; if it's a product or controller enroll, the session cannot have authentication.
  @param obj Json object that contain the info for enroll new production device or create new (development) device (X-Plug-Master header required!) or enroll new or already existent controller device.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_enroll_json(cp_session cps, json_t* obj, json_t** result);

/**
 This function performs an HTTP request to the server  for enrolling a new production device. It places place the response in *result.

  @param cps The session reference without authentication.
  @param model @ref details_PLUG_ID The device model's .
  @param hwid @ref details_HWID The serial number.
  @param pass string The password.
  @param props If not NULL, then initialize the custom properties.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_enroll_product_json(cp_session cps, const char* model, const char* hwid, const char* pass, json_t* props, json_t** result);

/**
 This function performs an HTTP request to the server  for enrolling a prototype. It places place the response in *result.

  @param cps The session reference without authentication.
  @param name string The name of the product.
  @param hwid @ref details_HWID If NULL, then it will be set as a random unique string.
  @param pass string If NULL, then set as the X-Plug-Master of the company
  @param perm @ref details_PERM_FILTER If NULL, then permit all.
  @param props If not NULL, then initialize the custom properties.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result., { "id":"PLUG_ID", "auth":String }
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_enroll_prototype_json(cp_session cps, const char* name, const char* hwid, const char* pass, json_t* perm, json_t* props, json_t** result);

/**
 This function performs an HTTP request to the server  for enrolling a new or already existent controller device. It places place the response in *result.

  @param cps The session reference without authentication if it's the first enroll.
  @param model The model @ref details_PLUG_ID of the device to control.
  @param ctrl The serial number ( @ref details_HWID ) of the device to control.
  @param pass The device password's
  @param hwid @ref details_HWID unique string to identify this controller device
  @param name string the name of this device
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_enroll_ctrl_json(cp_session cps, const char* model, const char* ctrl, const char* pass, const char* hwid, const char* name, json_t** result);

/**
 This function performs an HTTP request to the server  for enrolling an already existent controller device. It places place the response in *result.

  @param cps The session reference.
  @param model The model @ref details_PLUG_ID of the device to control.
  @param ctrl The serial number ( @ref details_HWID ) of the device to control.
  @param pass The device password's
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_control_device_json(cp_session cps, const char* model, const char* ctrl, const char* pass, json_t** result);

/**
 This function performs an HTTP request to the server for uncontrolling a device and [optionally] place the response in *result.

  @param cps The session reference.
  @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
  @param plugid_controlled If not NULL, then a json string or a json array of strings of @ref details_PLUG_ID device(s) to uncontroll;  (default all associated devices).
  @param result The response returned as a reference
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_uncontrol_device_json(cp_session cps, const char* plugid, json_t* plugid_controlled, json_t** result);

/**
 This function performs an HTTP request to the server for retrieving list channels/channels about already published data. It places the response information in *result and *result_length.

 @param cps The session reference.
 @param channel_mask The @ref details_CHMASK.
 @param before : @ref details_TIMESTAMP or @ref details_OBJECT_ID  Optional, timestamp valid if greater than zero or OID of published data
 @param after  : @ref details_TIMESTAMP or @ref details_OBJECT_ID  Optional, timestamp valid if greater than zero or OID of published data
 @param at     : @ref details_TIMESTAMP_CSV Optional, timestamp valid if greater than zero //TODO TIMESTAMP_CSV&?
 @param of     : @ref details_PLUG_ID_CSV Optional, @ref details_PLUG_ID_CSV
 @param offset : N
 @param limit  : N
 @param result A pointer such that *result will contain the dynamically allocated json string of the retrieved response body. The caller is responsible to free memory in *result.
 @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_get_channel_json(cp_session cps, const char* channel_mask, json_t* before, json_t* after, json_t* at, json_t* of, int offset, int limit, json_t** result);

/**
 This function performs an HTTP request to the server for reading a device. It places the response in *result.

  @param cps The session reference.
  @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_get_device_json(cp_session cps, const char* plugid, json_t** result);

/**
 This function performs an HTTP request to the server for reading the device properties  and place the response in *result.

  @param cps The session reference.
  @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
  @param prop If NULL, then all properties value; otherwise the single property value.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_get_device_prop_json(cp_session cps, const char* plugid, const char* prop, json_t** result);

/**
 This function performs an HTTP request to the server for writing or deleting device properties and [optionally] it places the response in *result.

  @param cps The session reference.
  @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
  @param prop If NULL, then value must be an object; otherwise the single property value is written.
  @param value A json value, use null to delete one or all device properties.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_set_device_prop_json(cp_session cps, const char* plugid, const char* prop, json_t* value);

/**
 This function performs an HTTP request to the server for deleting device property and [optionally] place the response in *result and *result_length.

 @param cps The session reference.
 @param plugid The @ref details_PLUG_ID of the device.
 @param prop The single property to be remove.
 @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_remove_device_prop_json(cp_session cps, const char* plugid, const char* prop);

/**
 This function performs an HTTP request to the server for modifying a device and [optionally] it places the response in *result

  @param cps The session reference.
  @param plugid If NULL, then is the @ref details_PLUG_ID in the session.
  @param value A json object like this:
  {\n
        \b "perm"   : @ref details_PERM_FILTER ,	// optional, it contains just the sharing filters to modify\n
        \b "name"   : String,		// optional\n
        \b "status" : @ref details_STATUS ,		// optional\n
        \b "props"  : Object		// optional, it contains just the properties to modify\n
    }
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_set_device_json(cp_session cps, const char* plugid, json_t* value, json_t** result);

/**
 This function performs an HTTP request to the server for removing a device (any: development, product or controller) and [optionally] place the response in *result.

  @param cps The session reference.
  @param plugid The @ref details_PLUG_ID_CSV of the device(s) to remove; if NULL then remove the device referenced in the session.
  @param result The response returned as a reference
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_unenroll_json(cp_session cps, json_t* plugid, json_t** result);

/**
 This function performs an HTTP request to the server for retrieving already published data

  @param cps The session reference.
  @param channel_mask @ref details_CHMASK The channel mask.
  @param before @ref details_TIMESTAMP or @ref details_OBJECT_ID Timestamp valid if greater than zero.
  @param after @ref details_TIMESTAMP or @ref details_OBJECT_ID Timestamp valid if greater than zero.
  @param at @ref details_TIMESTAMP_CSV Timestamp valid if greater than zero.
  @param of If not NULL, then @ref details_PLUG_ID_CSV.
  @param offset Offset valid if greater than zero.
  @param limit  Limit valid if greater than zero.
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_retrieve_data_json(cp_session cps, const char* channel_mask, cp_time before, cp_time after, cp_time at, const char* of, int offset, int limit, json_t** result);

/**
 This function performs an HTTP request to the server to publish data. Authentication is requested

  @param cps The session reference.
  @param channel A optional channel, if NULL data need to contain a couple "channel":"channel name"
  @param body JSON an object or an array of objects like this:\n
  {\n
    \b "id"        : @ref details_OBJECT_ID,\n
    \b "channel"   : @ref details_CHANNEL,	// optional, to override the channel in the url\n
    \b "data"      : JSON,\n
    \b "at"        : @ref details_TIMESTAMP_CSV ,\n
    \b "of"        : @ref details_PLUG_ID,	// optional, check if the X-Plug-Id is authorized for setting this field\n
    \b "is_priv"   : Boolean,	// optional, default false\n
    \b "expire_at" : @ref details_TIMESTAMP ,	// optional, expire date of this data entry\n
    \b "ttl"       : Number		// optional, how many *seconds* this data entry will live (if "expire_at" is present, then this field is ignored)\n
  }
  @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_publish_data_json(cp_session cps, const char* channel, json_t *body, json_t** result);

/**
 This function performs an HTTP request to the server to delete some published data

  @param cps The session reference.
  @param channel_mask The channel mask
  @param id     : @ref details_OBJECT_ID_CSV ,
  @param before : @ref details_TIMESTAMP or @ref details_OBJECT_ID ,
  @param after  : @ref details_TIMESTAMP or @ref details_OBJECT_ID ,
  @param at     : @ref details_TIMESTAMP_CSV,
  @param of     : @ref details_PLUG_ID_CSV ,
  @param result The response returned as a reference
  @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_remove_data_json(cp_session cps, const char* channel_mask, json_t* id, json_t* before, json_t* after, json_t* at, json_t* of, json_t** result);

/**
 This function performs an HTTP request to the server for writing or deleting device location and [optionally] it places the response in *result.

 @param cps The session reference.
 @param plugid If not NULL, then the @ref details_PLUG_ID of the device, otherwise the device referenced in the session.
 @param longitude
 @param latitude
 @param altitude //optional
 @param accuracy //optional
 @param timestamp //optional
 @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_set_device_location_json(cp_session cps, const char* plugid, double longitude, double latitude, double altitude, double accuracy, cp_time timestamp);

/**
 This function performs an HTTP request to the server for writing or deleting device location. It places the response in *result.

 @param cps The session reference.
 @param plugid If not NULL, then the @ref details_PLUG_ID of the device, otherwise the device referenced in the session.
 @param result A pointer such that *result will contain the dynamically allocated json object of the retrieved response body. The caller is responsible to free memory in *result.
 @return CP_SUCCESS if the request succeeds, CP_FAILED otherwise.
*/
cp_res cloudplugs_get_device_location_json(cp_session cps, const char* plugid, json_t** result);

#ifdef  __cplusplus
}
#endif

#endif // CP_REST_JSON_H
