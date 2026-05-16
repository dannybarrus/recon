package pipeline

import (
	"fmt"
	"testing"

	"github.com/dannybarrus/recon/bloom/internal/bloom"
)

func newFilter() bloom.Filter {
	return bloom.New(10000, 0.01, bloom.FNVHasher{})
}

func TestAddThenCheck_AddedItemsAreFound(t *testing.T) {
	fmt.Println(">>> START TestAddThenCheck_AddedItemsAreFound")

	f := newFilter()
	items := make([]string, 100)
	for i := range items {
		items[i] = fmt.Sprintf("item-%d", i)
	}

	fmt.Println("    Adding items...")
	add := NewAddPipeline(f, 4, 200)
	add.BatchAdd(items)
	add.Close()

	fmt.Println("    Checking items...")
	check := NewCheckPipeline(f, 4, 200)
	check.BatchCheck(items)
	check.Close()

	fmt.Println("    Draining results...")
	found := 0
	for r := range check.Results() {
		if r.Found {
			found++
		}
	}

	if found != len(items) {
		t.Errorf("found %d of %d added items, want %d", found, len(items), len(items))
	}

	fmt.Println("<<< END TestAddThenCheck_AddedItemsAreFound")
}

func TestConcurrentWorkers_AllWorkerCounts(t *testing.T) {
	fmt.Println(">>> START TestConcurrentWorkers_AllWorkerCounts")

	for _, workers := range []int{1, 2, 8, 16} {
		t.Run(fmt.Sprintf("workers=%d", workers), func(t *testing.T) {
			fmt.Printf("    >>> START workers=%d\n", workers)

			f := newFilter()
			items := make([]string, 500)
			for i := range items {
				items[i] = fmt.Sprintf("item-%d", i)
			}

			fmt.Printf("    Adding %d items...\n", len(items))
			add := NewAddPipeline(f, workers, 600)
			add.BatchAdd(items)
			add.Close()

			fmt.Printf("    Checking %d items...\n", len(items))
			check := NewCheckPipeline(f, workers, 600)
			check.BatchCheck(items)
			check.Close()

			fmt.Println("    Draining results...")
			found := 0
			for r := range check.Results() {
				if r.Found {
					found++
				}
			}

			if found != len(items) {
				t.Errorf("workers=%d: found %d of %d, want %d", workers, found, len(items), len(items))
			}

			fmt.Printf("    <<< END workers=%d\n", workers)
		})
	}

	fmt.Println("<<< END TestConcurrentWorkers_AllWorkerCounts")
}

func TestCheckPipeline_ResultsChannelClosesAfterDone(t *testing.T) {
	fmt.Println(">>> START TestCheckPipeline_ResultsChannelClosesAfterDone")

	f := newFilter()
	f.Add("test")

	check := NewCheckPipeline(f, 2, 10)
	check.Check("test")
	check.Check("missing")
	check.Close()

	fmt.Println("    Draining results...")
	count := 0
	for range check.Results() {
		count++
	}

	if count != 2 {
		t.Errorf("got %d results, want 2", count)
	}

	fmt.Println("<<< END TestCheckPipeline_ResultsChannelClosesAfterDone")
}
