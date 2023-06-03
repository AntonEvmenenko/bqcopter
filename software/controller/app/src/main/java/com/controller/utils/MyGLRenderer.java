package com.controller.utils;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import com.controller.geometry.Line2D;
import com.controller.geometry.LineStrip2D;
import com.controller.geometry.Point2D;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

public class MyGLRenderer implements GLSurfaceView.Renderer {
    public static final float[] COLOR_RED = { 1.0f, 0.0f, 0.0f, 0.5f };
    public static final float[] COLOR_GREEN = { 0.0f, 1.0f, 0.0f, 0.5f };
    public static final float[] COLOR_BLUE = { 0.0f, 0.0f, 1.0f, 0.5f };
    public static final float[] COLOR_BLACK = { 0.0f, 0.0f, 0.0f, 0.5f };
    public static final float[] COLOR_GRAY = { 0.87f, 0.87f, 0.87f, 0.5f };

    private final float[ ] mMVPMatrix = new float[16];
    private final float[ ] mProjectionMatrix = new float[16];
    private final float[ ] mViewMatrix = new float[16];

    public LineStrip2D roll;
    public LineStrip2D pitch;
    public LineStrip2D yaw;

    private Line2D[] grid;

    private float t = 0;

    @Override
    public void onSurfaceCreated( GL10 unused, EGLConfig config ) {
        // Set the background frame color
        GLES20.glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

        roll = new LineStrip2D();
        pitch = new LineStrip2D();
        yaw = new LineStrip2D();

        grid = new Line2D[ 30 ];
        for (int i = 0; i < grid.length; i++) {
            grid[i] = new Line2D(new Point2D<Float>(-3.0f, (15 - i) / 7.0f),
                    new Point2D<Float>(3.0f, (15 - i) / 7.0f), COLOR_GRAY, i == 15 ? 9.0f : 1.0f);
        }

    }

    @Override
    public void onDrawFrame( GL10 unused ) {
        // Redraw background color
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        Matrix.setLookAtM(mViewMatrix, 0, 0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);

        // Calculate the projection and view transformation
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mViewMatrix, 0);

        for (int i = 0; i < grid.length; i++) {
            grid[i].draw(mMVPMatrix);
        }

        roll.draw(mMVPMatrix, COLOR_RED);
        pitch.draw(mMVPMatrix, COLOR_GREEN);
        yaw.draw(mMVPMatrix, COLOR_BLUE);
    }

    @Override
    public void onSurfaceChanged( GL10 unused, int width, int height ) {
        GLES20.glViewport(0, 0, width, height);

        float ratio = (float) width / height;

        // this projection matrix is applied to object coordinates
        // in the onDrawFrame() method
        Matrix.frustumM(mProjectionMatrix, 0, -ratio, ratio, -1, 1, 3, 7);
    }

    public static int loadShader( int type, String shaderCode ){

        // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
        // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader( type );

        // add the source code to the shader and compile it
        GLES20.glShaderSource( shader, shaderCode );
        GLES20.glCompileShader(shader);

        return shader;
    }
}