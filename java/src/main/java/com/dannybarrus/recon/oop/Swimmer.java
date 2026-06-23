package com.dannybarrus.recon.oop;

public interface Swimmer {

    default String move() {
        return "swimming";
    }
}
