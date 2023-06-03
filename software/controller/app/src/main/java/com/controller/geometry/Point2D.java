package com.controller.geometry;

public class Point2D<E> {
    private E x;
    private E y;

    public Point2D(E x, E y) {
        this.x = x;
        this.y = y;
    }

    public E getX() {
        return x;
    }

    public E getY() {
        return y;
    }
}
