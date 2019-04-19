/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at
 * contact@bluekitchen-gmbh.com
 *
 */


#define __BTSTACK_FILE__ "HIDProController.c"

// *****************************************************************************
/*
 * Nintendo Switch Pro Controller spoof - issue: Switch is disconnecting during pairing process.
 
 *  Nathan Reeves 2019
 */
// *****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "btstack.h"
#include "btstack_event.h"
#include "HIDProController.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "btstack_run_loop_freertos.h"

static uint8_t hid_service_buffer[450];
static uint8_t device_id_sdp_service_buffer[200];
static uint8_t l2cap_sdp_service_buffer[600];
static const char hid_device_name[] = "Wireless Gamepad";
static btstack_packet_callback_registration_t hci_event_callback_registration;
static uint16_t hid_cid;

static int pairing_state = 0;

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t * packet, uint16_t packet_size){
    UNUSED(channel);
    UNUSED(packet_size);
    switch (packet_type){
        case HCI_EVENT_PACKET:
            switch (packet[0]){
                case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                    log_info("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n", hci_event_user_confirmation_request_get_numeric_value(packet));
                    log_info("SSP User Confirmation Auto accept\n");
                    break;
                case HCI_EVENT_HID_META:
                    switch (hci_event_hid_meta_get_subevent_code(packet)){
                        case HID_SUBEVENT_CONNECTION_OPENED:
                            if (hid_subevent_connection_opened_get_status(packet)) return;
                            hid_cid = hid_subevent_connection_opened_get_hid_cid(packet);
                            hid_device_request_can_send_now_event(hid_cid); //start sending
                            break;
                        case HID_SUBEVENT_CONNECTION_CLOSED:
                            printf("HID Disconnected\n");
                            hid_cid = 0;
                            pairing_state = 0;
                            break;
                        case HID_SUBEVENT_CAN_SEND_NOW:
                            //send:
                            
                            //report to invoke pairing process
                            if(pairing_state == 0) hid_device_send_interrupt_message(hid_cid, &reply02[0], sizeof(reply02));
                            
                            //Switch sends 02 device info request...
                            
                            //respond to 0x02 device info request. sends MAC addr
                            if(pairing_state == 1) hid_device_send_interrupt_message(hid_cid, &reply02[0], sizeof(reply02));
                            
                            //Switch sends 08 request...
                            
                            //respond to 0x08
                            if(pairing_state == 2) hid_device_send_interrupt_message(hid_cid, &reply08[0], sizeof(reply08));
                            
                            pairing_state++;
   
                            if(pairing_state < 3)
                                hid_device_request_can_send_now_event(hid_cid); //goto next pairing state
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

//// next three functions create the SDP record - Some stuff might be extra for connecting to the Switch
/// Copied from an 8bitdo Bluetooth SNES Controller which advertises as a 'Pro Controller' 

void device_id_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t vendor_id_source, uint16_t vendor_id, uint16_t product_id, uint16_t version){
    //1200 Service Request: PNP info
    
    uint8_t* attribute;
    de_create_sequence(service);
    
    // 0x0000 "Service Record Handle"
    de_add_number(service, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
    de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);
    
    // 0x0001 "Service Class ID List"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, BLUETOOTH_SERVICE_CLASS_PNP_INFORMATION );
    }
    de_pop_sequence(service, attribute);
    //0x0004 "Protocol Descriptor List"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t * l2cpProtocol = de_push_sequence(attribute);
        {
            de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_L2CAP);
            de_add_number(l2cpProtocol,  DE_UINT, DE_SIZE_16, 0x01);
        }
        de_pop_sequence(attribute, l2cpProtocol);
        
        uint8_t * hidProtocol = de_push_sequence(attribute);
        {
            de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, 0x01);
        }
        de_pop_sequence(attribute, hidProtocol);
    }
    de_pop_sequence(service, attribute);
    //0x0006 Lang Base Attribute ID
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST);
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x656e);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x006a);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0100);
    }
    de_pop_sequence(service, attribute);
    //0x0009 Profile Descriptor List
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t * hidProfile = de_push_sequence(attribute);
        {
            de_add_number(hidProfile,  DE_UUID, DE_SIZE_16, BLUETOOTH_SERVICE_CLASS_PNP_INFORMATION);
            de_add_number(hidProfile,  DE_UINT, DE_SIZE_16, 0x0100);    // Version 1.1
        }
        de_pop_sequence(attribute, hidProfile);
    }
    de_pop_sequence(service, attribute);
    //0x0100
    const char wgp2[] = "Wireless Gamepad PnP Server";
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0100);
    de_add_data(service, DE_STRING, strlen(wgp2), (uint8_t *)wgp2);
    
    //Gamepad
    const char gp2[] = "Gamepad";
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0101);
    de_add_data(service, DE_STRING, strlen(gp2), (uint8_t *)gp2);

    
    // 0x0200 "SpecificationID"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SPECIFICATION_ID);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0103);    // v1.3
    
    // 0x0201 "VendorID"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_VENDOR_ID);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x057E);
    
    // 0x0202 "ProductID"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PRODUCT_ID);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x2007);
    
    // 0x0203 "Version"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_VERSION);
    de_add_number(service,  DE_UINT, DE_SIZE_16, version);
    
    // 0x0204 "PrimaryRecord"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PRIMARY_RECORD);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  1);    // yes, this is the primary record - there are no others
    
    // 0x0205 "VendorIDSource"
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_VENDOR_ID_SOURCE);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0002);
}

//Custom HID SDP record - taken from hid_device.c
void hid_sdp_record(
                           uint8_t *service,
                           uint32_t service_record_handle,
                           uint16_t hid_device_subclass,
                           uint8_t  hid_country_code,
                           uint8_t  hid_virtual_cable,
                           uint8_t  hid_reconnect_initiate,
                           uint8_t  hid_boot_device,
                           const uint8_t * descriptor, uint16_t descriptor_size,
                           const char *device_name){
    //1124 Service Request: Hunam Interface Device
    uint8_t * attribute;
    de_create_sequence(service);
    
    de_add_number(service, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
    de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);
    
    de_add_number(service, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, BLUETOOTH_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE);
    }
    de_pop_sequence(service, attribute);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t * l2cpProtocol = de_push_sequence(attribute);
        {
            de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_L2CAP);
            de_add_number(l2cpProtocol,  DE_UINT, DE_SIZE_16, PSM_HID_CONTROL);
        }
        de_pop_sequence(attribute, l2cpProtocol);
        
        uint8_t * hidProtocol = de_push_sequence(attribute);
        {
            de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_HIDP);
        }
        de_pop_sequence(attribute, hidProtocol);
    }
    de_pop_sequence(service, attribute);
    
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_BROWSE_GROUP_LIST); // public browse group
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT );
    }
    de_pop_sequence(service, attribute);
    

    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST);
    attribute = de_push_sequence(service);
    {
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x656e);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x006a);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0100);
    }
    de_pop_sequence(service, attribute);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t * hidProfile = de_push_sequence(attribute);
        {
            de_add_number(hidProfile,  DE_UUID, DE_SIZE_16, BLUETOOTH_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE);
            de_add_number(hidProfile,  DE_UINT, DE_SIZE_16, 0x0101);    // Version 1.1
        }
        de_pop_sequence(attribute, hidProfile);
    }
    de_pop_sequence(service, attribute);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS);
    attribute = de_push_sequence(service);
    {
        uint8_t * additionalDescriptorAttribute = de_push_sequence(attribute);
        {
            uint8_t * l2cpProtocol = de_push_sequence(additionalDescriptorAttribute);
            {
                de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_L2CAP);
                de_add_number(l2cpProtocol,  DE_UINT, DE_SIZE_16, PSM_HID_INTERRUPT);
            }
            de_pop_sequence(additionalDescriptorAttribute, l2cpProtocol);
            
            uint8_t * hidProtocol = de_push_sequence(additionalDescriptorAttribute);
            {
                de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_HIDP);
            }
            de_pop_sequence(additionalDescriptorAttribute, hidProtocol);
        }
        de_pop_sequence(attribute, additionalDescriptorAttribute);
    }
    de_pop_sequence(service, attribute);
    
    // 0x0100 "ServiceName"
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0100);
    de_add_data(service,  DE_STRING, strlen(device_name), (uint8_t *) device_name);
    
    const char gp[] = "Gamepad";
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0101);
    de_add_data(service, DE_STRING, strlen(gp), (uint8_t *)gp);
    
    const char nin[] = "Nintendo";
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0102);
    de_add_data(service, DE_STRING, strlen(nin), (uint8_t *)nin);
    
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_PARSER_VERSION);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0111);  // v1.1.1
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_DEVICE_SUBCLASS);
    de_add_number(service,  DE_UINT, DE_SIZE_8,  hid_device_subclass);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_COUNTRY_CODE);
    de_add_number(service,  DE_UINT, DE_SIZE_8,  hid_country_code);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_VIRTUAL_CABLE);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  1);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_RECONNECT_INITIATE);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  1);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_DESCRIPTOR_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t* hidDescriptor = de_push_sequence(attribute);
        {
            de_add_number(hidDescriptor,  DE_UINT, DE_SIZE_8, 0x22);    // Report Descriptor
            de_add_data(hidDescriptor,  DE_STRING, descriptor_size, (uint8_t *) descriptor);
        }
        de_pop_sequence(attribute, hidDescriptor);
    }
    de_pop_sequence(service, attribute);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HIDLANGID_BASE_LIST);
    attribute = de_push_sequence(service);
    {
        uint8_t* hig_lang_base = de_push_sequence(attribute);
        {
            // see: http://www.usb.org/developers/docs/USB_LANGIDs.pdf
            de_add_number(hig_lang_base,  DE_UINT, DE_SIZE_16, 0x0409);    // HIDLANGID = English (US)
            de_add_number(hig_lang_base,  DE_UINT, DE_SIZE_16, 0x0100);    // HIDLanguageBase = 0x0100 default
        }
        de_pop_sequence(attribute, hig_lang_base);
    }
    de_pop_sequence(service, attribute);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0209);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  1);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_REMOTE_WAKE);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  1);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x020C);
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x0C80);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, 0x20D);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  0);
    
    de_add_number(service,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_HID_BOOT_DEVICE);
    de_add_number(service,  DE_BOOL, DE_SIZE_8,  0);
}

void l2cap_create_sdp_record(void)
{
    //0100 Service Request: L2CAP
    uint8_t * attribute;
    de_create_sequence(l2cap_sdp_service_buffer);
    
    de_add_number(l2cap_sdp_service_buffer, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
    de_add_number(l2cap_sdp_service_buffer, DE_UINT, DE_SIZE_32, 0x0000);
    
    de_add_number(l2cap_sdp_service_buffer, DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, 0x1000);
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    //0x0004 "Protocol Descriptor List"
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        uint8_t * l2cpProtocol = de_push_sequence(attribute);
        {
            de_add_number(l2cpProtocol,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_L2CAP);
            de_add_number(l2cpProtocol,  DE_UINT, DE_SIZE_16, 0x01);
        }
        de_pop_sequence(attribute, l2cpProtocol);
        
        uint8_t * hidProtocol = de_push_sequence(attribute);
        {
            de_add_number(hidProtocol,  DE_UUID, DE_SIZE_16, 0x01);
        }
        de_pop_sequence(attribute, hidProtocol);
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    // Public browse group
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_BROWSE_GROUP_LIST);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        de_add_number(attribute,  DE_UUID, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT );
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    //0x0006 Lang Base Attribute ID
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x656e);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x006a);
        de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0100);
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    //0x0009 Profile Descriptor List
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        uint8_t * hidProfile = de_push_sequence(attribute);
        {
            de_add_number(hidProfile,  DE_UUID, DE_SIZE_16, BLUETOOTH_PROTOCOL_L2CAP);
            de_add_number(hidProfile,  DE_UINT, DE_SIZE_16, 0x0100);
        }
        de_pop_sequence(attribute, hidProfile);
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    //0x0100
    const char wgp2[] = "Wireless Gamepad";
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, 0x0100);
    de_add_data(l2cap_sdp_service_buffer, DE_STRING, strlen(wgp2), (uint8_t *)wgp2);
    
    //Gamepad
    const char gp2[] = "Gamepad";
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, 0x0101);
    de_add_data(l2cap_sdp_service_buffer, DE_STRING, strlen(gp2), (uint8_t *)gp2);
    
    // 0x0200 "SpecificationID"
    de_add_number(l2cap_sdp_service_buffer,  DE_UINT, DE_SIZE_16, BLUETOOTH_ATTRIBUTE_SPECIFICATION_ID);
    attribute = de_push_sequence(l2cap_sdp_service_buffer);
    {
        de_add_number(attribute,  DE_UINT, DE_SIZE_16, 0x0100);
    }
    de_pop_sequence(l2cap_sdp_service_buffer, attribute);
    
    //Register service
    sdp_register_service(l2cap_sdp_service_buffer);
}

int btstack_main(int argc, const char * argv[]);
int btstack_main(int argc, const char * argv[]){
    (void)argc;
    (void)argv;
    // register for HCI events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    hci_register_sco_packet_handler(&packet_handler);
    
    //hci_dump_open(NULL, HCI_DUMP_STDOUT); //Packet logging (slows things down)
    gap_discoverable_control(1);
    gap_set_class_of_device(0x2508); //joystick
    gap_set_local_name("Pro Controller"); //should be 'Pro Controller', 'Joy-Con (R)' or 'Joy-Con (L)'

    // L2CAP
    l2cap_init();
    
    // SDP Service
    sdp_init();
    memset(hid_service_buffer, 0, sizeof(hid_service_buffer));
    
    //1200 Service Request: PNP info
    device_id_sdp_record(device_id_sdp_service_buffer, 0x10001, DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH, BLUETOOTH_COMPANY_ID_BLUEKITCHEN_GMBH, 1, 1);
    printf("Device ID SDP service record size: %u\n", de_get_len((uint8_t*)device_id_sdp_service_buffer));
    sdp_register_service(device_id_sdp_service_buffer);
    
    //1124 Service Request: Hunam Interface Device - NOTE - comment out lines in sdp_server.c sdp_register_service() to allow for 0x10000 service handles
    hid_sdp_record(hid_service_buffer, 0x10000, 0x2508, 33, 0, 0, 1, hid_descriptor_pro_controller, sizeof(hid_descriptor_pro_controller), hid_device_name);
    printf("HID service record size: %u\n", de_get_len( hid_service_buffer));
    sdp_register_service(hid_service_buffer);
    
    //0100 Service Request: L2CAP
    l2cap_create_sdp_record();
    
    //init HID Device - hid_device.c
    hid_device_init(1, sizeof(hid_descriptor_pro_controller), hid_descriptor_pro_controller);
    hid_device_register_packet_handler(&packet_handler);
    
    // turn on
    hci_power_control(HCI_POWER_ON);
    
    return 0;
}
