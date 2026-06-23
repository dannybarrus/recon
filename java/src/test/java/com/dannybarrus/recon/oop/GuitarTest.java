package com.dannybarrus.recon.oop;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class GuitarTest {

    @Test
    void newGuitar_isNotTuned() {
        Guitar guitar = new Guitar("Test Guitar", 6);
        assertFalse(guitar.isTuned());
    }

    @Test
    void performSong_tunesTheGuitar() {
        Guitar guitar = new Guitar("Test Guitar", 6);
        guitar.performSong();
        assertTrue(guitar.isTuned());
    }

    @Test
    void getName_returnsConstructorValue() {
        Guitar guitar = new Guitar("Les Paul", 6);
        assertEquals("Les Paul", guitar.getName());
    }

    @Test
    void supportedFormats_isAStaticInterfaceMethod() {
        // Playable.supportedFormats() is callable directly on the interface.
        // There is no guitar.supportedFormats() at all -- static interface
        // methods are never inherited by implementers.
        assertEquals("MP3, WAV, FLAC", Playable.supportedFormats());
    }
}
