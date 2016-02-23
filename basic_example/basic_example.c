/*
Copyright 2015 CloudPlugs Inc.

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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define AUTH_PLUGID "dev-xxxxxxxxxxxxxxxxxx" /**< The device plug ID or your CloudPlugs account id if AUTH_MASTER is CP_TRUE */
#define AUTH_PASS "your-password" /**< The device connection password or your CloudPlugs account password if AUTH_MASTER is CP_TRUE */
#define AUTH_MASTER CP_TRUE

int getTemp() {
    return rand() % 100;
}

int main(void) {
    srand (time(NULL));
    cloudplugs_global_init();
    printf("****CLOUDPLUGS EXAMPLE****\n");
    cp_session cps = cloudplugs_create_session();

    /**< By default the library uses a plain text connection to CloudPlugs,
         uncomment the following to change the behavior and to use a SSL secure connection */
    //cloudplugs_enable_ssl(cps, CP_TRUE);

    char* res = NULL;
    size_t res_len;
    cp_res cp_res;

    cloudplugs_set_auth(cps, AUTH_PLUGID, AUTH_PASS, AUTH_MASTER);

    const char* channel = "temperature";

    char buffer [15];
    sprintf (buffer, "{\"data\":%d}", getTemp());

    cp_res = cloudplugs_publish_data(cps, channel, buffer, &res, &res_len);
    if(cp_res)
        printf("PUBLISH: [ERROR-> %s] [HTTP_RES %d] %s\n", cloudplugs_get_last_err_string(cps), cloudplugs_get_last_http_result(cps), res);
    else
        printf("PUBLISH: %s\n", res);

    if(res) free(res);

    cloudplugs_destroy_session(cps);

    fflush(stdout);
    cloudplugs_global_shutdown();
    return 0;
}

