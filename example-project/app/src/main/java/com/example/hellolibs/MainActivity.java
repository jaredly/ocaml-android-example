package com.example.hellolibs;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.MotionEvent;
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends Activity {
    OCamlBindings bindings = new OCamlBindings();
    Renderer renderer;
    GLSurfaceView glView;

    private static class Renderer implements GLSurfaceView.Renderer {
        OCamlBindings bindings = new OCamlBindings();
        long lastDrawTime = SystemClock.elapsedRealtimeNanos();
        GLSurfaceView view;
        Context context;

        public Renderer(GLSurfaceView view, Context context) {
            super();

            this.view = view;
            this.context = context;
            view.setEGLContextClientVersion(2);
            view.setRenderer(this);
        }

        public void onDrawFrame(GL10 gl) {
            long now = SystemClock.elapsedRealtimeNanos();
            long elapsed = now - lastDrawTime;
            bindings.reasonglUpdate(elapsed / 1000.0);
            lastDrawTime = now;
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            // TODO
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            bindings.reasonglMain(view, new MyAssetManager(context.getAssets(), context.getSharedPreferences("reasongl", MODE_PRIVATE)));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        glView = new GLSurfaceView(this);
        renderer = new Renderer(glView, this);
        setContentView(glView);

        glView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, final MotionEvent event) {
                if (event != null) {
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        glView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                bindings.reasonglTouchPress(event.getX(), event.getY());
                            }
                        });
                    } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
                        glView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                bindings.reasonglTouchDrag(event.getX(), event.getY());
                            }
                        });
                    } else if (event.getAction() == MotionEvent.ACTION_UP) {
                        glView.queueEvent(new Runnable() {
                            @Override
                            public void run() {
                                bindings.reasonglTouchRelease(event.getX(), event.getY());
                            }
                        });
                    }
                    return true;
                } else {
                    return false;
                }
            }
        });
    }
}
