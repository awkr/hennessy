#include "core/input.h"
#include "core/log.h"
#include "platform.h"

#if defined(PLATFORM_APPLE)

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@class AppDelegate;
@class WindowDelegate;
@class ContentView;

hn::input::Key translate_key_code(u32 key_code);

struct InternalState {
  AppDelegate    *appDelegate = nullptr;
  WindowDelegate *wndDelegate = nullptr;
  NSWindow       *window      = nullptr;
  ContentView    *view        = nullptr;
  CAMetalLayer   *metalLayer  = nullptr;
};

@interface WindowDelegate : NSObject <NSWindowDelegate> {
  hn::platform::State *_state;
}

- (instancetype)initWithState:(hn::platform::State *)state;

@end

@implementation WindowDelegate

- (instancetype)initWithState:(hn::platform::State *)state {
  self = [super init];
  if (self) {
    _state = state;
  }
  return self;
}

- (BOOL)windowShouldClose:(id)sender {
  _state->quit = true;
  return YES;
}

- (void)windowDidResize:(NSNotification *)notification {
}

- (void)windowDidMiniaturize:(NSNotification *)notification {
}

@end

#pragma mark -

@interface ContentView : NSView {
}

@end

@implementation ContentView

+ (Class)layerClass {
  return [CAMetalLayer class];
}

- (void)keyDown:(NSEvent *)event {
  auto key = translate_key_code(event.keyCode);
  hn::input::process_key(key, YES);
}

- (void)keyUp:(NSEvent *)event {
  auto key = translate_key_code(event.keyCode);
  hn::input::process_key(key, NO);
}

- (void)mouseDown:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonLeft, true);
}

- (void)mouseUp:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonLeft, false);
}

- (void)rightMouseDown:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonRight, true);
}

- (void)rightMouseUp:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonRight, false);
}

- (void)otherMouseDown:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonMiddle, true);
}

- (void)otherMouseUp:(NSEvent *)event {
  hn::input::process_button(hn::input::ButtonMiddle, false);
}

- (void)mouseMoved:(NSEvent *)event {
  const auto &loc = event.locationInWindow;
  hn::input::process_mouse_move((f32)loc.x, (f32)loc.y);
}

- (void)scrollWheel:(NSEvent *)event {
  hn::input::process_mouse_wheel((f32)event.scrollingDeltaX, (f32)event.scrollingDeltaY);
}

@end

#pragma mark -

@interface AppDelegate : NSObject <NSApplicationDelegate> {
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
  @autoreleasepool {
    NSEvent *event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:static_cast<NSEventModifierFlags>(0)
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
  } // autoreleasepool
  [NSApp stop:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification {
  HN_debug("will terminate");
}

@end

namespace hn::platform {

bool initialize(State *state, const char *application_name, i32 x, i32 y, u32 width, u32 height) {
  auto s        = new InternalState();
  state->pState = s;

  @autoreleasepool {
    auto appDelegate = [[AppDelegate alloc] init];
    s->appDelegate   = appDelegate;

    auto wndDelegate = [[WindowDelegate alloc] initWithState:state];
    s->wndDelegate   = wndDelegate;

    auto layer    = [CAMetalLayer layer];
    s->metalLayer = layer;

    auto view = [[ContentView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
    [view setLayer:layer];
    [view setWantsLayer:YES];
    s->view = view;

    auto app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];
    [app activateIgnoringOtherApps:YES];
    [app setDelegate:appDelegate];

    auto window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(x, y, width, height)
                  styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                            NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                    backing:NSBackingStoreBuffered
                      defer:NO];
    [window setTitle:@(application_name)];
    [window setLevel:NSNormalWindowLevel];
    [window setContentView:view];
    [window makeFirstResponder:view];
    [window setIsVisible:YES];
    [window setAcceptsMouseMovedEvents:YES];
    [window setRestorable:NO];
    [window setDelegate:wndDelegate];
    [window makeMainWindow];
    [window makeKeyAndOrderFront:nil];

    if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
      [NSApp run];
    }

    return true;
  }
}

void terminate(State *state) {
  @autoreleasepool {
    auto pState = (InternalState *)state->pState;

    [pState->window orderOut:nil];
    [pState->window setDelegate:nil];
    [pState->window close];

    [pState->wndDelegate release];

    [pState->view release];

    [NSApp setDelegate:nil];
    [pState->appDelegate release];

    delete pState;
  }
}

bool poll_events(State *state) {
  @autoreleasepool {
    while (YES) {
      NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantPast]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
      if (!event) {
        break;
      }
      [NSApp sendEvent:event];
    }
    return true;
  }
}

void *allocate(u64 size, bool aligned) { return malloc(size); }

void free(void *block, bool aligned) {
  ::free(block);
  block = nullptr;
}

void *memory_zero(void *block, u64 size) { return memset(block, 0, size); }

void *memory_copy(void *dst, const void *src, u64 size) { return memcpy(dst, src, size); }

void *memory_set(void *dst, i32 value, u64 size) { return memset(dst, value, size); }

f64 get_system_time() { return CACurrentMediaTime(); }

void sleep(u64 ms) {}

} // namespace platform

hn::input::Key translate_key_code(u32 key_code) {
  switch (key_code) {
  case 0: return hn::input::KeyA;
  case 1: return hn::input::KeyS;
  case 2: return hn::input::KeyD;
  case 8: return hn::input::KeyC;
  case 11: return hn::input::KeyB;
  case 13: return hn::input::KeyW;
  case 49: return hn::input::KeySpace;
  case 53: return hn::input::KeyESC;
  case 123: return hn::input::KeyLeft;
  case 124: return hn::input::KeyRight;
  case 125: return hn::input::KeyDown;
  case 126: return hn::input::KeyUp;
  default: return hn::input::KeyMax;
  }
}

#endif
