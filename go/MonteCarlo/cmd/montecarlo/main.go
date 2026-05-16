// montecarlo approximates mathematical constants using concurrent random sampling.
//
// Usage:
//
//	montecarlo run  --sim pi --trials 10000000 --workers 8
//	montecarlo all  --trials 5000000
//	montecarlo convergence --sim pi --max 10000000 --steps 10
//
// The convergence command shows how accuracy improves as trial count grows.
// This is the most visually impressive output -- watch the estimate home in
// on the true value as the number of random samples increases.
package main

import (
	"flag"
	"fmt"
	"math"
	"os"

	"github.com/dannybarrus/recon/montecarlo/internal/pipeline"
	"github.com/dannybarrus/recon/montecarlo/internal/simulation"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("usage: montecarlo <run|all|convergence> [flags]")
		os.Exit(1)
	}

	switch os.Args[1] {
	case "run":
		runSingle(os.Args[2:])
	case "all":
		runAll(os.Args[2:])
	case "convergence":
		runConvergence(os.Args[2:])
	default:
		fmt.Fprintf(os.Stderr, "unknown command: %s\n", os.Args[1])
		os.Exit(1)
	}
}

func runSingle(args []string) {
	fs      := flag.NewFlagSet("run", flag.ExitOnError)
	simName := fs.String("sim", "pi", "simulation to run: pi, euler, sqrt2")
	trials  := fs.Int("trials", 10_000_000, "number of random trials")
	workers := fs.Int("workers", 0, "goroutines to use (0 = NumCPU)")
	fs.Parse(args)

	sim, err := getSim(*simName)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	engine := pipeline.New(*workers)
	result := engine.Run(sim, *trials)
	fmt.Println(result)
}

func runAll(args []string) {
	fs      := flag.NewFlagSet("all", flag.ExitOnError)
	trials  := fs.Int("trials", 5_000_000, "trials per simulation")
	workers := fs.Int("workers", 0, "goroutines (0 = NumCPU)")
	fs.Parse(args)

	engine := pipeline.New(*workers)
	sims   := []simulation.Simulation{
		simulation.PiSimulation{},
		simulation.EulerSimulation{},
		simulation.SqrtTwoSimulation{},
	}

	fmt.Printf("Monte Carlo Simulation Engine\n")
	fmt.Printf("Trials per simulation: %d   Workers: %d\n\n", *trials, engine.Workers())
	fmt.Printf("%-25s  %-16s  %-16s  %-10s  %-10s  %s\n",
		"Simulation", "Estimate", "True Value", "Abs Error", "Error %", "")
	fmt.Println(repeat("-", 100))

	for _, sim := range sims {
		result := engine.Run(sim, *trials)
		bar    := accuracyBar(result.ErrorPct)
		fmt.Printf("%-25s  %-16.8f  %-16.8f  %-10.6f  %-9.4f%%  %s\n",
			result.SimulationName,
			result.Estimate,
			result.TrueValue,
			result.Error,
			result.ErrorPct,
			bar,
		)
	}
}

func runConvergence(args []string) {
	fs      := flag.NewFlagSet("convergence", flag.ExitOnError)
	simName := fs.String("sim", "pi", "simulation: pi, euler, sqrt2")
	max     := fs.Int("max", 10_000_000, "maximum trial count")
	steps   := fs.Int("steps", 10, "number of steps to show")
	workers := fs.Int("workers", 0, "goroutines (0 = NumCPU)")
	fs.Parse(args)

	sim, err := getSim(*simName)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	engine := pipeline.New(*workers)

	fmt.Printf("Convergence: %s  (true value = %.8f)\n\n", sim.Name(), sim.TrueValue())
	fmt.Printf("%-12s  %-16s  %-10s  %s\n", "Trials", "Estimate", "Error %", "Accuracy")
	fmt.Println(repeat("-", 70))

	for i := 1; i <= *steps; i++ {
		trials := *max / *steps * i
		result := engine.Run(sim, trials)
		bar    := accuracyBar(result.ErrorPct)
		fmt.Printf("%-12d  %-16.8f  %-9.4f%%  %s\n",
			trials, result.Estimate, result.ErrorPct, bar)
	}
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

func getSim(name string) (simulation.Simulation, error) {
	switch name {
	case "pi":
		return simulation.PiSimulation{}, nil
	case "euler":
		return simulation.EulerSimulation{}, nil
	case "sqrt2":
		return simulation.SqrtTwoSimulation{}, nil
	default:
		return nil, fmt.Errorf("unknown simulation %q — choose: pi, euler, sqrt2", name)
	}
}

// accuracyBar renders a simple visual indicator of accuracy.
// The fewer characters, the closer to the true value.
func accuracyBar(errorPct float64) string {
	stars := int(math.Max(0, 5-math.Log10(errorPct+0.0001)*2))
	if stars > 10 { stars = 10 }
	return repeat("*", stars) + repeat(".", 10-stars)
}

func repeat(s string, n int) string {
	result := ""
	for i := 0; i < n; i++ {
		result += s
	}
	return result
}
