package com.controller.geometry;

import android.opengl.GLES20;

import com.controller.utils.MyGLRenderer;
import com.controller.utils.RingBuffer;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class Line2D {
    private final String vertexShaderCode =
            "uniform mat4 uMVPMatrix;" +
                    "attribute vec4 vPosition;" +
                    "void main() {" +
                    "  gl_Position = uMVPMatrix * vPosition;" +
                    "}";

    private final String fragmentShaderCode =
            "precision mediump float;" +
                    "uniform vec4 vColor;" +
                    "void main() {" +
                    "  gl_FragColor = vColor;" +
                    "}";

    private static final int COORDS_PER_VERTEX = 3;

    private Point2D<Float> p1;
    private Point2D<Float> p2;
    private float[] color;
    private float width;

    private final int mProgram;

    private Object lock = new Object( );

    public Line2D(Point2D<Float> p1, Point2D<Float> p2, float[] color, float width) {
        this.p1 = p1;
        this.p2 = p2;

        this.color = color;
        this.width = width;

        int vertexShader = MyGLRenderer.loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = MyGLRenderer.loadShader( GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode );

        // create empty OpenGL ES Program
        mProgram = GLES20.glCreateProgram();

        // add the vertex shader to program
        GLES20.glAttachShader(mProgram, vertexShader);

        // add the fragment shader to program
        GLES20.glAttachShader(mProgram, fragmentShader);

        // creates OpenGL ES program executables
        GLES20.glLinkProgram(mProgram);
    }

    public void draw(float[] mvpMatrix) {
        // Add program to OpenGL ES environment
        GLES20.glUseProgram(mProgram);

        // get handle to vertex shader's vPosition member
        int mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");

        // Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);

        int coordsSize = 0;

        FloatBuffer vertexBuffer;

        synchronized ( lock ) {
            coordsSize = 2;
            // initialize vertex byte buffer for shape coordinates (number of coordinate values * 4 bytes per float)
            ByteBuffer buffer = ByteBuffer.allocateDirect( coordsSize * COORDS_PER_VERTEX * 4 );
            // use the device hardware's native byte order
            buffer.order(ByteOrder.nativeOrder());

            // create a floating point buffer from the ByteBuffer
            vertexBuffer = buffer.asFloatBuffer();
            // add the coordinates to the FloatBuffer

            vertexBuffer.put(p1.getX());
            vertexBuffer.put(p1.getY());
            vertexBuffer.put(0.0f);
            vertexBuffer.put(p2.getX());
            vertexBuffer.put(p2.getY());
            vertexBuffer.put(0.0f);

        }

        // set the buffer to read the first coordinate
        vertexBuffer.position(0);

        // Prepare the triangle coordinate data
        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX,
                GLES20.GL_FLOAT, false,
                COORDS_PER_VERTEX * 4, vertexBuffer);

        // get handle to fragment shader's vColor member
        int mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");

        // Set color for drawing the triangle
        GLES20.glUniform4fv(mColorHandle, 1, color, 0);

        // get handle to shape's transformation matrix
        int mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");

        // Pass the projection and view transformation to the shader
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);

        GLES20.glLineWidth(width);
        // Draw the triangle
        GLES20.glDrawArrays(GLES20.GL_LINE_STRIP, 0, coordsSize);

        // Disable vertex array
        GLES20.glDisableVertexAttribArray(mPositionHandle);
    }
}
