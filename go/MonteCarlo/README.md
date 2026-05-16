# Monte Carlo Simulation Engine

A concurrent engine that approximates mathematical constants using random sampling, built in Go with goroutines and channels.

## What It Does in 30 Seconds

Throw a million darts at random at a square. Count how many land inside the circle drawn inside it. The ratio of hits to total throws converges to Pi/4. Multiply by 4 and you have Pi — approximated purely through randomness.

That is Monte Carlo simulation. The more trials, the closer the answer. Distribute the trials across goroutines and the wall time drops proportionally to the number of CPU cores.

This engine applies the same approach to three mathematical constants:

| Simulation | Method | True Value |
|---|---|---|
| Pi | Quarter-circle dart throwing | 3.14159265... |
| Euler's number (e) | Draw random values until sum exceeds 1; count the draws | 2.71828182... |
| sqrt(2) | Geometric sampling of unit square hypotenuse | 1.41421356... |

## Why Monte Carlo?

Named after the Monte Carlo Casino in Monaco. The method was developed in the 1940s by physicists Stanislaw Ulam and John von Neumann while working on the Manhattan Project. Ulam was trying to calculate the probability of a solitaire card game succeeding and realized random sampling could solve problems too complex for exact calculation. They needed a code name for the classified work and chose Monte Carlo — a nod to Ulam's uncle, who would borrow money from relatives to gamble there.

## Project Structure

```
montecarlo\
  cmd\montecarlo\
    main.go                    CLI entry point
  internal\
    simulation\
      simulation.go            Simulation interface and three implementations
      simulation_test.go       Unit tests and benchmarks per simulation
    pipeline\
      engine.go                Concurrent engine -- channels, goroutines, fan-out/fan-in
      engine_test.go           Engine tests including accuracy and concurrency
  go.mod
  .gitignore
  .goreleaser.yml              Cross-platform build configuration
  .github\workflows\ci.yml    CI: vet, race detector, build, smoke test
  runmontecarlo.bat            Windows launcher
  clean.bat                    Removes build artifacts before checkin
```

## Key Go Concepts Demonstrated

| Concept | Where |
|---|---|
| Interfaces | `Simulation` in `simulation.go` -- any type with `Run`, `Estimate`, `Name`, `TrueValue` works |
| Goroutines | `engine.go` -- one goroutine per worker, plus dispatcher and collector |
| Channels | `engine.go` -- jobs channel (fan-out), results channel (fan-in) |
| Per-goroutine RNG | `engine.go` -- each worker gets its own seeded `rand.New` to avoid lock contention |
| `runtime.NumCPU` | `engine.go` -- defaults to the number of available cores |
| Benchmarks | `simulation_test.go` -- `BenchmarkPiSimulation_1M` and `BenchmarkEulerSimulation_1M` |

## The Concurrency Model

```
Dispatcher goroutine
  splits n trials across w workers
  sends job batches to jobs channel
  closes jobs channel when done

Worker goroutines (w of them)
  each reads one job from jobs channel
  creates its own rand.New(rand.NewSource(seed))  -- no shared state, no locks
  runs simulation.Run(trials, rng)
  sends partial result to results channel

Main goroutine
  reads w results from results channel
  sums hits and total trials
  calls simulation.Estimate(totalHits, totalTrials)
```

Using a separate RNG per worker eliminates mutex contention on a shared random source. This is the standard Go pattern for concurrent random number generation.

## Running It

```bat
go mod tidy
go test ./...
go run ./cmd/montecarlo all --trials 1000000
```

Or on Windows, double-click `runmontecarlo.bat` which runs tidy, tests, build, and all simulations in sequence.

### Commands

```bat
montecarlo.exe run         --sim pi --trials 10000000 --workers 8
montecarlo.exe all         --trials 5000000
montecarlo.exe convergence --sim pi --max 10000000 --steps 10
```

### Example Output -- Convergence

```
Convergence: Pi (quarter-circle)  (true value = 3.14159265)

Trials         Estimate          Error %    Accuracy
----------------------------------------------------------------------
1000000        3.14071200        0.0281%    **........
2000000        3.14128800        0.0097%    ***......
3000000        3.14138933        0.0065%    ***......
4000000        3.14148500        0.0034%    ****......
5000000        3.14152640        0.0021%    ****......
10000000       3.14159225        0.0000%    **********
```

## Adding a New Simulation

Implement the `Simulation` interface in `internal\simulation\`:

```go
type MySimulation struct{}

func (s MySimulation) Name() string        { return "My Simulation" }
func (s MySimulation) TrueValue() float64  { return 1.23456 }

func (s MySimulation) Run(n int, rng *rand.Rand) int {
    hits := 0
    for i := 0; i < n; i++ {
        if someCondition { hits++ }
    }
    return hits
}

func (s MySimulation) Estimate(hits, total int) float64 {
    return float64(hits) / float64(total) * someScalar
}
```

Register it in `cmd\montecarlo\main.go` and it runs with full concurrency support automatically.

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

Builds all platform binaries into `dist\` without pushing to GitHub. Use this to verify the build before tagging.

### Publish a real release

1. Commit and push your changes:
   ```bat
   git add .
   git commit -m "feat: describe your change"
   git push
   ```

2. Tag the release:
   ```bat
   git tag v1.0.0
   git push origin v1.0.0
   ```

3. Run Goreleaser:
   ```bat
   goreleaser release --clean
   ```

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
  montecarlo_v1.0.0_linux_amd64.tar.gz
  montecarlo_v1.0.0_linux_arm64.tar.gz
  montecarlo_v1.0.0_darwin_amd64.tar.gz
  montecarlo_v1.0.0_darwin_arm64.tar.gz
  montecarlo_v1.0.0_windows_amd64.zip
  montecarlo_v1.0.0_windows_arm64.zip
  checksums.txt
```

End users download the archive for their platform and run the binary directly. No Go installation required.

### Clean before checkin

```bat
clean.bat
```

Removes `montecarlo.exe`, `dist\`, and test artifacts. The `.gitignore` also prevents these from being tracked by Git.
