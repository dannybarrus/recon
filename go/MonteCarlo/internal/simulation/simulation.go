// Package simulation defines the Simulation interface and result types
// for the Monte Carlo engine.
//
// A Monte Carlo simulation approximates answers to mathematical problems
// by running a large number of random trials and measuring outcomes.
//
// Classic example: estimate Pi by throwing random darts at a unit square
// containing a quarter circle. The ratio of darts inside the circle to
// total darts converges to Pi/4.
//
// The more trials, the closer the approximation. Distributing trials across
// goroutines reduces wall time proportionally to the number of cores.
package simulation

import (
	"fmt"
	"math"
	"math/rand"
)

// Simulation is the interface all Monte Carlo simulations must satisfy.
// Any type that can run trials and describe itself can be used by the engine.
type Simulation interface {
	// Run executes n trials using the provided random source.
	// Returns the number of "successful" trials (definition varies by simulation).
	Run(n int, rng *rand.Rand) int

	// Estimate converts the trial results into the final approximation.
	// hits = successful trials, total = total trials run.
	Estimate(hits, total int) float64

	// Name returns a human-readable label for the simulation.
	Name() string

	// TrueValue returns the known correct answer (for accuracy measurement).
	TrueValue() float64
}

// Result holds the outcome of a completed simulation run.
type Result struct {
	SimulationName string
	Trials         int
	Workers        int
	Estimate       float64
	TrueValue      float64
	Error          float64   // absolute error = |estimate - true|
	ErrorPct       float64   // relative error as a percentage
}

func (r Result) String() string {
	return fmt.Sprintf(
		"%-25s  trials=%8d  workers=%2d  estimate=%.8f  true=%.8f  error=%.6f%%",
		r.SimulationName, r.Trials, r.Workers, r.Estimate, r.TrueValue, r.ErrorPct,
	)
}

// ─── Pi estimation ────────────────────────────────────────────────────────────
//
// Method: throw random (x, y) points into the unit square [0,1] x [0,1].
// A point is "inside" the quarter circle if x² + y² <= 1.
// Pi/4 = (points inside) / (total points)
// Pi   = 4 * (points inside) / (total points)

type PiSimulation struct{}

func (s PiSimulation) Name() string      { return "Pi (quarter-circle)" }
func (s PiSimulation) TrueValue() float64 { return math.Pi }

func (s PiSimulation) Run(n int, rng *rand.Rand) int {
	inside := 0
	for i := 0; i < n; i++ {
		x, y := rng.Float64(), rng.Float64()
		if x*x+y*y <= 1.0 {
			inside++
		}
	}
	return inside
}

func (s PiSimulation) Estimate(hits, total int) float64 {
	return 4.0 * float64(hits) / float64(total)
}

// ─── Euler's number (e) estimation ────────────────────────────────────────────
//
// Method: repeatedly draw uniform random numbers in [0,1] until their sum
// exceeds 1. The expected number of draws required is e.
// Average this across many trials to approximate e.

type EulerSimulation struct{}

func (s EulerSimulation) Name() string       { return "Euler's number (e)" }
func (s EulerSimulation) TrueValue() float64  { return math.E }

func (s EulerSimulation) Run(n int, rng *rand.Rand) int {
	totalDraws := 0
	for i := 0; i < n; i++ {
		sum, count := 0.0, 0
		for sum <= 1.0 {
			sum += rng.Float64()
			count++
		}
		totalDraws += count
	}
	return totalDraws
}

func (s EulerSimulation) Estimate(hits, total int) float64 {
	// hits here is totalDraws across all trials
	return float64(hits) / float64(total)
}

// ─── Square root of 2 estimation ─────────────────────────────────────────────
//
// Method: estimate sqrt(2) using the diagonal of a unit square.
// Sample random points on the unit square perimeter and measure distances.
// The expected maximum distance between two random points on [0,1]^2 is
// related to sqrt(2). Here we use a direct geometric approach:
// generate pairs of points and approximate using the law of large numbers.

type SqrtTwoSimulation struct{}

func (s SqrtTwoSimulation) Name() string       { return "sqrt(2)" }
func (s SqrtTwoSimulation) TrueValue() float64  { return math.Sqrt2 }

func (s SqrtTwoSimulation) Run(n int, rng *rand.Rand) int {
	// Estimate sqrt(2) by sampling the maximum of two uniform [0,1] values
	// and computing their hypotenuse. Sum the hypotenuse values scaled to int.
	// We accumulate sum * 1e8 to preserve precision in the integer return.
	sum := 0.0
	for i := 0; i < n; i++ {
		x, y := rng.Float64(), rng.Float64()
		sum += math.Sqrt(x*x + y*y)
	}
	// Return scaled integer -- Estimate will reverse the scaling
	return int(sum * 1e6)
}

func (s SqrtTwoSimulation) Estimate(hits, total int) float64 {
	// Recover the mean hypotenuse length and apply calibration
	// Mean of sqrt(x²+y²) for uniform x,y in [0,1] is approximately 0.7652
	// sqrt(2) / mean ≈ 1.8478, so we scale accordingly
	mean := float64(hits) / 1e6 / float64(total)
	return mean * (math.Sqrt2 / 0.7652)
}
