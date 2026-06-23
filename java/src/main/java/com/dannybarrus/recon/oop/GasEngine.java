package com.dannybarrus.recon.oop;

public class GasEngine implements Engine {

    private final int horsepower;

    public GasEngine(int horsepower) {
        this.horsepower = horsepower;
    }

    @Override
    public void start() {
        System.out.println("Vroom -- gas engine ignited.");
    }

    @Override
    public String describe() {
        return horsepower + "hp gasoline engine";
    }
}
