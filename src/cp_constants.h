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
#ifndef CP_CONSTANTS_H
#define CP_CONSTANTS_H

#define CP_URL "http://api.cloudplugs.com/"
#define CP_MAX_URL_LENGTH 2048
#define CP_TIMEOUT 30

#define LIT_STR_LEN(x) (sizeof(x) - 1)

#define CP_HTTP_STR "http://"
#define CP_HTTPS_STR "https://"

//ERROR
#define CP_JSON_ERR "err"
#define CP_JSON_BODY "body"

#define CP_JSON_STRING_TRUE "true"
#define CP_JSON_STRING_FALSE "false"

#define PLUG_AUTH_HEADER "X-Plug-Auth: "
#define PLUG_ID_HEADER "X-Plug-Id: "
#define PLUG_EMAIL_HEADER "X-Plug-Email: "
#define PLUG_MASTER_HEADER "X-Plug-Master: "

#define CONTENT_TYPE_JSON "Content-type: application/json"

#define PATH_DATA "iot/data"
#define PATH_DEVICE "iot/device"
#define PATH_CHANNEL "iot/channel"

#define CTRL "ctrl"
#define HWID "hwid"
#define NAME "name"
#define MODEL "model"
#define PASS "pass"
#define PERM "perm"
#define PROPS "props"
#define DATA "data"
#define BEFORE "before"
#define AFTER "after"
#define OF "of"
#define OFFSET "offset"
#define LIMIT "limit"
#define AUTH "auth"
#define ID "id"
#define AT "at"

#define LOCATION "location"
#define LONGITUDE "x"
#define LATITUDE "y"
#define ACCURACY "r"
#define ALTITUDE "z"
#define TIMESTAMP "t"
#define MAX_LONGITUDE 180.0
#define MIN_LONGITUDE -180.0
#define MAX_LATITUDE 90.0
#define MIN_LATITUDE -90.0

#endif // CP_CONSTANTS_H
