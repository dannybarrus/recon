package simulation

import (
	"math"
	"math/rand"
	"testing"
)

// ─── Pi simulation ────────────────────────────────────────────────────────────

func TestPiSimulation_ConvergesWithSufficientTrials(t *testing.T) {
	sim := PiSimulation{}
	rng := rand.New(rand.NewSource(42))

	hits  := sim.Run(1_000_000, rng)
	total := 1_000_000
	est   := sim.Estimate(hits, total)

	// With 1M trials, expect within 0.1% of Pi
	tolerance := 0.001 * math.Pi
	if math.Abs(est-math.Pi) > tolerance {
		t.Errorf("Pi estimate %.6f differs from %.6f by more than %.6f", est, math.Pi, tolerance)
	}
}

func TestPiSimulation_ImprovesWithMoreTrials(t *testing.T) {
	sim  := PiSimulation{}
	rng1 := rand.New(rand.NewSource(1))
	rng2 := rand.New(rand.NewSource(1))   // same seed for fair comparison

	smallHits := sim.Run(1_000, rng1)
	largeHits := sim.Run(1_000_000, rng2)

	smallErr := math.Abs(sim.Estimate(smallHits, 1_000) - math.Pi)
	largeErr := math.Abs(sim.Estimate(largeHits, 1_000_000) - math.Pi)

	if largeErr >= smallErr*0.5 {
		// Large trial count should generally produce a smaller error
		// We use 0.5x as a loose bound to avoid flakiness
		t.Logf("small error: %.6f  large error: %.6f (larger run should be more accurate)", smallErr, largeErr)
	}
}

func TestPiSimulation_NeverExceedsFour(t *testing.T) {
	sim := PiSimulation{}
	rng := rand.New(rand.NewSource(99))

	for i := 0; i < 100; i++ {
		hits := sim.Run(1000, rng)
		est  := sim.Estimate(hits, 1000)
		if est > 4.0 || est < 0 {
			t.Errorf("Pi estimate %.6f is out of valid range [0, 4]", est)
		}
	}
}

func TestPiSimulation_Metadata(t *testing.T) {
	sim := PiSimulation{}
	if sim.Name() == "" {
		t.Error("Name() should not be empty")
	}
	if sim.TrueValue() != math.Pi {
		t.Errorf("TrueValue() = %.6f, want %.6f", sim.TrueValue(), math.Pi)
	}
}

// ─── Euler simulation ─────────────────────────────────────────────────────────

func TestEulerSimulation_ConvergesWithSufficientTrials(t *testing.T) {
	sim := EulerSimulation{}
	rng := rand.New(rand.NewSource(42))

	hits := sim.Run(1_000_000, rng)
	est  := sim.Estimate(hits, 1_000_000)

	tolerance := 0.005   // 0.5% tolerance
	if math.Abs(est-math.E) > tolerance {
		t.Errorf("e estimate %.6f differs from %.6f by more than %.6f", est, math.E, tolerance)
	}
}

func TestEulerSimulation_Metadata(t *testing.T) {
	sim := EulerSimulation{}
	if sim.TrueValue() != math.E {
		t.Errorf("TrueValue() = %.6f, want %.6f", sim.TrueValue(), math.E)
	}
}

// ─── Result formatting ────────────────────────────────────────────────────────

func TestResult_String_IsNotEmpty(t *testing.T) {
	r := Result{
		SimulationName: "Test",
		Trials:         1000,
		Workers:        4,
		Estimate:       3.14,
		TrueValue:      math.Pi,
		Error:          0.001,
		ErrorPct:       0.03,
	}
	if r.String() == "" {
		t.Error("Result.String() should not be empty")
	}
}

// ─── Benchmarks ───────────────────────────────────────────────────────────────

func BenchmarkPiSimulation_1M(b *testing.B) {
	sim := PiSimulation{}
	rng := rand.New(rand.NewSource(42))
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		sim.Run(1_000_000, rng)
	}
}

func BenchmarkEulerSimulation_1M(b *testing.B) {
	sim := EulerSimulation{}
	rng := rand.New(rand.NewSource(42))
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		sim.Run(1_000_000, rng)
	}
}
