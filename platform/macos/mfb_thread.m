
#import <Foundation/Foundation.h>

#include "platform/modules/px_thread.h"

// ------------------------------------
@interface PXSystemThread : NSObject

@property(strong, nonatomic) NSThread* nsthread;
@property(assign, nonatomic) px_thread* pxthread;

- (instancetype)initWithThread:(px_thread*)thread;
- (void)PX_ThreadRun;
@end

// ------------------------------------
@implementation PXSystemThread

// ------------------
- (instancetype)initWithThread:(px_thread*)thread {
    self = [super init];
    if (self) {
        // Save px_thread reference
        _pxthread = thread;

        // Create and retain NSThread instance
        _nsthread = [[NSThread alloc] initWithTarget:self selector:@selector(PX_ThreadRun) object:nil];
    }
    return self;
}

// ------------------
- (void)PX_ThreadRun {
    _pxthread->isRun = 1;
    _pxthread->pfunc(_pxthread->userptr);
    _pxthread->isRun = 0;
}
@end

// ------------------------------------
int PX_ThreadCreate(px_thread* pthread, func_thread func, void* ptr) {
    pthread->pfunc = func;
    pthread->userptr = ptr;

    PXSystemThread* sysThread = [[PXSystemThread alloc] initWithThread:pthread];
    pthread->handle_ptr = (__bridge_retained void*)sysThread;

    if (pthread->handle_ptr) {
        [sysThread.nsthread start]; // start NSThread
        return 1;
    } else {
        return 0;
    }
}

// ------------------------------------
void PX_ThreadDestroy(px_thread* pthread) {
    if (pthread != NULL && pthread->handle_ptr != NULL) {
        CFRelease(pthread->handle_ptr);
    }
}
