//
//  HIDProController.h
//  
//
//  Created by Nathan Reeves on 12/9/18.
//

#include <stdio.h>
//"Pro Controller" -- HID Descriptor from 8Bitdo SF30 
const uint8_t hid_descriptor_pro_controller[] =
{
    0x05, 0x01, 0x15, 0x00, 0x09, 0x04, 0xa1, 0x01, 0x85, 0x30, 0x05, 0x01, 0x05, 0x09, 0x19, 0x01,
    0x29, 0x0a, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x0a, 0x55, 0x00, 0x65, 0x00, 0x81, 0x02,
    0x05, 0x09, 0x19, 0x0b, 0x29, 0x0e, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x04, 0x81, 0x02,
    0x75, 0x01, 0x95, 0x02, 0x81, 0x03, 0x0b, 0x01, 0x00, 0x01, 0x00, 0xa1, 0x00, 0x0b, 0x30, 0x00,
    0x01, 0x00, 0x0b, 0x31, 0x00, 0x01, 0x00, 0x0b, 0x32, 0x00, 0x01, 0x00, 0x0b, 0x35, 0x00, 0x01,
    0x00, 0x15, 0x00, 0x27, 0xff, 0xff, 0x00, 0x00, 0x75, 0x10, 0x95, 0x04, 0x81, 0x02, 0xc0, 0x0b,
    0x39, 0x00, 0x01, 0x00, 0x15, 0x00, 0x25, 0x07, 0x35, 0x00, 0x46, 0x3b, 0x01, 0x65, 0x14, 0x75,
    0x04, 0x95, 0x01, 0x81, 0x02, 0x05, 0x09, 0x19, 0x0f, 0x29, 0x12, 0x15, 0x00, 0x25, 0x01, 0x75,
    0x01, 0x95, 0x04, 0x81, 0x02, 0x75, 0x08, 0x95, 0x34, 0x81, 0x03, 0x06, 0x00, 0xff, 0x85, 0x21,
    0x09, 0x01, 0x75, 0x08, 0x95, 0x3f, 0x81, 0x03, 0x85, 0x81, 0x09, 0x02, 0x75, 0x08, 0x95, 0x3f,
    0x81, 0x03, 0x85, 0x01, 0x09, 0x03, 0x75, 0x08, 0x95, 0x3f, 0x91, 0x83, 0x85, 0x10, 0x09, 0x04,
    0x75, 0x08, 0x95, 0x3f, 0x91, 0x83, 0x85, 0x80, 0x09, 0x05, 0x75, 0x08, 0x95, 0x3f, 0x91, 0x83,
    0x85, 0x82, 0x09, 0x06, 0x75, 0x08, 0x95, 0x3f, 0x91, 0x83, 0xc0
    
};

//  Reports   //

static uint8_t inputreport30[] = {0xa1, 0x30, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00, 0x00 , 0x00 , 0x00  , 0x00 , 0x00};

//The device info reply contains the MAC addr D8:A0:1D:40:6B:22. Replace this with your own. Big endian
static uint8_t reply02[] = {0xa1, 0x21, 0x00, 0x40, 0x00, 0x00, 0x00, 0xe6, 0x27, 0x78, 0xab, 0xd7, 0x76, 0x00, 0x82, 0x02, 0x03, 0x48, 0x03, 0x02, 0xD8, 0xA0, 0x1D, 0x40, 0x6B, 0x22, 0x03, 0x01, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 };
static uint8_t reply08[] = {0xa1, 0x21, 0x02, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 };
static uint8_t reply03[] = {0xa1, 0x21, 0x05, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 };
static uint8_t reply04[] = {0xa1, 0x21, 0x06, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x83, 0x04, 0x00, 0xCC, 0x00, 0xEE, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00};
static uint8_t reply1060[] = {0xa1, 0x21, 0x03, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x90, 0x10, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 };
static uint8_t reply1050[] ={0xa1, 0x21, 0x04, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x90, 0x10, 0x50, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 , 0x00 , 0x00
    , 0x00 , 0x00 , 0x00 , 0x00  , 0x00 , 0x00 };
static uint8_t reply1080[] = {0xa1, 0x21, 0x04, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x90, 0x10, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t reply103D[] = {0xa1, 0x21, 0x05, 0x8E, 0x84, 0x00, 0x12, 0x01, 0x18, 0x80, 0x01, 0x18, 0x80, 0x80, 0x90, 0x10, 0x3D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t report3f[] = {0xa1, 0x3f, 0x00, 0x00, 0x08, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80};