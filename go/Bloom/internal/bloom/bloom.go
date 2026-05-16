// Package bloom implements a probabilistic set membership data structure.
//
// A Bloom filter answers two questions with certainty and probability:
//   - "Definitely not in the set" — 100% accurate
//   - "Probably in the set"       — accurate to a configurable false positive rate
//
// It achieves this using a bit array and multiple hash functions.
// Memory usage is fixed regardless of how many items are added.
//
// Thread safety: BloomFilter is safe for concurrent use. Add uses a write lock
// and Check uses a read lock via sync.RWMutex. Multiple concurrent reads are
// allowed; writes are exclusive.
package bloom

import (
	"fmt"
	"hash/fnv"
	"math"
	"sync"
)

// Filter is the interface for set membership testing.
type Filter interface {
	Add(item string)
	Check(item string) bool
	FalsePositiveRate() float64
	ItemCount() uint
}

// Hasher produces hash values for a given item and seed.
type Hasher interface {
	Hash(item string, seed uint) uint
}

// FNVHasher uses the FNV-1a hash algorithm.
type FNVHasher struct{}

func (h FNVHasher) Hash(item string, seed uint) uint {
	f := fnv.New64a()
	f.Write([]byte(fmt.Sprintf("%d:%s", seed, item)))
	return uint(f.Sum64())
}

// BloomFilter is a space-efficient probabilistic data structure.
// It is safe for concurrent use by multiple goroutines.
type BloomFilter struct {
	bits      []bool
	size      uint
	hashCount uint
	count     uint
	hasher    Hasher
	mu        sync.RWMutex   // protects bits and count
}

// New creates a BloomFilter sized for n expected items at false positive rate p.
func New(n uint, p float64, hasher Hasher) *BloomFilter {
	m := optimalSize(n, p)
	k := optimalHashCount(m, n)
	return &BloomFilter{
		bits:      make([]bool, m),
		size:      m,
		hashCount: k,
		hasher:    hasher,
	}
}

// Add inserts an item into the filter.
// Acquires a write lock -- concurrent Adds and Checks are safe.
func (bf *BloomFilter) Add(item string) {
	positions := bf.positions(item)

	bf.mu.Lock()
	for _, pos := range positions {
		bf.bits[pos] = true
	}
	bf.count++
	bf.mu.Unlock()
}

// Check tests whether an item is in the filter.
// Acquires a read lock -- multiple concurrent Checks are allowed.
//
// Returns false: the item is DEFINITELY NOT in the set.
// Returns true:  the item is PROBABLY in the set.
func (bf *BloomFilter) Check(item string) bool {
	positions := bf.positions(item)

	bf.mu.RLock()
	defer bf.mu.RUnlock()

	for _, pos := range positions {
		if !bf.bits[pos] {
			return false
		}
	}
	return true
}

// positions computes the k bit array positions for an item.
// Called before acquiring any lock -- hash computation is pure and safe.
func (bf *BloomFilter) positions(item string) []uint {
	pos := make([]uint, bf.hashCount)
	for i := uint(0); i < bf.hashCount; i++ {
		pos[i] = bf.hasher.Hash(item, i) % bf.size
	}
	return pos
}

// FalsePositiveRate returns the current estimated false positive probability.
func (bf *BloomFilter) FalsePositiveRate() float64 {
	bf.mu.RLock()
	count := bf.count
	bf.mu.RUnlock()

	if count == 0 {
		return 0
	}
	exponent := -float64(bf.hashCount) * float64(count) / float64(bf.size)
	return math.Pow(1-math.Exp(exponent), float64(bf.hashCount))
}

// ItemCount returns the number of items added.
func (bf *BloomFilter) ItemCount() uint {
	bf.mu.RLock()
	defer bf.mu.RUnlock()
	return bf.count
}

// BitsSet returns the number of bits currently set to true.
func (bf *BloomFilter) BitsSet() uint {
	bf.mu.RLock()
	defer bf.mu.RUnlock()

	count := uint(0)
	for _, b := range bf.bits {
		if b {
			count++
		}
	}
	return count
}

// Stats returns a summary of the filter's current state.
func (bf *BloomFilter) Stats() string {
	return fmt.Sprintf(
		"items=%d  bits=%d  set=%d  fill=%.1f%%  fpr=%.4f%%  hashes=%d",
		bf.ItemCount(),
		bf.size,
		bf.BitsSet(),
		float64(bf.BitsSet())/float64(bf.size)*100,
		bf.FalsePositiveRate()*100,
		bf.hashCount,
	)
}

// ─── Optimal parameter calculations ──────────────────────────────────────────

func optimalSize(n uint, p float64) uint {
	m := -float64(n) * math.Log(p) / (math.Log(2) * math.Log(2))
	return uint(math.Ceil(m))
}

func optimalHashCount(m, n uint) uint {
	k := float64(m) / float64(n) * math.Log(2)
	return uint(math.Max(1, math.Round(k)))
}
