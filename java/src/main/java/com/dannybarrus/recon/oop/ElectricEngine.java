package com.dannybarrus.recon.oop;

public class ElectricEngine implements Engine {

    private final int batteryCapacityKwh;

    public ElectricEngine(int batteryCapacityKwh) {
        this.batteryCapacityKwh = batteryCapacityKwh;
    }

    @Override
    public void start() {
        System.out.println("Silent startup -- electric engine ready.");
    }

    @Override
    public String describe() {
        return batteryCapacityKwh + "kWh electric engine";
    }
}
