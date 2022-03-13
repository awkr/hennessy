#include "core/log.h"
#include "platform.h"

#if defined(PLATFORM_APPLE)

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

@class AppDelegate;
@class WindowDelegate;
@class ContentView;

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

- (void)keyUp:(NSEvent *)event {
  HN_debug("%d", event.keyCode);
  switch (event.keyCode) {
  case 53: { // ESC
    break;
  }
  }
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

void free(void *block, bool aligned) { ::free(block); }

void *memory_zero(void *block, u64 size) { return memset(block, 0, size); }

void *memory_copy(void *dst, const void *src, u64 size) { return memcpy(dst, src, size); }

void *memory_set(void *dst, i32 value, u64 size) { return memset(dst, value, size); }

f64 get_system_time() { return CACurrentMediaTime(); }

void sleep(u64 ms) {}

} // namespace platform

#endif
