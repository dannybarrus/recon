# Bloom Filter

A space-efficient probabilistic data structure for set membership testing, implemented as a concurrent CLI tool in Go.

## What It Does

A Bloom filter answers one question with two levels of certainty:

- **Definitely not in the set** — 100% accurate, no exceptions
- **Probably in the set** — accurate to a configurable false positive rate

It achieves this using a fixed-size bit array and multiple hash functions. Memory usage is constant regardless of how many items are added. A hash map storing the same data might use 10-100x more memory.

**Real-world uses:** spam filters, web crawlers (have I visited this URL?), database query optimisation (does this row exist before hitting disk?), CDN cache layers.

## The Math in 30 Seconds

When you **add** an item, it runs through *k* hash functions. Each produces a position in the bit array. Those *k* bits are set to 1.

When you **check** an item, the same *k* hash functions produce the same *k* positions. If any bit is 0, the item is **definitely not** in the set. If all bits are 1, the item is **probably** in the set — because another item may have set those same bits.

The optimal bit array size *m* and hash count *k* are derived from the target false positive rate *p* and expected item count *n*:

```
m = -n * ln(p) / (ln(2)^2)
k = (m/n) * ln(2)
```

For a 1% false positive rate with 10,000 items: *m* = 95,851 bits (~12 KB), *k* = 7 hash functions.

## Project Structure

```
bloom\
  cmd\bloom\
    main.go                  CLI entry point
  internal\
    bloom\
      bloom.go               Filter and Hasher interfaces, BloomFilter implementation
      bloom_test.go          Unit tests and benchmarks
    pipeline\
      pipeline.go            Concurrent worker pool (channels and goroutines)
      pipeline_test.go       Pipeline tests
  go.mod
  .gitignore
  .goreleaser.yml            Cross-platform build configuration
  .github\workflows\ci.yml  CI: vet, test with race detector, build, smoke test
  runbloom.bat               Windows launcher
  clean.bat                  Removes build artifacts before checkin
```

## Key Go Concepts Demonstrated

| Concept | Where |
|---|---|
| Interfaces | `Filter` and `Hasher` in `bloom.go` -- pluggable hash strategies |
| Goroutines and channels | `pipeline.go` -- fan-out worker pool, fan-in results channel |
| `sync.RWMutex` | `bloom.go` -- write lock for Add, read lock for Check |
| `sync.WaitGroup` | `pipeline.go` -- coordinates worker shutdown and submission tracking |
| Race detector | `go test -race ./...` -- surfaced and fixed two classes of concurrency bug |

## Concurrency Notes

This project went through two rounds of concurrency debugging, both worth understanding.

### Round 1: Deadlock in CheckPipeline

**Symptom:** The process hung indefinitely with no output when checking large numbers of items.

**Cause:** The results channel buffer (size 1000) filled up while workers tried to send Check results. Workers blocked on send. The jobs channel then filled up because workers were no longer reading from it. `BatchCheck` blocked trying to submit more jobs. Everything stopped.

**Fix:** `BatchCheck` now submits jobs in a background goroutine and returns immediately. `Close` waits for submission to finish before closing the jobs channel. The caller drains `Results()` concurrently with both.

```go
check.BatchCheck(items)   // returns immediately, submits in background
go check.Close()          // runs concurrently -- waits for submission then closes jobs
for r := range check.Results() { ... }  // drains as workers produce results
```

### Round 2: Data Race in BloomFilter.Add

**Symptom:** `go test -race ./...` reported concurrent writes to the same memory address in `bloom.go`.

**Cause:** Multiple `AddPipeline` workers were calling `Add` concurrently. `Add` writes to the `bits` slice without any synchronization. Two goroutines writing to overlapping bit positions at the same time is a data race.

**Fix:** Added `sync.RWMutex` to `BloomFilter`. `Add` acquires a write lock (exclusive). `Check` acquires a read lock (shared -- multiple concurrent reads are fine). Hash computation happens before acquiring any lock since it is pure.

```go
func (bf *BloomFilter) Add(item string) {
    positions := bf.positions(item)   // pure -- no lock needed
    bf.mu.Lock()
    for _, pos := range positions { bf.bits[pos] = true }
    bf.count++
    bf.mu.Unlock()
}
```

**Lesson:** Always run `go test -race ./...` before considering concurrent code correct. The race detector finds bugs that never appear in normal testing because they depend on exact goroutine scheduling timing.

## Running It

```bat
go mod tidy
go test ./...
go run ./cmd/bloom demo --capacity 10000 --fpr 0.01 --trials 50000
```

Or on Windows, double-click `runbloom.bat` which runs tidy, tests, build, and demo in sequence.

### Commands

```bat
bloom.exe demo  --capacity 10000 --fpr 0.01 --trials 50000 --workers 4
bloom.exe add   --items "apple,banana,cherry" --capacity 1000 --fpr 0.01
bloom.exe check --items "apple,mango" --capacity 1000 --fpr 0.01
```

### Example Output

```
Bloom Filter Demo
  Capacity: 10000 items
  Target FPR: 1.00%
  Workers: 4

Loading 10000 items...
items=10000  bits=95851  set=66689  fill=69.6%  fpr=1.0012%  hashes=7

Measuring false positive rate with 50000 non-member checks...

Results:
  False positives:   498 / 50000
  Actual FPR:        0.9960%
  Target FPR:        1.0000%
  Theoretical FPR:   1.0012%
```

## Windows Development Setup

### Required -- always

Download and install Go from https://go.dev/dl/

### Required -- race detector

The Go race detector depends on ThreadSanitizer, which requires a C compiler.
On Windows, install MSYS2 to provide one:

1. Download and install MSYS2 from https://www.msys2.org/ (accept default path `C:\msys64`)
2. Open the MSYS2 terminal and update the core system:
   ```
   pacman -Syuu
   ```
   If the terminal closes, reopen it and run `pacman -Syuu` again.
3. Install the MinGW-w64 toolchain:
   ```
   pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
   ```
4. Add to your PATH: `C:\msys64\ucrt64\bin`
5. Restart your terminal and verify with `gcc --version`

Once installed, run the race detector with:
```bat
go test -race ./...
```

### Required -- release builds

```bat
go install github.com/goreleaser/goreleaser/v2@latest
```

## Creating a Release

Goreleaser handles cross-platform builds and GitHub release creation.

### Test your release configuration locally (no publish)

```bat
goreleaser release --snapshot --clean
```

This builds all platform binaries and places them in `dist\` without pushing anything to GitHub. Use this to verify the build works before tagging.

### Publish a real release

1. Ensure your changes are committed and pushed:
   ```bat
   git add .
   git commit -m "feat: describe your change"
   git push
   ```

2. Tag the release following semantic versioning:
   ```bat
   git tag v1.0.0
   git push origin v1.0.0
   ```

3. Run Goreleaser:
   ```bat
   goreleaser release --clean
   ```
   Goreleaser will build binaries for all platforms, create archives, generate a checksums file, and publish a GitHub release with auto-generated notes from your commit messages.

### Semantic versioning

| Tag | When to use |
|---|---|
| `v1.0.0` | First stable release |
| `v1.0.1` | Bug fix, no new features |
| `v1.1.0` | New feature, backwards compatible |
| `v2.0.0` | Breaking change |

### What Goreleaser produces

```
dist\
  bloom_v1.0.0_linux_amd64.tar.gz
  bloom_v1.0.0_linux_arm64.tar.gz
  bloom_v1.0.0_darwin_amd64.tar.gz
  bloom_v1.0.0_darwin_arm64.tar.gz
  bloom_v1.0.0_windows_amd64.zip
  bloom_v1.0.0_windows_arm64.zip
  checksums.txt
```

End users download the archive for their platform, extract the binary, and run it directly. No Go installation required on their machine.

### Clean before checkin

```bat
clean.bat
```

Removes `bloom.exe`, `dist\`, and any test artifacts. The `.gitignore` also prevents these from being tracked by Git.
