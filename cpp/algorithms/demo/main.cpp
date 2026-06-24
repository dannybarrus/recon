#include <cstdio>
#include <cstring>

#include "recon/algorithms/bitcounting.h"
#include "recon/algorithms/comparestr.h"
#include "recon/algorithms/contigarray.h"
#include "recon/algorithms/custom_strchr.h"
#include "recon/algorithms/expressionmatch.h"
#include "recon/algorithms/fibonacci.h"
#include "recon/algorithms/removeduplicates.h"
#include "recon/algorithms/reversestring.h"
#include "recon/algorithms/reversewords.h"
#include "recon/algorithms/strintintstr.h"

using namespace recon::algorithms;

namespace {

void demoBitcounting() {
    std::printf("--- bitcounting ---\n");
    computeBitcountTables();

    bool allOk = true;
    allOk &= verifyBitcounts(0);
    allOk &= verifyBitcounts(0xFFFFFFFFu);
    allOk &= verifyBitcounts(0xDEADBEEFu);
    for (unsigned int i = 0; i < 10000; i++) {
        allOk &= verifyBitcounts(i * 7919u);  // scatter across the range
    }
    std::printf("All eight bitcount implementations agree: %s\n", allOk ? "yes" : "NO -- mismatch found");
    std::printf("iteratedBitcount(0xDEADBEEF) = %d\n\n", iteratedBitcount(0xDEADBEEFu));
}

void demoFibonacci() {
    std::printf("--- fibonacci ---\n");
    for (int i = 0; i <= 10; i++) {
        std::printf("  fib(%d) = naive:%d  iterative:%d\n", i, fibonacciNaive(i), fibonacciIterative(i));
    }
    std::printf("\n");
}

void demoReverseString() {
    std::printf("--- reversestring ---\n");
    char buf[] = "This is a test";
    reverseString(buf);
    std::printf("  reversed: \"%s\"\n", buf);

    char dangerous[] = "100%% off";
    reverseString(dangerous);
    std::printf("  reversed (had a literal %%%% in it): \"%s\"\n\n", dangerous);
}

void demoReverseWords() {
    std::printf("--- reversewords ---\n");
    char buf[] = "This is a test";
    std::printf("  before: \"%s\"\n", buf);
    reverseWords(buf);
    std::printf("  after:  \"%s\"   (expected: \"test a is This\")\n\n", buf);
}

void demoCustomStrchr() {
    std::printf("--- custom_strchr ---\n");
    const char* s = "abc";
    const char* r = customStrchr(s, 'a');
    std::printf("  customStrchr(\"abc\", 'a') = %s   (expected: \"abc\" -- match AT index 0, the bug this fixes)\n",
        r ? r : "nullptr (BUG)");

    const char* s2 = "This is a test";
    const char* r2 = customStrchr(s2, 't');
    std::printf("  customStrchr(\"This is a test\", 't') = \"%s\"\n\n", r2 ? r2 : "nullptr");
}

void demoRemoveDuplicates() {
    std::printf("--- removeduplicates ---\n");
    int arr[] = {1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 8, 9, 0};
    int size = static_cast<int>(sizeof(arr) / sizeof(arr[0]));

    int newLength = removeDuplicates(arr, size);
    std::printf("  de-duplicated (%d elements): [", newLength);
    for (int i = 0; i < newLength; i++) {
        std::printf("%d%s", arr[i], (i < newLength - 1) ? ", " : "");
    }
    std::printf("]\n\n");
}

void demoContigArray() {
    std::printf("--- contigarray ---\n");
    int array1[] = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8};
    int array2[] = {3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -7, -8};
    int array3[] = {1, 2, 3, 4, 5, -2, -3, -4, -5, -6, -7, -8};

    std::printf("  Ascending array:              %d\n", countContiguousElements(array1, 12));
    std::printf("  Descending array:             %d\n", countContiguousElements(array2, 12));
    std::printf("  Ascending then descending:    %d   (5 ascending, 7 descending -- takes the longer one)\n\n",
        countContiguousElements(array3, 12));
}

void demoCompareStrings() {
    std::printf("--- comparestr ---\n");
    int r1 = compareStrings("Test1 this does not match", "Test1 tnope it sure does not");
    int r2 = compareStrings("Test1", "Test1");
    std::printf("  compareStrings(mismatched strings) = %d   (nonzero)\n", r1);
    std::printf("  compareStrings(\"Test1\", \"Test1\")  = %d   (expected: 0)\n\n", r2);
}

void demoExpressionMatch() {
    std::printf("--- expressionmatch ---\n");
    const char* text = "This is a test string";

    struct Case { const char* pattern; bool expected; };
    Case cases[] = {
        {"T*test*string", true},
        {"*test*", true},
        {"*", true},
        {"*This is *", true},
        {"A*string", false},
        {"This is a test string", true},
        {"This is a test", false},
        {"This is***", true},
        {"This*ZZZNOTPRESENT", false},
    };

    for (const auto& c : cases) {
        bool result = matchesPattern(text, c.pattern);
        std::printf("  \"%s\" vs \"%-20s\" -> %-5s (expected %-5s) %s\n",
            text, c.pattern, result ? "true" : "false", c.expected ? "true" : "false",
            (result == c.expected) ? "OK" : "*** MISMATCH ***");
    }
    std::printf("\n");
}

void demoStrIntIntStr() {
    std::printf("--- strintintstr ---\n");
    int values[] = {14352, 0, -42, 1, -1};
    for (int v : values) {
        char buf[64];
        intToStr(v, buf);
        int roundTrip = strToInt(buf);
        std::printf("  %d -> \"%s\" -> %d   %s\n", v, buf, roundTrip, (roundTrip == v) ? "OK" : "*** MISMATCH ***");
    }
    std::printf("\n");
}

}  // namespace

int main() {
    demoBitcounting();
    demoFibonacci();
    demoReverseString();
    demoReverseWords();
    demoCustomStrchr();
    demoRemoveDuplicates();
    demoContigArray();
    demoCompareStrings();
    demoExpressionMatch();
    demoStrIntIntStr();
    return 0;
}
