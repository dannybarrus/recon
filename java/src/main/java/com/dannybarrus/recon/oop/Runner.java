package com.dannybarrus.recon.oop;

public interface Runner {

    default String move() {
        return "running";
    }
}
