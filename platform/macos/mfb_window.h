
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#ifndef MFB_WINDOW_H
#define MFB_WINDOW_H

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>
#include <mach/mach_time.h>

#include "mfb.h"
#include "mfb_time.h"

// ------------------------------------
@class mfb_window;
@class mfb_view_delegate;

// ------------------------------------
typedef struct {
    float x, y, z, w;
} mfb_vertex;

// ------------------------------------
typedef struct {
    void* user_data;

    mfb_active_func active_func;
    mfb_resize_func resize_func;
    mfb_close_func close_func;
    mfb_keyboard_func keyboard_func;
    mfb_char_input_func char_input_func;
    mfb_mouse_button_func mouse_btn_func;
    mfb_mouse_move_func mouse_move_func;
    mfb_mouse_drag_func mouse_drag_func;
    mfb_mouse_scroll_func mouse_wheel_func;

    uint32_t window_width;
    uint32_t window_height;

    uint32_t dst_offset_x;
    uint32_t dst_offset_y;
    uint32_t dst_width;
    uint32_t dst_height;
    float factor_x;
    float factor_y;
    float factor_width;
    float factor_height;

    void* draw_buffer;
    uint32_t buffer_width;
    uint32_t buffer_height;
    uint32_t buffer_stride;

    int32_t mouse_pos_x;
    int32_t mouse_pos_y;
    float mouse_wheel_x;
    float mouse_wheel_y;
    float mouse_wheel_z;
    uint8_t mouse_button_status[8];
    uint8_t key_status[512];
    uint32_t mod_keys;

    bool is_active;
    bool is_initialized;

    bool close;

    mfb_window* window;
    mfb_view_delegate* viewController;
    mfb_timer* timer;
    double elapsed;

    struct {
        mfb_vertex vertices[4];
    } metal;
    
    MTKView* render_view;

} mfb_window_data;

// ------------------------------------
@interface mfb_window : NSWindow <NSWindowDelegate> {
@public
    mfb_window_data* window_data;
@private
    NSView* childContentView;
}

- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(NSWindowStyleMask)windowStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)deferCreation
               windowData:(mfb_window_data*)windowData;

- (void)removeWindowData;

@end

// ------------------------------------
@interface mfb_view : NSView <NSTextInputClient> {
@public
    mfb_window_data* window_data;
@private
    NSTrackingArea* tracking_area;
}

@end

// ------------------------------------
// Number of textures in flight (tripple buffered)
enum { MaxBuffersInFlight = 3 };

@interface mfb_view_delegate : NSViewController <MTKViewDelegate> {
@public
    mfb_window_data* window_data;

    id<MTLDevice> metal_device;
    id<MTLLibrary> metal_library;

    dispatch_semaphore_t semaphore; // Used for syncing with CPU/GPU
    id<MTLCommandQueue> command_queue;

    id<MTLRenderPipelineState> pipeline_state;
    id<MTLTexture> texture_buffers[MaxBuffersInFlight];

    int current_buffer;
}

- (id)initWithWindowData:(mfb_window_data*)windowData;
- (void)resizeTextures;

@end

#endif
