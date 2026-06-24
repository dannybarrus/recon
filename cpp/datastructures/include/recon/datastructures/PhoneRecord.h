#pragma once

#include <string>

namespace recon::datastructures {

// A simple value type -- name and phone number.
//
// The original (in both doublylinkedlist.cpp and lnklstreverse.cpp) used
// manually malloc'd char* fields for name and number, each sized exactly
// to the input length via strlen()+1. lnklstreverse.cpp's GetPhone()
// then called:
//
//   strcpy_s(pPhone->pName, strlen(pPhone->pName), Buffer);
//
// passing strlen(pPhone->pName) as the destination BUFFER SIZE -- but
// pPhone->pName was JUST malloc'd and never initialized at that point.
// Calling strlen() on uninitialized memory is undefined behavior: the
// "size" strcpy_s received was effectively garbage, not the real
// allocated size.
//
// Using std::string here doesn't patch that bug -- it removes the whole
// category of bug. There is no manual allocation, no strlen-of-garbage,
// no size mismatch to get wrong.
struct PhoneRecord {
    std::string name;
    std::string number;
};

}  // namespace recon::datastructures
