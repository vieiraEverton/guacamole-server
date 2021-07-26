/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "config.h"

#include "common/cursor.h"
#include "common/display.h"
#include "common/recording.h"
#include "vnc.h"

#include <guacamole/user.h>
#include <rfb/rfbclient.h>

int __guac_vnc_user_key_event_handler(rfbClient* rfb_client, int keysym, int pressed){
    if (keysym == 65489 && pressed == 0) // f20
        SendSwEvent(rfb_client, 1, 1); //TMB_ALTER_MONITOR

    else if (keysym == 65490 && pressed == 0) // f21
        SendCustomEvent(rfb_client, 0); // TMB_LAUNCH_ON_STARTUP

    else if (keysym == 65491 && pressed == 0) // f22
        SendCustomEvent(rfb_client, 1); // TMB_CUSTOM_CMD

    else if (keysym == 65492 && pressed == 1) // f23
        SendCustomEvent(rfb_client, 100); // TMB_ENABLE_TOUCH

    else if (keysym == 65492 && pressed == 0) // f23
        SendCustomEvent(rfb_client, 101); // TMB_DISABLE_TOUCH

    else if (keysym == 65493 && pressed == 0) // f24
        SendCustomEvent(rfb_client, 102); // TMB_PATIENT_BROWSER

    else if (keysym == 65494 && pressed == 0) // f25
        SendCustomEvent(rfb_client, 103); // TMB_START_EXAM

    else if (keysym == 65495 && pressed == 0) // f26
        SendCustomEvent(rfb_client, 104); // TMB_WRITE_TEXT_EXAM

    else
        return 0;

    return 1;
}

int guac_vnc_user_mouse_handler(guac_user* user, int x, int y, int mask) {

    guac_client* client = user->client;
    guac_vnc_client* vnc_client = (guac_vnc_client*) client->data;
    rfbClient* rfb_client = vnc_client->rfb_client;

    /* Store current mouse location/state */
    guac_common_cursor_update(vnc_client->display->cursor, user, x, y, mask);

    /* Report mouse position within recording */
    if (vnc_client->recording != NULL)
        guac_common_recording_report_mouse(vnc_client->recording, x, y, mask);

    /* Send VNC event only if finished connecting */
    if (rfb_client != NULL)
        SendPointerEvent(rfb_client, x, y, mask);

    return 0;
}

int guac_vnc_user_key_handler(guac_user* user, int keysym, int pressed) {

    guac_vnc_client* vnc_client = (guac_vnc_client*) user->client->data;
    rfbClient* rfb_client = vnc_client->rfb_client;

    /* Report key state within recording */
    if (vnc_client->recording != NULL)
        guac_common_recording_report_key(vnc_client->recording,
                keysym, pressed);

    /* Send VNC event only if finished connecting */
    if (rfb_client != NULL) {
        if (__guac_vnc_user_key_event_handler(rfb_client, keysym, pressed) == 1)
            return 0;
        SendKeyEvent(rfb_client, keysym, pressed);
    }

    return 0;
}

