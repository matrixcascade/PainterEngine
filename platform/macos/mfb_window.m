
// Copyright (c) 2014 Daniel Collin. All rights reserved.
// https://github.com/emoon/minifb MIT License

#include "mfb_window.h"
#include "mfb_display.h"
#include "mfb_time.h"
#include "mfb_common.h"

// ------------------------------------
short int g_keycodes[512] = {0};

void resize_dst(mfb_window_data* window_data, uint32_t width, uint32_t height);

// ------------------------------------
uint64_t mfb_timer_tick() {
    static mach_timebase_info_data_t timebase = {0};

    if (timebase.denom == 0) {
        (void)mach_timebase_info(&timebase);
    }

    uint64_t time = mach_absolute_time();

    // Perform the arithmetic at 128-bit precision to avoid the overflow!
    uint64_t high = (time >> 32) * timebase.numer;
    uint64_t highRem = ((high % timebase.denom) << 32) / timebase.denom;
    uint64_t low = (time & 0xFFFFFFFFull) * timebase.numer / timebase.denom;
    high /= timebase.denom;

    return (high << 32) + highRem + low;
}

// ------------------------------------
void mfb_timer_init() {
    g_timer_frequency = 1e+9;
    g_timer_resolution = 1.0 / g_timer_frequency;
}

// ------------------------------------
@implementation mfb_window

// ------------------
- (id)initWithContentRect:(NSRect)contentRect
                styleMask:(NSWindowStyleMask)windowStyle
                  backing:(NSBackingStoreType)bufferingType
                    defer:(BOOL)deferCreation
               windowData:(mfb_window_data*)windowData {
    self = [super initWithContentRect:contentRect styleMask:windowStyle backing:bufferingType defer:deferCreation];

    if (self) {
        [self setOpaque:YES];
        [self setBackgroundColor:[NSColor clearColor]];

        self.delegate = self;

        self->window_data = windowData;
        mfb_view* view = (mfb_view*)self->childContentView.superview;
        view->window_data = windowData;
    }
    return self;
}

// ------------------
- (void)removeWindowData {
    self->window_data = 0x0;
    mfb_view* view = (mfb_view*)self->childContentView.superview;
    view->window_data = 0x0;
}

// ------------------
- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    // [super dealloc];
}

// ------------------
- (void)setContentSize:(NSSize)newSize {
    NSSize sizeDelta = newSize;
    NSSize childBoundsSize = [childContentView bounds].size;
    sizeDelta.width -= childBoundsSize.width;
    sizeDelta.height -= childBoundsSize.height;

    mfb_view* frameView = [super contentView];
    NSSize newFrameSize = [frameView bounds].size;
    newFrameSize.width += sizeDelta.width;
    newFrameSize.height += sizeDelta.height;

    [super setContentSize:newFrameSize];
}

// ------------------
- (void)flagsChanged:(NSEvent*)event {
    if (window_data == 0x0) return;

    const uint64_t flags = [event modifierFlags];
    uint32_t mod_keys = 0, mod_keys_aux = 0;

    // NSEventModifierFlagHelp = 1 << 22,
    // NSEventModifierFlagFunction = 1 << 23,
    if (flags & NSEventModifierFlagCapsLock) {
        mod_keys |= KB_MOD_CAPS_LOCK;
    }
    if (flags & NSEventModifierFlagShift) {
        mod_keys |= KB_MOD_SHIFT;
    }
    if (flags & NSEventModifierFlagControl) {
        mod_keys |= KB_MOD_CONTROL;
    }
    if (flags & NSEventModifierFlagOption) {
        mod_keys |= KB_MOD_ALT;
    }
    if (flags & NSEventModifierFlagCommand) {
        mod_keys |= KB_MOD_SUPER;
    }
    if (flags & NSEventModifierFlagNumericPad) {
        mod_keys |= KB_MOD_NUM_LOCK;
    }

    if (mod_keys != window_data->mod_keys) {
        short int key_code = g_keycodes[[event keyCode] & 0x1ff];
        if (key_code != KB_KEY_UNKNOWN) {
            mod_keys_aux = mod_keys ^ window_data->mod_keys;
            if (mod_keys_aux & KB_MOD_CAPS_LOCK) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_CAPS_LOCK) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
            if (mod_keys_aux & KB_MOD_SHIFT) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_SHIFT) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
            if (mod_keys_aux & KB_MOD_CONTROL) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_CONTROL) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
            if (mod_keys_aux & KB_MOD_ALT) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_ALT) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
            if (mod_keys_aux & KB_MOD_SUPER) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_SUPER) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
            if (mod_keys_aux & KB_MOD_NUM_LOCK) {
                window_data->key_status[key_code] = (mod_keys & KB_MOD_NUM_LOCK) != 0;
                kCall(keyboard_func, key_code, mod_keys, window_data->key_status[key_code]);
            }
        }
    }
    window_data->mod_keys = mod_keys;

    [super flagsChanged:event];
}

// ------------------
- (void)keyDown:(NSEvent*)event {
    if (window_data != 0x0) {
        short int key_code = g_keycodes[[event keyCode] & 0x1ff];
        window_data->key_status[key_code] = true;
        kCall(keyboard_func, key_code, window_data->mod_keys, true);
    }
    [childContentView.superview interpretKeyEvents:@[ event ]];
}

// ------------------
- (void)keyUp:(NSEvent*)event {
    if (window_data != 0x0) {
        short int key_code = g_keycodes[[event keyCode] & 0x1ff];
        window_data->key_status[key_code] = false;
        kCall(keyboard_func, key_code, window_data->mod_keys, false);
    }
}

// ------------------
- (void)mainWindowChanged:(NSNotification*)notification {
    kUnused(notification);

    if (window_data != 0x0) {
        if (window_data->is_active == true) {
            window_data->is_active = false;
            kCall(active_func, false);
        }
    }
}

// ------------------
- (void)setContentView:(NSView*)aView {
    if ([childContentView isEqualTo:aView]) {
        return;
    }

    NSRect bounds = [self frame];
    bounds.origin = NSZeroPoint;

    mfb_view* frameView = [super contentView];
    if (!frameView) {
        frameView = [[mfb_view alloc] initWithFrame:bounds];

        [super setContentView:frameView];
    }

    if (childContentView) {
        [childContentView removeFromSuperview];
    }
    childContentView = aView;
    [childContentView setFrame:[self contentRectForFrameRect:bounds]];
    [childContentView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
    [frameView addSubview:childContentView];
}

// ------------------
- (NSView*)contentView {
    return childContentView;
}

// ------------------
- (BOOL)canBecomeKeyWindow {
    return YES;
}

// ------------------
- (void)windowDidBecomeKey:(NSNotification*)notification {
    kUnused(notification);
    if (window_data != 0x0) {
        window_data->is_active = true;
        kCall(active_func, true);
    }
}

// ------------------
- (void)windowDidResignKey:(NSNotification*)notification {
    kUnused(notification);
    if (window_data) {
        window_data->is_active = false;
        kCall(active_func, false);
    }
}

// ------------------
- (BOOL)windowShouldClose:(NSWindow*)window {
    bool destroy = false;
    if (!window_data) {
        destroy = true;
    } else {
        // Obtain a confirmation of close
        if (!window_data->close_func || window_data->close_func((struct mfb_opaque_window*)window_data)) {
            destroy = true;
        }
    }

    return destroy;
}

// ------------------
- (void)windowWillClose:(NSNotification*)notification {
    kUnused(notification);
    if (window_data) {
        window_data->close = true;
    }
}

// ------------------
- (BOOL)canBecomeMainWindow {
    return YES;
}

// ------------------
- (NSRect)contentRectForFrameRect:(NSRect)windowFrame {
    windowFrame.origin = NSZeroPoint;
    return NSInsetRect(windowFrame, 0, 0);
}

// ------------------
+ (NSRect)frameRectForContentRect:(NSRect)windowContentRect styleMask:(NSWindowStyleMask)windowStyle {
    kUnused(windowStyle);
    return NSInsetRect(windowContentRect, 0, 0);
}

// ------------------
- (void)willClose {
    if (window_data != 0x0) {
        window_data->close = true;
    }
}

// ------------------
- (void)windowDidResize:(NSNotification*)notification {
    kUnused(notification);
    if (window_data != 0x0) {
        CGSize size = [self contentRectForFrameRect:[self frame]].size;

        window_data->window_width = size.width;
        window_data->window_height = size.height;
        resize_dst(window_data, size.width, size.height);

        kCall(resize_func, size.width, size.height);
    }
}

@end

// ------------------------------------
@implementation mfb_view

// ------------------
- (void)updateTrackingAreas {
    if (tracking_area != nil) {
        [self removeTrackingArea:tracking_area];
        // [tracking_area release];
    }

    int opts = (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways);
    tracking_area = [[NSTrackingArea alloc] initWithRect:[self bounds] options:opts owner:self userInfo:nil];
    [self addTrackingArea:tracking_area];
}

// ------------------
- (BOOL)acceptsFirstMouse:(NSEvent*)event {
    (void)event;
    return YES;
}

// ------------------
- (void)mouseDown:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[MOUSE_BTN_1] = true;
        kCall(mouse_btn_func, MOUSE_BTN_1, window_data->mod_keys, true);
    }
}

// ------------------
- (void)mouseUp:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[MOUSE_BTN_1] = false;
        kCall(mouse_btn_func, MOUSE_BTN_1, window_data->mod_keys, false);
    }
}

// ------------------
- (void)rightMouseDown:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[MOUSE_BTN_2] = true;
        kCall(mouse_btn_func, MOUSE_BTN_2, window_data->mod_keys, true);
    }
}

// ------------------
- (void)rightMouseUp:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[MOUSE_BTN_2] = false;
        kCall(mouse_btn_func, MOUSE_BTN_2, window_data->mod_keys, false);
    }
}

// ------------------
- (void)otherMouseDown:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[[event buttonNumber] & 0x07] = true;
        kCall(mouse_btn_func, (mfb_mouse_button)[event buttonNumber], window_data->mod_keys, true);
    }
}

// ------------------
- (void)otherMouseUp:(NSEvent*)event {
    (void)event;
    if (window_data != 0x0) {
        window_data->mouse_button_status[[event buttonNumber] & 0x07] = false;
        kCall(mouse_btn_func, (mfb_mouse_button)[event buttonNumber], window_data->mod_keys, false);
    }
}

// ------------------
- (void)scrollWheel:(NSEvent*)event {
    if (window_data != 0x0) {
        window_data->mouse_wheel_x = [event deltaX];
        window_data->mouse_wheel_y = [event deltaY];
        window_data->mouse_wheel_z = [event deltaZ];
        kCall(mouse_wheel_func, window_data->mod_keys, window_data->mouse_wheel_x, window_data->mouse_wheel_y, window_data->mouse_wheel_z);
    }
}

// ------------------
- (void)mouseDragged:(NSEvent*)event {
    [self mouseMoved:event];
}

// ------------------
- (void)rightMouseDragged:(NSEvent*)event {
    [self mouseMoved:event];
}

// ------------------
- (void)otherMouseDragged:(NSEvent*)event {
    [self mouseMoved:event];
}

// ------------------
- (void)mouseMoved:(NSEvent*)event {
    if (window_data != 0x0) {
        NSPoint point = [event locationInWindow];
        // NSPoint localPoint = [self convertPoint:point fromView:nil];
        window_data->mouse_pos_x = point.x;
#if defined(USE_INVERTED_Y_ON_MACOS)
        window_data->mouse_pos_y = point.y;
#else
        window_data->mouse_pos_y = window_data->window_height - point.y;
#endif
        if (event.type == NSEventTypeLeftMouseDragged) {
            // Handling mouse movement during drag and drop
            kCall(mouse_drag_func, window_data->mouse_pos_x, window_data->mouse_pos_y);
        } else {
            // Handling regular mouse movement
            kCall(mouse_move_func, window_data->mouse_pos_x, window_data->mouse_pos_y);
        }
    }
}

// ------------------
- (void)mouseExited:(NSEvent*)event {
    (void)event;
    // printf("mouse exit\n");
}

// ------------------
- (void)mouseEntered:(NSEvent*)event {
    (void)event;
    // printf("mouse enter\n");
}

// ------------------
- (BOOL)canBecomeKeyView {
    return YES;
}

// ------------------
- (NSView*)nextValidKeyView {
    return self;
}

// ------------------
- (NSView*)previousValidKeyView {
    return self;
}

// ------------------
- (BOOL)acceptsFirstResponder {
    return YES;
}

// ------------------
- (void)viewDidMoveToWindow {
}

// ------------------
- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    // [super dealloc];
}

#pragma mark NSTextInputClient

// Invokes the action specified by the given selector.
// ------------------
- (void)doCommandBySelector:(nonnull SEL)selector {
    kUnused(selector);
}

// Returns an attributed string derived from the given range in the receiver's text storage.
// ------------------
- (nullable NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {
    kUnused(range);
    kUnused(actualRange);
    return nil;
}

// Inserts the given string into the receiver, replacing the specified content.
// ------------------
- (void)insertText:(nonnull id)string replacementRange:(NSRange)replacementRange {
    kUnused(replacementRange);

    if (window_data != 0x0) {
        NSString* characters;
        unsigned int codepoint;

        if ([string isKindOfClass:[NSAttributedString class]])
            characters = [string string];
        else
            characters = (NSString*)string;

        NSRange range = NSMakeRange(0, [characters length]);
        while (range.length) {
            codepoint = 0;
            if ([characters getBytes:&codepoint
                           maxLength:sizeof(codepoint)
                          usedLength:NULL
                            encoding:NSUTF8StringEncoding // NSUTF32StringEncoding
                             options:0
                               range:range
                      remainingRange:&range]) {
                if ((codepoint & 0xff00) == 0xf700) continue;

                kCall(char_input_func, codepoint);
            }
        }
    }
}

// Returns the index of the character whose bounding rectangle includes the given point.
// ------------------
- (NSUInteger)characterIndexForPoint:(NSPoint)point {
    kUnused(point);
    return 0;
}

// Returns the first logical boundary rectangle for characters in the given range.
// ------------------
- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange {
    kUnused(range);
    kUnused(actualRange);
    return NSMakeRect(0.0, 0.0, 0.0, 0.0);
}

// ------------------
static const NSRange kEmptyRange = {NSNotFound, 0};

// Returns a Boolean value indicating whether the receiver has marked text.
// ------------------
- (BOOL)hasMarkedText {
    return false;
}

// Returns the range of the marked text.
// ------------------
- (NSRange)markedRange {
    return kEmptyRange;
}

// Returns the range of selected text.
// ------------------
- (NSRange)selectedRange {
    return kEmptyRange;
}

// Replaces a specified range in the receiver’s text storage with the given string and sets the selection.
// ------------------
- (void)setMarkedText:(nonnull id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {
    kUnused(string);
    kUnused(selectedRange);
    kUnused(replacementRange);
}

// ------------------
- (void)unmarkText {
}

// Returns an array of attribute names recognized by the receiver.
// ------------------
- (nonnull NSArray<NSString*>*)validAttributesForMarkedText {
    return [NSArray array];
}

@end

// ------------------------------------
#define kShader(inc, src) @inc #src

NSString* g_shader_src = kShader(
    "#include <metal_stdlib>\n", using namespace metal;

    struct VertexOutput {
        float4 pos [[position]];
        float2 texcoord;
    };

    struct Vertex { float4 position [[position]]; };

    vertex VertexOutput vertFunc(unsigned int vID [[vertex_id]], const device Vertex* pos [[buffer(0)]]) {
        VertexOutput out;

        out.pos = pos[vID].position;

        out.texcoord.x = (float)(vID / 2);
        out.texcoord.y = 1.0 - (float)(vID % 2);

        return out;
    }

    fragment float4 fragFunc(VertexOutput input [[stage_in]], texture2d<half> colorTexture [[texture(0)]]) {
        constexpr sampler textureSampler(mag_filter::nearest, min_filter::nearest);

        // Sample the texture to obtain a color
        const half4 colorSample = colorTexture.sample(textureSampler, input.texcoord);

        // We return the color of the texture
        // return float4(colorSample);

        // Converting color from RGBA to BGRA format
        return float4(colorSample.b, colorSample.g, colorSample.r, colorSample.a);
    });

// ------------------------------------
@implementation mfb_view_delegate

// ------------------
- (id)initWithWindowData:(mfb_window_data*)windowData {
    self = [super init];
    if (self) {
        window_data = windowData;

        metal_device = MTLCreateSystemDefaultDevice();
        if (!metal_device) {
            NSLog(@"Metal is not supported on this device");
            return 0x0;
        }

        // Used for syncing the CPU and GPU
        semaphore = dispatch_semaphore_create(MaxBuffersInFlight);

        // Setup command queue
        command_queue = [metal_device newCommandQueue];

        [self _createShaders];
        [self _createAssets];
    }
    return self;
}

// ------------------
- (bool)_createShaders {
    NSError* error = 0x0;

    metal_library = [metal_device newLibraryWithSource:g_shader_src options:[[MTLCompileOptions alloc] init] error:&error];
    if (error || !metal_library) {
        NSLog(@"Unable to create shaders %@", error);
        return false;
    }

    id<MTLFunction> vertex_shader_func = [metal_library newFunctionWithName:@"vertFunc"];
    id<MTLFunction> fragment_shader_func = [metal_library newFunctionWithName:@"fragFunc"];

    if (!vertex_shader_func) {
        NSLog(@"Unable to get vertFunc!\n");
        return false;
    }

    if (!fragment_shader_func) {
        NSLog(@"Unable to get fragFunc!\n");
        return false;
    }

    // Create a reusable pipeline state
    MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineStateDescriptor.label = @"painter_pipeline";
    pipelineStateDescriptor.vertexFunction = vertex_shader_func;
    pipelineStateDescriptor.fragmentFunction = fragment_shader_func;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = 80; // bgra8Unorm;

    pipeline_state = [metal_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
    if (!pipeline_state) {
        NSLog(@"Failed to created pipeline state, error %@", error);
        return false;
    }

    return true;
}

// ------------------
- (void)_createAssets {
    static mfb_vertex s_vertices[4] = {
        {-1.0, -1.0, 0, 1},
        {-1.0, 1.0, 0, 1},
        {1.0, -1.0, 0, 1},
        {1.0, 1.0, 0, 1},
    };
    memcpy(window_data->metal.vertices, s_vertices, sizeof(s_vertices));

    MTLTextureDescriptor* td;
    td = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                            width:window_data->buffer_width
                                                           height:window_data->buffer_height
                                                        mipmapped:false];

    // Create the texture from the device by using the descriptor
    for (size_t i = 0; i < MaxBuffersInFlight; ++i) {
        texture_buffers[i] = [metal_device newTextureWithDescriptor:td];
    }
}

// ------------------
- (void)resizeTextures {
    MTLTextureDescriptor* td;
    td = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                            width:window_data->buffer_width
                                                           height:window_data->buffer_height
                                                        mipmapped:false];

    // Create the texture from the device by using the descriptor
    for (size_t i = 0; i < MaxBuffersInFlight; ++i) {
        // [texture_buffers[i] release];
        texture_buffers[i] = [metal_device newTextureWithDescriptor:td];
    }
}

// ------------------
- (void)drawInMTKView:(nonnull MTKView*)view {
    // Wait to ensure only MaxBuffersInFlight number of frames are getting proccessed
    // by any stage in the Metal pipeline (App, Metal, Drivers, GPU, etc)
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);

    current_buffer = (current_buffer + 1) % MaxBuffersInFlight;

    // Create a new command buffer for each render pass to the current drawable
    id<MTLCommandBuffer> commandBuffer = [command_queue commandBuffer];
    commandBuffer.label = @"painter_command_buffer";

    // Add completion hander which signals semaphore when Metal and the GPU has fully
    // finished processing the commands we're encoding this frame.  This indicates when the
    // dynamic buffers filled with our vertices, that we're writing to this frame, will no longer
    // be needed by Metal and the GPU, meaning we can overwrite the buffer contents without
    // corrupting the rendering.
    __block dispatch_semaphore_t block_sema = semaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
      (void)buffer;
      dispatch_semaphore_signal(block_sema);
    }];

    // Copy the bytes from our data object into the texture
    MTLRegion region = {{0, 0, 0}, {window_data->buffer_width, window_data->buffer_height, 1}};
    [texture_buffers[current_buffer] replaceRegion:region
                                       mipmapLevel:0
                                         withBytes:window_data->draw_buffer
                                       bytesPerRow:window_data->buffer_stride];

    // Delay getting the currentRenderPassDescriptor until absolutely needed. This avoids
    // holding onto the drawable and blocking the display pipeline any longer than necessary
    MTLRenderPassDescriptor* renderPassDescriptor = view.currentRenderPassDescriptor;
    if (renderPassDescriptor != nil) {
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);

        // Create a render command encoder so we can render into something
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        renderEncoder.label = @"painter_command_encoder";

        // Set render command encoder state
        [renderEncoder setRenderPipelineState:pipeline_state];
        [renderEncoder setVertexBytes:window_data->metal.vertices length:sizeof(window_data->metal.vertices) atIndex:0];

        [renderEncoder setFragmentTexture:texture_buffers[current_buffer] atIndex:0];

        // Draw the vertices of our quads
        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:0 vertexCount:4];

        // We're done encoding commands
        [renderEncoder endEncoding];

        [commandBuffer presentDrawable:view.currentDrawable];
    }

    // Finalize rendering here & push the command buffer to the GPU
    [commandBuffer commit];
}

// ------------------
- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size {
    (void)view;
    (void)size;
}

@end

// ------------------------------------
void calc_dst_factor(mfb_window_data* window_data, uint32_t width, uint32_t height) {
    if (window_data->dst_width == 0) {
        window_data->dst_width = width;
    }
    window_data->factor_x = (float)window_data->dst_offset_x / (float)width;
    window_data->factor_width = (float)window_data->dst_width / (float)width;

    if (window_data->dst_height == 0) {
        window_data->dst_height = height;
    }
    window_data->factor_y = (float)window_data->dst_offset_y / (float)height;
    window_data->factor_height = (float)window_data->dst_height / (float)height;
}

// ------------------------------------
void resize_dst(mfb_window_data* window_data, uint32_t width, uint32_t height) {
    window_data->dst_offset_x = (uint32_t)(width * window_data->factor_x);
    window_data->dst_offset_y = (uint32_t)(height * window_data->factor_y);
    window_data->dst_width = (uint32_t)(width * window_data->factor_width);
    window_data->dst_height = (uint32_t)(height * window_data->factor_height);
}

// ------------------------------------
void init_keycodes();

// ------------------------------------
mfb_window_data* create_window_data(unsigned width, unsigned height) {
    mfb_window_data* window_data;

    window_data = malloc(sizeof(mfb_window_data));
    if (window_data == 0x0) {
        NSLog(@"Cannot allocate window data");
        return 0x0;
    }
    memset((void*)window_data, 0, sizeof(mfb_window_data));

    calc_dst_factor(window_data, width, height);

    window_data->buffer_width = width;
    window_data->buffer_height = height;
    window_data->buffer_stride = width * 4;

    window_data->draw_buffer = malloc(width * height * 4);
    if (!window_data->draw_buffer) {
        free(window_data);
        NSLog(@"Unable to create draw buffer");
        return 0x0;
    }

    return window_data;
}

// ------------------------------------
struct mfb_opaque_window* mfb_open_ex(const char* title, unsigned width, unsigned height, unsigned flags) {
    @autoreleasepool {
        mfb_window_data* window_data = create_window_data(width, height);
        if (window_data == 0x0) {
            return 0x0;
        }

        init_keycodes();

        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        NSRect rectangle, frameRect;
        NSWindowStyleMask styles = 0;

        if (flags & WF_BORDERLESS) {
            styles |= NSWindowStyleMaskBorderless;
        } else {
            styles |= NSWindowStyleMaskClosable | NSWindowStyleMaskTitled;
        }

        if (flags & WF_RESIZABLE) styles |= NSWindowStyleMaskResizable;

        if (flags & WF_FULLSCREEN) {
            styles = NSWindowStyleMaskFullScreen;
            NSScreen* mainScreen = [NSScreen mainScreen];
            NSRect screenRect = [mainScreen frame];
            window_data->window_width = screenRect.size.width;
            window_data->window_height = screenRect.size.height;
            rectangle = NSMakeRect(0, 0, window_data->window_width, window_data->window_height);
            frameRect = rectangle;
        } else if (flags & WF_FULLSCREEN_DESKTOP) {
            NSScreen* mainScreen = [NSScreen mainScreen];
            NSRect screenRect = [mainScreen visibleFrame];
            window_data->window_width = screenRect.size.width;
            window_data->window_height = screenRect.size.height;
            rectangle = NSMakeRect(0, 0, window_data->window_width, window_data->window_height);
            frameRect = rectangle;
        } else {
            window_data->window_width = width;
            window_data->window_height = height;
            rectangle = NSMakeRect(0, 0, window_data->window_width, window_data->window_height);
            frameRect = [NSWindow frameRectForContentRect:rectangle styleMask:styles];
        }

        window_data->window = [[mfb_window alloc] initWithContentRect:frameRect
                                                            styleMask:styles
                                                              backing:NSBackingStoreBuffered
                                                                defer:NO
                                                           windowData:window_data];
        if (!window_data->window) {
            NSLog(@"Cannot create window");
            if (window_data->draw_buffer != 0x0) {
                free(window_data->draw_buffer);
                window_data->draw_buffer = 0x0;
            }
            free(window_data);
            return 0x0;
        }

        window_data->viewController = [[mfb_view_delegate alloc] initWithWindowData:window_data];

        MTKView* view = [[MTKView alloc] initWithFrame:rectangle];
        view.device = window_data->viewController->metal_device;
        view.delegate = window_data->viewController;
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        view.preferredFramesPerSecond = 0;

        window_data->render_view = view;
        [window_data->window.contentView addSubview:view];

        window_data->timer = mfb_timer_create();

        [window_data->window setTitle:[NSString stringWithUTF8String:title]];
        [window_data->window setReleasedWhenClosed:NO];
        [window_data->window performSelectorOnMainThread:@selector(makeKeyAndOrderFront:) withObject:nil waitUntilDone:YES];
        [window_data->window setAcceptsMouseMovedEvents:YES];

        [window_data->window center];

        [NSApp activateIgnoringOtherApps:YES];

        [NSApp finishLaunching];

        mfb_set_keyboard_callback((struct mfb_opaque_window*)window_data, keyboard_default);

        NSLog(@"Window created using Metal API");
        
        window_data->elapsed = 0;
        window_data->is_initialized = true;
        return (struct mfb_opaque_window*)window_data;
    }
}

// ------------------------------------
static void destroy_window_data(mfb_window_data* window_data) {
    if (window_data == 0x0) return;

    @autoreleasepool {
        if (window_data != 0x0) {
            mfb_window* window = window_data->window;
            [window performClose:nil];

            // Flush events!
            NSEvent* event;
            do {
                event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
                if (event) {
                    [NSApp sendEvent:event];
                }
            } while (event);
            [window removeWindowData];

            mfb_timer_destroy(window_data->timer);
        }

        if (window_data->draw_buffer != 0x0) {
            free(window_data->draw_buffer);
            window_data->draw_buffer = 0x0;
        }

        memset((void*)window_data, 0, sizeof(mfb_window_data));
        free(window_data);
    }
}

// ------------------------------------
static void update_events(mfb_window_data* window_data) {
    NSEvent* event;

    @autoreleasepool {
        do {
            event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
            if (event) {
                [NSApp sendEvent:event];
            }
        } while ((window_data->close == false) && event);
    }
}

// ------------------------------------
mfb_state mfb_window_timer_reset(struct mfb_opaque_window* window) {
    if (window == 0x0) {
        return STATE_INVALID_WINDOW;
    }

    mfb_window_data* window_data = (mfb_window_data*)window;
    if (window_data->close) {
        return STATE_EXIT;
    }

    mfb_timer_reset(window_data->timer);
    mfb_timer_now(window_data->timer);

    return STATE_OK;
}

// ------------------------------------
unsigned mfb_get_screen_width() {
    NSSize screenSize = [[NSScreen mainScreen] frame].size;
    CGFloat screenWidth = screenSize.width;
    return screenWidth;
}

// ------------------------------------
unsigned mfb_get_screen_height() {
    NSSize screenSize = [[NSScreen mainScreen] frame].size;
    CGFloat screenHeight = screenSize.height;
    return screenHeight;
}

// ------------------------------------
mfb_state mfb_window_update(struct mfb_opaque_window* window, void* buffer, unsigned width, unsigned height) {
    if (window == 0x0) {
        return STATE_INVALID_WINDOW;
    }

    mfb_window_data* window_data = (mfb_window_data*)window;
    if (window_data->close) {
        return STATE_EXIT;
    }

    double current;
    uint32_t millis = 1;
    current = mfb_timer_now(window_data->timer);

    if (buffer == 0x0) {
        return STATE_INVALID_BUFFER;
    }

    if (window_data->buffer_width != width || window_data->buffer_height != height) {
        window_data->buffer_width = width;
        window_data->buffer_stride = width * 4;
        window_data->buffer_height = height;
        window_data->draw_buffer = realloc(window_data->draw_buffer, window_data->buffer_stride * window_data->buffer_height);

        [window_data->viewController resizeTextures];
    }

    memcpy(window_data->draw_buffer, buffer, window_data->buffer_stride * window_data->buffer_height);

    if (window_data->close) {
        return STATE_EXIT;
    }

    [window_data->render_view draw];

    while (1) {
        current = mfb_timer_now(window_data->timer);
        if (current >= g_time_for_frame * 0.960) {
            mfb_timer_reset(window_data->timer);
            window_data->elapsed += current; // microseconds per frame
            return STATE_OK;
        } else if (current >= g_time_for_frame * 0.900) {
            usleep((millis * 0) * 1000);
        } else {
            usleep((millis * 1) * 1000);
        }
    }

    return STATE_OK;
}

// ------------------------------------
mfb_state mfb_window_update_events(struct mfb_opaque_window* window) {
    if (window == 0x0) {
        return STATE_INVALID_WINDOW;
    }

    mfb_window_data* window_data = (mfb_window_data*)window;
    if (window_data->close) {
        destroy_window_data(window_data);
        return STATE_EXIT;
    }

    update_events(window_data);
    if (window_data->close) {
        destroy_window_data(window_data);
        return STATE_EXIT;
    }

    // [[window_data->window contentView] setNeedsDisplay:YES];

    return STATE_OK;
}

// ------------------------------------
int mfb_wait_sync(struct mfb_opaque_window* window) {
    NSEvent* event;

    if (window == 0x0) {
        return false;
    }

    mfb_window_data* window_data = (mfb_window_data*)window;
    if (window_data->close) {
        destroy_window_data(window_data);
        return false;
    }

    if (g_use_hardware_sync) {
        return true;
    }

    @autoreleasepool {
        if (window_data == 0x0) {
            return false;
        }

        while (1) {
            event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantFuture] inMode:NSDefaultRunLoopMode dequeue:YES];
            if (event) {
                pthread_mutex_lock(&_eventMutex);
                [NSApp sendEvent:event];
                pthread_mutex_unlock(&_eventMutex);

                if (window_data->close) {
                    destroy_window_data(window_data);
                    break;
                }
            }
        }
    }

    return true;
}

// ------------------------------------
int mfb_set_viewport(struct mfb_opaque_window* window, unsigned offset_x, unsigned offset_y, unsigned width, unsigned height) {
    if (window == 0x0) {
        return false;
    }

    mfb_window_data* window_data = (mfb_window_data*)window;

    if (offset_x + width > window_data->window_width) {
        return false;
    }
    if (offset_y + height > window_data->window_height) {
        return false;
    }

    window_data->dst_offset_x = offset_x;
    window_data->dst_offset_y = offset_y;
    window_data->dst_width = width;
    window_data->dst_height = height;
    calc_dst_factor(window_data, window_data->window_width, window_data->window_height);

    float x1 = ((float)offset_x / window_data->window_width) * 2.0f - 1.0f;
    float x2 = (((float)offset_x + width) / window_data->window_width) * 2.0f - 1.0f;
    float y1 = ((float)offset_y / window_data->window_height) * 2.0f - 1.0f;
    float y2 = (((float)offset_y + height) / window_data->window_height) * 2.0f - 1.0f;

    window_data->metal.vertices[0].x = x1;
    window_data->metal.vertices[0].y = y1;

    window_data->metal.vertices[1].x = x1;
    window_data->metal.vertices[1].y = y2;

    window_data->metal.vertices[2].x = x2;
    window_data->metal.vertices[2].y = y1;

    window_data->metal.vertices[3].x = x2;
    window_data->metal.vertices[3].y = y2;

    return true;
}

// ------------------------------------
extern short int g_keycodes[512];

void init_keycodes() {
    // Clear keys
    for (unsigned int i = 0; i < sizeof(g_keycodes) / sizeof(g_keycodes[0]); ++i) g_keycodes[i] = 0;

    g_keycodes[0x1D] = KB_KEY_0;
    g_keycodes[0x12] = KB_KEY_1;
    g_keycodes[0x13] = KB_KEY_2;
    g_keycodes[0x14] = KB_KEY_3;
    g_keycodes[0x15] = KB_KEY_4;
    g_keycodes[0x17] = KB_KEY_5;
    g_keycodes[0x16] = KB_KEY_6;
    g_keycodes[0x1A] = KB_KEY_7;
    g_keycodes[0x1C] = KB_KEY_8;
    g_keycodes[0x19] = KB_KEY_9;
    g_keycodes[0x00] = KB_KEY_A;
    g_keycodes[0x0B] = KB_KEY_B;
    g_keycodes[0x08] = KB_KEY_C;
    g_keycodes[0x02] = KB_KEY_D;
    g_keycodes[0x0E] = KB_KEY_E;
    g_keycodes[0x03] = KB_KEY_F;
    g_keycodes[0x05] = KB_KEY_G;
    g_keycodes[0x04] = KB_KEY_H;
    g_keycodes[0x22] = KB_KEY_I;
    g_keycodes[0x26] = KB_KEY_J;
    g_keycodes[0x28] = KB_KEY_K;
    g_keycodes[0x25] = KB_KEY_L;
    g_keycodes[0x2E] = KB_KEY_M;
    g_keycodes[0x2D] = KB_KEY_N;
    g_keycodes[0x1F] = KB_KEY_O;
    g_keycodes[0x23] = KB_KEY_P;
    g_keycodes[0x0C] = KB_KEY_Q;
    g_keycodes[0x0F] = KB_KEY_R;
    g_keycodes[0x01] = KB_KEY_S;
    g_keycodes[0x11] = KB_KEY_T;
    g_keycodes[0x20] = KB_KEY_U;
    g_keycodes[0x09] = KB_KEY_V;
    g_keycodes[0x0D] = KB_KEY_W;
    g_keycodes[0x07] = KB_KEY_X;
    g_keycodes[0x10] = KB_KEY_Y;
    g_keycodes[0x06] = KB_KEY_Z;

    // g_keycodes[0x27] = KB_KEY_APOSTROPHE;
    // g_keycodes[0x2A] = KB_KEY_BACKSLASH;
    // g_keycodes[0x2B] = KB_KEY_COMMA;
    // g_keycodes[0x18] = KB_KEY_EQUAL;
    // g_keycodes[0x32] = KB_KEY_GRAVE_ACCENT;
    // g_keycodes[0x21] = KB_KEY_LEFT_BRACKET;
    // g_keycodes[0x1B] = KB_KEY_MINUS;
    // g_keycodes[0x2F] = KB_KEY_PERIOD;
    // g_keycodes[0x1E] = KB_KEY_RIGHT_BRACKET;
    // g_keycodes[0x29] = KB_KEY_SEMICOLON;
    // g_keycodes[0x2C] = KB_KEY_SLASH;
    // g_keycodes[0x0A] = KB_KEY_WORLD_1;

    g_keycodes[0x33] = KB_KEY_BACKSPACE;
    // g_keycodes[0x39] = KB_KEY_CAPS_LOCK;
    g_keycodes[0x75] = KB_KEY_DELETE;
    g_keycodes[0x7D] = KB_KEY_DOWN;
    g_keycodes[0x77] = KB_KEY_END;
    g_keycodes[0x24] = KB_KEY_ENTER;
    g_keycodes[0x35] = KB_KEY_ESCAPE;
    g_keycodes[0x7A] = KB_KEY_F1;
    g_keycodes[0x78] = KB_KEY_F2;
    g_keycodes[0x63] = KB_KEY_F3;
    g_keycodes[0x76] = KB_KEY_F4;
    g_keycodes[0x60] = KB_KEY_F5;
    g_keycodes[0x61] = KB_KEY_F6;
    g_keycodes[0x62] = KB_KEY_F7;
    g_keycodes[0x64] = KB_KEY_F8;
    g_keycodes[0x65] = KB_KEY_F9;
    g_keycodes[0x6D] = KB_KEY_F10;
    g_keycodes[0x67] = KB_KEY_F11;
    g_keycodes[0x6F] = KB_KEY_F12;
    g_keycodes[0x69] = KB_KEY_F13;
    g_keycodes[0x6B] = KB_KEY_F14;
    g_keycodes[0x71] = KB_KEY_F15;
    g_keycodes[0x6A] = KB_KEY_F16;
    g_keycodes[0x40] = KB_KEY_F17;
    g_keycodes[0x4F] = KB_KEY_F18;
    g_keycodes[0x50] = KB_KEY_F19;
    g_keycodes[0x5A] = KB_KEY_F20;
    g_keycodes[0x73] = KB_KEY_HOME;
    g_keycodes[0x72] = KB_KEY_INSERT;
    g_keycodes[0x7B] = KB_KEY_LEFT;
    // g_keycodes[0x3A] = KB_KEY_LEFT_ALT;
    g_keycodes[0x3B] = KB_KEY_LEFT_CONTROL;
    g_keycodes[0x38] = KB_KEY_LEFT_SHIFT;
    // g_keycodes[0x37] = KB_KEY_LEFT_SUPER;
    g_keycodes[0x6E] = KB_KEY_MENU;
    g_keycodes[0x47] = KB_KEY_NUM_LOCK;
    // g_keycodes[0x79] = KB_KEY_PAGE_DOWN;
    // g_keycodes[0x74] = KB_KEY_PAGE_UP;
    g_keycodes[0x7C] = KB_KEY_RIGHT;
    // g_keycodes[0x3D] = KB_KEY_RIGHT_ALT;
    g_keycodes[0x3E] = KB_KEY_RIGHT_CONTROL;
    g_keycodes[0x3C] = KB_KEY_RIGHT_SHIFT;
    // g_keycodes[0x36] = KB_KEY_RIGHT_SUPER;
    g_keycodes[0x31] = KB_KEY_SPACE;
    g_keycodes[0x30] = KB_KEY_TAB;
    g_keycodes[0x7E] = KB_KEY_UP;

    g_keycodes[0x52] = KB_KEY_KP_0;
    g_keycodes[0x53] = KB_KEY_KP_1;
    g_keycodes[0x54] = KB_KEY_KP_2;
    g_keycodes[0x55] = KB_KEY_KP_3;
    g_keycodes[0x56] = KB_KEY_KP_4;
    g_keycodes[0x57] = KB_KEY_KP_5;
    g_keycodes[0x58] = KB_KEY_KP_6;
    g_keycodes[0x59] = KB_KEY_KP_7;
    g_keycodes[0x5B] = KB_KEY_KP_8;
    g_keycodes[0x5C] = KB_KEY_KP_9;
    g_keycodes[0x45] = KB_KEY_KP_ADD;
    g_keycodes[0x41] = KB_KEY_KP_DECIMAL;
    g_keycodes[0x4B] = KB_KEY_KP_DIVIDE;
    // g_keycodes[0x4C] = KB_KEY_KP_ENTER;
    // g_keycodes[0x51] = KB_KEY_KP_EQUAL;
    g_keycodes[0x43] = KB_KEY_KP_MULTIPLY;
    g_keycodes[0x4E] = KB_KEY_KP_SUBTRACT;
}

// ------------------------------------
void mfb_get_monitor_scale(struct mfb_opaque_window* window, float* scale_x, float* scale_y) {
    float scale = 1.0f;

    if (window != 0x0) {
        mfb_window_data* window_data = (mfb_window_data*)window;

        scale = [window_data->window backingScaleFactor];
    } else {
        scale = [[NSScreen mainScreen] backingScaleFactor];
    }

    if (scale_x) {
        *scale_x = scale;
        if (*scale_x == 0) {
            *scale_x = 1;
        }
    }

    if (scale_y) {
        *scale_y = scale;
        if (*scale_y == 0) {
            *scale_y = 1;
        }
    }
}
