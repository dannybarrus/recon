// Package pipeline provides a concurrent worker pool for batch Bloom filter operations.
//
// Add and Check are separated into two distinct pipelines to prevent deadlock.
//
// AddPipeline:   submit items -> workers add to filter -> no output
// CheckPipeline: submit items -> workers check filter  -> results channel
//
// IMPORTANT: With CheckPipeline, always drain Results() concurrently with
// submission. BatchCheck submits jobs in a background goroutine so the caller
// can drain the results channel at the same time. Without concurrent draining,
// the results channel fills up, workers block, the jobs channel fills up,
// and everything deadlocks.
package pipeline

import (
	"sync"

	"github.com/dannybarrus/recon/bloom/internal/bloom"
)

// Result is the outcome of a Check operation.
type Result struct {
	Item  string
	Found bool
}

// ─── AddPipeline ─────────────────────────────────────────────────────────────

// AddPipeline distributes Add operations across a worker pool.
// No results are produced. Call Close() to wait for all workers to finish.
type AddPipeline struct {
	filter bloom.Filter
	jobs   chan string
	wg     sync.WaitGroup
}

// NewAddPipeline creates an AddPipeline with workerCount goroutines.
func NewAddPipeline(filter bloom.Filter, workerCount, bufferSize int) *AddPipeline {
	p := &AddPipeline{
		filter: filter,
		jobs:   make(chan string, bufferSize),
	}
	for i := 0; i < workerCount; i++ {
		p.wg.Add(1)
		go func() {
			defer p.wg.Done()
			for item := range p.jobs {
				p.filter.Add(item)
			}
		}()
	}
	return p
}

// Add submits one item for insertion.
func (p *AddPipeline) Add(item string) { p.jobs <- item }

// BatchAdd submits a slice of items for insertion.
func (p *AddPipeline) BatchAdd(items []string) {
	for _, item := range items {
		p.jobs <- item
	}
}

// Close signals no more items will be added and waits for all workers to finish.
func (p *AddPipeline) Close() {
	close(p.jobs)
	p.wg.Wait()
}

// ─── CheckPipeline ────────────────────────────────────────────────────────────

// CheckPipeline distributes Check operations across a worker pool.
//
// BatchCheck submits jobs in a background goroutine and returns immediately.
// This allows the caller to drain Results() concurrently with submission,
// which is required to prevent deadlock when checking large numbers of items.
//
// Usage:
//
//	check := NewCheckPipeline(filter, 4, 100)
//	check.BatchCheck(items)   // returns immediately, submits in background
//	check.Close()             // waits for submission to complete, closes jobs
//	for r := range check.Results() { ... }   // drain after close
type CheckPipeline struct {
	filter      bloom.Filter
	jobs        chan string
	results     chan Result
	workerWg    sync.WaitGroup
	submitterWg sync.WaitGroup
}

// NewCheckPipeline creates a CheckPipeline with workerCount goroutines.
func NewCheckPipeline(filter bloom.Filter, workerCount, bufferSize int) *CheckPipeline {
	p := &CheckPipeline{
		filter:  filter,
		jobs:    make(chan string, bufferSize),
		results: make(chan Result, bufferSize),
	}

	// Start workers
	for i := 0; i < workerCount; i++ {
		p.workerWg.Add(1)
		go func() {
			defer p.workerWg.Done()
			for item := range p.jobs {
				p.results <- Result{
					Item:  item,
					Found: p.filter.Check(item),
				}
			}
		}()
	}

	// Close results channel when all workers are done
	go func() {
		p.workerWg.Wait()
		close(p.results)
	}()

	return p
}

// Check submits one item for membership testing.
func (p *CheckPipeline) Check(item string) { p.jobs <- item }

// BatchCheck submits items in a background goroutine and returns immediately.
// The caller must drain Results() concurrently -- do not wait for BatchCheck
// to finish before draining, or the results channel will fill and deadlock.
func (p *CheckPipeline) BatchCheck(items []string) {
	p.submitterWg.Add(1)
	go func() {
		defer p.submitterWg.Done()
		for _, item := range items {
			p.jobs <- item
		}
	}()
}

// Close waits for all submitted items to be dispatched, then closes the jobs
// channel. Workers will finish in-flight jobs and exit. Drain Results() after
// calling Close to collect all remaining results.
func (p *CheckPipeline) Close() {
	p.submitterWg.Wait() // wait for all BatchCheck goroutines to finish submitting
	close(p.jobs)        // signal workers no more jobs are coming
}

// Results returns the channel that receives Check results.
// This channel is closed automatically when all workers are done.
func (p *CheckPipeline) Results() <-chan Result { return p.results }
