// Package pipeline implements the concurrent Monte Carlo engine.
//
// Architecture:
//
//	Dispatcher splits n trials across w workers via a jobs channel.
//	Each worker runs its share of trials independently with its own RNG.
//	Workers send partial hit counts back via a results channel.
//	The collector sums all partial results to produce the final estimate.
//
//	jobs channel    -->  [worker 1]  --> results channel
//	                -->  [worker 2]  -->
//	                -->  [worker w]  -->
//	                          collector sums results
//
// Using a separate RNG per worker avoids lock contention on a shared source.
// This is the standard Go pattern for concurrent random number generation.
package pipeline

import (
	"math"
	"math/rand"
	"runtime"
	"time"

	"github.com/dannybarrus/recon/montecarlo/internal/simulation"
)

// job carries a share of work to one goroutine.
type job struct {
	trials int
	seed   int64
}

// partialResult carries one worker's contribution back to the collector.
type partialResult struct {
	hits   int
	trials int
}

// Engine runs Monte Carlo simulations concurrently across a worker pool.
type Engine struct {
	workers int
}

// New creates an Engine. If workers <= 0, uses runtime.NumCPU().
func New(workers int) *Engine {
	if workers <= 0 {
		workers = runtime.NumCPU()
	}
	return &Engine{workers: workers}
}

// Run executes the given simulation for totalTrials trials across the worker pool.
// Returns a Result with the estimate and accuracy metrics.
func (e *Engine) Run(sim simulation.Simulation, totalTrials int) simulation.Result {
	jobs    := make(chan job, e.workers)
	results := make(chan partialResult, e.workers)

	// Launch workers -- each reads one job, runs trials, sends partial result
	for i := 0; i < e.workers; i++ {
		go func() {
			for j := range jobs {
				rng  := rand.New(rand.NewSource(j.seed))
				hits := sim.Run(j.trials, rng)
				results <- partialResult{hits: hits, trials: j.trials}
			}
		}()
	}

	// Dispatch: split trials as evenly as possible across workers
	go func() {
		base      := totalTrials / e.workers
		remainder := totalTrials % e.workers
		for i := 0; i < e.workers; i++ {
			t := base
			if i < remainder {
				t++   // distribute the leftover trials one at a time
			}
			jobs <- job{trials: t, seed: time.Now().UnixNano() + int64(i)}
		}
		close(jobs)
	}()

	// Collect: sum all partial results
	totalHits   := 0
	totalActual := 0
	for i := 0; i < e.workers; i++ {
		r := <-results
		totalHits   += r.hits
		totalActual += r.trials
	}
	close(results)

	estimate  := sim.Estimate(totalHits, totalActual)
	trueVal   := sim.TrueValue()
	absError  := math.Abs(estimate - trueVal)
	errPct    := absError / trueVal * 100

	return simulation.Result{
		SimulationName: sim.Name(),
		Trials:         totalActual,
		Workers:        e.workers,
		Estimate:       estimate,
		TrueValue:      trueVal,
		Error:          absError,
		ErrorPct:       errPct,
	}
}

// Workers returns the number of goroutines in the pool.
func (e *Engine) Workers() int { return e.workers }
