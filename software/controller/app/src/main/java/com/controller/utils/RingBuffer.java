package com.controller.utils;

public class RingBuffer<E> {
    private static final int MAX_SIZE = 10000;

    private E data[];
    private int size = 0;
    private int realSize = 0;
    private int startIndex = 0;
    private int endIndex = 0;

    public RingBuffer( int size ) {
        assert size <= MAX_SIZE;
        this.size = size;
        data = (E[])new Object[MAX_SIZE];
    }

    public void add(E newElement) {
        data[endIndex] = newElement;
        endIndex = (endIndex + 1) % size;
        if (realSize == size) {
            startIndex = (startIndex + 1) % size;
        } else {
            realSize++;
        }
    }

    public E get(int index) {
        return data[(startIndex + index) % size];
    }

    public int getSize() {
        return realSize;
    }

    public void clear() {
        realSize = 0;
        startIndex = 0;
        endIndex = 0;
    }
}
