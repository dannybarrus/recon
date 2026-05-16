package main

import (
	"flag"
	"fmt"
	"os"
	"strings"

	"github.com/dannybarrus/recon/bloom/internal/bloom"
	"github.com/dannybarrus/recon/bloom/internal/pipeline"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("usage: bloom <add|check|demo> [flags]")
		os.Exit(1)
	}

	switch os.Args[1] {
	case "add":
		runAdd(os.Args[2:])
	case "check":
		runCheck(os.Args[2:])
	case "demo":
		runDemo(os.Args[2:])
	default:
		fmt.Fprintf(os.Stderr, "unknown command: %s\n", os.Args[1])
		os.Exit(1)
	}
}

func runAdd(args []string) {
	fs       := flag.NewFlagSet("add", flag.ExitOnError)
	items    := fs.String("items", "", "comma-separated items to add")
	capacity := fs.Uint("capacity", 10000, "expected number of items")
	fpr      := fs.Float64("fpr", 0.01, "target false positive rate (0.0-1.0)")
	workers  := fs.Int("workers", 4, "number of concurrent workers")
	fs.Parse(args)

	if *items == "" {
		fmt.Fprintln(os.Stderr, "error: --items is required")
		os.Exit(1)
	}

	f    := bloom.New(uint(*capacity), *fpr, bloom.FNVHasher{})
	list := strings.Split(*items, ",")

	add := pipeline.NewAddPipeline(f, *workers, len(list)+10)
	add.BatchAdd(list)
	add.Close()

	fmt.Printf("Added %d items\n", len(list))
	fmt.Println(f.Stats())
}

func runCheck(args []string) {
	fs       := flag.NewFlagSet("check", flag.ExitOnError)
	items    := fs.String("items", "", "comma-separated items to check")
	capacity := fs.Uint("capacity", 10000, "expected number of items")
	fpr      := fs.Float64("fpr", 0.01, "target false positive rate")
	workers  := fs.Int("workers", 4, "number of concurrent workers")
	fs.Parse(args)

	if *items == "" {
		fmt.Fprintln(os.Stderr, "error: --items is required")
		os.Exit(1)
	}

	f    := bloom.New(uint(*capacity), *fpr, bloom.FNVHasher{})
	list := strings.Split(*items, ",")

	check := pipeline.NewCheckPipeline(f, *workers, len(list)+10)
	check.BatchCheck(list)
	check.Close()

	for r := range check.Results() {
		status := "NOT FOUND"
		if r.Found {
			status = "PROBABLY FOUND"
		}
		fmt.Printf("  %-30s %s\n", r.Item, status)
	}
}

func runDemo(args []string) {
	fs       := flag.NewFlagSet("demo", flag.ExitOnError)
	capacity := fs.Uint("capacity", 10000, "number of items to load")
	fpr      := fs.Float64("fpr", 0.01, "target false positive rate")
	trials   := fs.Int("trials", 50000, "number of non-member checks for FPR measurement")
	workers  := fs.Int("workers", 4, "concurrent workers")
	fs.Parse(args)

	fmt.Printf("Bloom Filter Demo\n")
	fmt.Printf("  Capacity: %d items\n", *capacity)
	fmt.Printf("  Target FPR: %.2f%%\n", *fpr*100)
	fmt.Printf("  Workers: %d\n\n", *workers)

	f := bloom.New(uint(*capacity), *fpr, bloom.FNVHasher{})

	// Phase 1: load the filter
	fmt.Printf("Loading %d items...\n", *capacity)
	members := make([]string, *capacity)
	for i := uint(0); i < *capacity; i++ {
		members[i] = fmt.Sprintf("member-item-%d", i)
	}

	add := pipeline.NewAddPipeline(f, *workers, 1000)
	add.BatchAdd(members)
	add.Close()

	fmt.Println(f.Stats())

	// Phase 2: measure actual false positive rate
	// BatchCheck submits in background -- drain Results() concurrently
	fmt.Printf("\nMeasuring false positive rate with %d non-member checks...\n", *trials)

	nonMembers := make([]string, *trials)
	for i := 0; i < *trials; i++ {
		nonMembers[i] = fmt.Sprintf("non-member-%d", i)
	}

	check := pipeline.NewCheckPipeline(f, *workers, 1000)
	check.BatchCheck(nonMembers)   // submits in background goroutine

	// Close and drain must happen concurrently with each other.
	// Close waits for submission then closes jobs channel.
	// Drain reads results as workers produce them.
	// Neither can happen fully before the other -- run close in a goroutine.
	go check.Close()

	falsePositives := 0
	for r := range check.Results() {
		if r.Found {
			falsePositives++
		}
	}

	actualFPR := float64(falsePositives) / float64(*trials)
	fmt.Printf("\nResults:\n")
	fmt.Printf("  False positives:   %d / %d\n", falsePositives, *trials)
	fmt.Printf("  Actual FPR:        %.4f%%\n", actualFPR*100)
	fmt.Printf("  Target FPR:        %.4f%%\n", *fpr*100)
	fmt.Printf("  Theoretical FPR:   %.4f%%\n", f.FalsePositiveRate()*100)
}
