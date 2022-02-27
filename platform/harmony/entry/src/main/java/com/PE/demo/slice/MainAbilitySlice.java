package com.PE.demo.slice;

import ohos.aafwk.ability.AbilitySlice;
import ohos.aafwk.content.Intent;
import ohos.agp.components.Component;
import ohos.agp.components.DependentLayout;
import ohos.agp.components.PositionLayout;
import ohos.agp.components.Text;
import ohos.agp.components.surfaceprovider.SurfaceProvider;
import ohos.agp.graphics.SurfaceOps;
import ohos.agp.render.Canvas;
import ohos.agp.render.opengl.*;
import ohos.agp.utils.Color;
import ohos.agp.colors.RgbColor;
import ohos.agp.components.element.ShapeElement;
import ohos.agp.components.ComponentContainer.LayoutConfig;
import ohos.agp.utils.Point;
import ohos.hiviewdfx.HiLog;
import ohos.hiviewdfx.HiLogLabel;
import ohos.multimodalinput.event.MmiPoint;
import ohos.multimodalinput.event.TouchEvent;

import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class MainAbilitySlice extends AbilitySlice {
    // Load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("GlNative");
    }
    private PositionLayout myLayout = new PositionLayout(this);
    SurfaceCallBack surfaceCallBack=new SurfaceCallBack();

    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        LayoutConfig config = new LayoutConfig(LayoutConfig.MATCH_PARENT, LayoutConfig.MATCH_PARENT);
        myLayout.setLayoutConfig(config);
        ShapeElement element = new ShapeElement();
        element.setShape(ShapeElement.RECTANGLE);
        element.setRgbColor(new RgbColor(255, 255, 255));
        myLayout.setBackground(element);
        Text text = new Text(this);
        text.setTextColor(Color.BLACK);
        myLayout.addComponent(text);
        super.setUIContent(myLayout);

        onCreate();
        addSurfaceProvider();
        startDraw();

    }

    private void addSurfaceProvider() {
        SurfaceProvider surfaceProvider = new SurfaceProvider(this);
        surfaceProvider.pinToZTop(true);
        if (surfaceProvider.getSurfaceOps().isPresent()) {
            surfaceProvider.getSurfaceOps().get().addCallback(surfaceCallBack);
        }
        surfaceProvider.setTouchEventListener(new Component.TouchEventListener() {
            @Override
            public boolean onTouchEvent(Component component, TouchEvent touchEvent) {
                switch (touchEvent.getAction()) {
                    case TouchEvent.PRIMARY_POINT_DOWN: {
                        //获取点信息
                        MmiPoint point = touchEvent.getPointerPosition(touchEvent.getIndex());
                        onMouseDown((int)point.getX(),(int)point.getY());
                        //PRIMARY_POINT_DOWN 一定要返回true
                        return true;
                    }
                    case TouchEvent.PRIMARY_POINT_UP:

                        break;
                    case TouchEvent.POINT_MOVE: {
                        MmiPoint point = touchEvent.getPointerPosition(touchEvent.getIndex());
                        onMouseMove((int)point.getX(),(int)point.getY());
                        break;
                    }

                }

                return false;
            }
        });
        myLayout.addComponent(surfaceProvider);
    }

    private class SurfaceCallBack implements SurfaceOps.Callback {
        private final HiLogLabel LOG_TAG = new HiLogLabel(HiLog.LOG_APP, 0xD001400, "SCallBacks");
        private EGLDisplay eglDisplay;
        private EGLSurface eglSurface;

        private EGLContext eglContext = null;
        private EGLConfig eglConfig = null;

        private int disWidth = 0;
        private int disHeight = 0;

        @Override
        public void surfaceCreated(SurfaceOps callbackSurfaceOps) {
            eglDisplay = EGL.eglGetDisplay(EGL.EGL_DEFAULT_DISPLAY);
            if (eglDisplay == EGL.EGL_NO_DISPLAY) {
                return;
            }
            int[] version = new int[2];
            if (!EGL.eglInitialize(eglDisplay, version, version)) {
                return;
            }

            int alphaSize = 3;
            int depthSize = 3;
            int stencilSize = 3;
            int renderType = 0x0004;
            int[] attributes = new int[] {
                    EGL.EGL_RED_SIZE, 3,
                    EGL.EGL_GREEN_SIZE, 3,
                    EGL.EGL_ALPHA_SIZE, alphaSize,
                    EGL.EGL_DEPTH_SIZE, depthSize,
                    EGL.EGL_STENCIL_SIZE, stencilSize,

                    EGL.EGL_RENDERABLE_TYPE, renderType,
                    EGL.EGL_NONE
            };
            int[] configNum = new int[1];
            EGLConfig[] configs = new EGLConfig[1];
            if(!EGL.eglChooseConfig(eglDisplay, attributes, configs, 1, configNum)){
                return;
            }
            if (eglConfig == null) {
                eglConfig = configs[0];
            }
            int[] contextAttr = new int[] {
                    0x3098, 2, EGL.EGL_NONE,
            };
            eglContext = EGL.eglCreateContext(eglDisplay, eglConfig, EGL.EGL_NO_CONTEXT, contextAttr);
            if (eglContext == EGL.EGL_NO_CONTEXT) {
                return;
            }

            String openVersion = GLES20.glGetString(GLES20.GL_VERSION);
            HiLog.info(LOG_TAG, "OpenVersion: " + openVersion);

        }

        @Override
        public void surfaceChanged(SurfaceOps callbackSurfaceOps, int format, int width, int height) {
            int[] contextAttr = new int[] {
                    EGL.EGL_NONE
            };
            eglSurface = EGL.eglCreateWindowSurface(eglDisplay, eglConfig, callbackSurfaceOps.getSurface(),contextAttr);

            if (eglSurface == EGL.EGL_NO_SURFACE) {
                return;
            }
            if (!EGL.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
                return;
            }
            float ratio = (float) width/height;


            disWidth = width;
            disHeight = height;
            onDrawFrame();
        }

        @Override
        public void surfaceDestroyed(SurfaceOps callbackSurfaceOps) {
            EGL.eglMakeCurrent(eglDisplay, null, null, null);
            EGL.eglDestroySurface(eglDisplay, eglSurface);
            EGL.eglDestroyContext(eglDisplay, eglContext);
            EGL.eglTerminate(eglDisplay);
        }

        public void onDrawFrame() {

            GLES20.glEnable(GLES20.GL_DEPTH_TEST);
            //设置显示范围
            GLES20.glViewport(0,0, disWidth, disHeight);
            //设置背景，清除颜色
            GLES20.glClearColor(0.8f,0.8f,0.8f, 1.0f);
            //执行背景清除
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            onDraw();

            //更新缓冲区显示到屏幕
            if(!EGL.eglSwapBuffers(eglDisplay, eglSurface)) {
                return;
            }
        }

    }
    @Override
    public void onActive() {
        super.onActive();
    }

    @Override
    public void onForeground(Intent intent) {
        super.onForeground(intent);
    }

    public void startDraw() {
        Runnable requestRunnable = () -> surfaceCallBack.onDrawFrame();
        ThreadPoolExecutor threadPoolExecutor = new ThreadPoolExecutor(1, 1, 0L, TimeUnit.MILLISECONDS, new LinkedBlockingQueue<>());
        threadPoolExecutor.execute(() -> {
            while (true) {
                getUITaskDispatcher().asyncDispatch(requestRunnable);
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    public native void onCreate();
    public native void onDraw();
    public native void onMouseDown(int x,int y);
    public native void onMouseMove(int x,int y);
}
