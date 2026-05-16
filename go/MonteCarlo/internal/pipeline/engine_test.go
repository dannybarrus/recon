package pipeline

import (
	"math"
	"testing"

	"github.com/dannybarrus/recon/montecarlo/internal/simulation"
)

func TestEngine_PiEstimateWithinTolerance(t *testing.T) {
	engine := New(4)
	result := engine.Run(simulation.PiSimulation{}, 1_000_000)

	tolerance := 0.01   // 1% of Pi
	if result.ErrorPct > tolerance*100 {
		t.Errorf("Pi error %.4f%% exceeds %.4f%% tolerance", result.ErrorPct, tolerance*100)
	}
}

func TestEngine_EulerEstimateWithinTolerance(t *testing.T) {
	engine := New(4)
	result := engine.Run(simulation.EulerSimulation{}, 1_000_000)

	if result.ErrorPct > 1.0 {   // within 1%
		t.Errorf("e error %.4f%% exceeds 1%% tolerance", result.ErrorPct)
	}
}

func TestEngine_TrialCountIsCorrect(t *testing.T) {
	engine := New(4)
	result := engine.Run(simulation.PiSimulation{}, 100_000)

	if result.Trials != 100_000 {
		t.Errorf("Trials = %d, want 100000", result.Trials)
	}
}

func TestEngine_WorkerCountIsRecorded(t *testing.T) {
	engine := New(8)
	result := engine.Run(simulation.PiSimulation{}, 10_000)

	if result.Workers != 8 {
		t.Errorf("Workers = %d, want 8", result.Workers)
	}
}

func TestEngine_DefaultsToNumCPU(t *testing.T) {
	engine := New(0)
	if engine.Workers() <= 0 {
		t.Errorf("Workers() = %d, want > 0", engine.Workers())
	}
}

func TestEngine_MoreTrialsMeansLowerError(t *testing.T) {
	engine  := New(4)
	small   := engine.Run(simulation.PiSimulation{}, 1_000)
	large   := engine.Run(simulation.PiSimulation{}, 1_000_000)

	// Large should generally be more accurate -- use generous bound for test stability
	if large.ErrorPct > small.ErrorPct*1.5 {
		t.Logf("small error: %.4f%%  large error: %.4f%%", small.ErrorPct, large.ErrorPct)
		t.Errorf("more trials should produce lower or comparable error")
	}
}

func TestEngine_ResultFieldsPopulated(t *testing.T) {
	engine := New(2)
	result := engine.Run(simulation.PiSimulation{}, 10_000)

	if result.SimulationName == "" { t.Error("SimulationName should not be empty") }
	if result.Estimate == 0        { t.Error("Estimate should not be zero") }
	if result.TrueValue == 0       { t.Error("TrueValue should not be zero") }
	if result.Error < 0            { t.Error("Error should not be negative") }
	if math.IsNaN(result.ErrorPct) { t.Error("ErrorPct should not be NaN") }
}
