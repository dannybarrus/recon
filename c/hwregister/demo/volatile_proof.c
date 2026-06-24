// Compiled TWICE by CMakeLists.txt -- once with USE_VOLATILE defined,
// once without -- and BOTH are forced to a real optimization level
// (-O2 / clang's equivalent / MSVC's /O2) regardless of the overall
// project's build type. This is deliberately a real, runnable artifact
// rather than just a comment claiming what "should" happen: build and
// run both executables yourself, on your own compiler, and see what
// actually happens on your machine.
//
// A hard iteration cap means this can NEVER truly hang forever even if
// the non-volatile version reproduces the bug -- it reports a timeout
// and exits instead.

#include <stdint.h>
#include <stdio.h>

#include "recon/recon_thread.h"

#ifdef USE_VOLATILE
static volatile uint8_t flag = 0;
#else
static uint8_t flag = 0;
#endif

static void* setter_thread(void* arg) {
    (void)arg;
    recon_thread_sleep_ms(200);
    flag = 1;
    return NULL;
}

int main(void) {
#ifdef USE_VOLATILE
    printf("volatile_proof: built WITH volatile\n");
#else
    printf("volatile_proof: built WITHOUT volatile\n");
#endif
#ifdef _MSC_VER
    printf("  (on MSVC, this only shows the real divergence in Release configuration --\n");
    printf("   Debug builds are unoptimized by default and both variants will appear to\n");
    printf("   work fine, which is expected, not a sign the bug doesn't exist on MSVC)\n");
#endif

    recon_thread_t thread;
    recon_thread_create(&thread, setter_thread, NULL);

    long long iterations = 0;
    // long long, not plain long: plain `long` is only 32 bits on
    // Windows x64 (LLP64), unlike Linux/macOS where it's 64. This
    // literal (3 billion) overflows a 32-bit signed long, silently
    // wrapping to a negative value -- which made the loop condition
    // below false on its very first check, on every build, regardless
    // of optimization level, which is exactly the "0 iterations,
    // immediately reports timed out, identically in Debug and
    // Release" behavior this bug actually produced when first built
    // on MSVC. long long is guaranteed at least 64 bits everywhere.
    const long long kSafetyCap = 3000000000LL;

    while (flag == 0 && iterations < kSafetyCap) {
        iterations++;
    }

    if (flag == 1) {
        printf("  observed the flag change after %lld iterations -- correctly detected the update\n",
               iterations);
    } else {
        printf("  TIMED OUT after %lld iterations -- never observed the update\n", iterations);
        printf("  (this is the actual bug volatile exists to prevent, reproduced live)\n");
    }

    recon_thread_join(thread);
    return 0;
}
