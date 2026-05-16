package bloom

import (
	"fmt"
	"math"
	"testing"
)

func newTestFilter(n uint, p float64) *BloomFilter {
	return New(n, p, FNVHasher{})
}

// ─── Core behaviour ───────────────────────────────────────────────────────────

func TestAddAndCheck_AddedItemsAreFound(t *testing.T) {
	f := newTestFilter(1000, 0.01)
	items := []string{"apple", "banana", "cherry", "date", "elderberry"}

	for _, item := range items {
		f.Add(item)
	}

	for _, item := range items {
		if !f.Check(item) {
			t.Errorf("Check(%q) = false, want true (item was added)", item)
		}
	}
}

func TestCheck_UnaddedItemsReturnFalse(t *testing.T) {
	f := newTestFilter(1000, 0.01)
	f.Add("known-item")

	// With a 1% FPR and 1 item in a 1000-item filter, the vast majority
	// of random strings should return false.
	falsePositives := 0
	trials := 1000
	for i := 0; i < trials; i++ {
		candidate := fmt.Sprintf("unknown-item-%d", i)
		if f.Check(candidate) {
			falsePositives++
		}
	}

	// False positive rate should be well under 5% even accounting for variance
	fpr := float64(falsePositives) / float64(trials)
	if fpr > 0.05 {
		t.Errorf("false positive rate %.4f exceeds 5%% threshold", fpr)
	}
}

// A Bloom filter must NEVER produce false negatives.
// If an item was added, Check must always return true.
func TestNoFalseNegatives(t *testing.T) {
	f := newTestFilter(500, 0.01)
	added := make([]string, 500)
	for i := 0; i < 500; i++ {
		item := fmt.Sprintf("item-%d", i)
		added[i] = item
		f.Add(item)
	}

	for _, item := range added {
		if !f.Check(item) {
			t.Errorf("false negative: %q was added but Check returned false", item)
		}
	}
}

// ─── False positive rate ──────────────────────────────────────────────────────

func TestFalsePositiveRate_StartsAtZero(t *testing.T) {
	f := newTestFilter(1000, 0.01)
	if f.FalsePositiveRate() != 0 {
		t.Errorf("FalsePositiveRate() = %f, want 0 for empty filter", f.FalsePositiveRate())
	}
}

func TestFalsePositiveRate_IncreasesWithLoad(t *testing.T) {
	f := newTestFilter(100, 0.01)
	prev := f.FalsePositiveRate()

	for i := 0; i < 100; i++ {
		f.Add(fmt.Sprintf("item-%d", i))
		curr := f.FalsePositiveRate()
		if curr < prev {
			t.Errorf("FalsePositiveRate decreased after adding item %d: %.6f < %.6f", i, curr, prev)
		}
		prev = curr
	}
}

func TestFalsePositiveRate_ApproximatesTargetAtCapacity(t *testing.T) {
	target := 0.01   // 1% target FPR
	n := uint(10000)
	f := newTestFilter(n, target)

	for i := uint(0); i < n; i++ {
		f.Add(fmt.Sprintf("item-%d", i))
	}

	actual := f.FalsePositiveRate()
	// Allow 2x tolerance -- actual FPR should be in the right ballpark
	if actual > target*2 {
		t.Errorf("FalsePositiveRate() = %.6f, want <= %.6f (2x target)", actual, target*2)
	}
}

// ─── Optimal parameters ───────────────────────────────────────────────────────

func TestOptimalSize_ProducesPositiveValue(t *testing.T) {
	size := optimalSize(1000, 0.01)
	if size == 0 {
		t.Error("optimalSize() = 0, want > 0")
	}
}

func TestOptimalHashCount_AtLeastOne(t *testing.T) {
	k := optimalHashCount(9586, 1000)
	if k < 1 {
		t.Errorf("optimalHashCount() = %d, want >= 1", k)
	}
}

func TestOptimalHashCount_CloseTo7ForOnePercent(t *testing.T) {
	// For p=0.01, k should be approximately 7
	// This is a well-known result from Bloom filter theory
	m := optimalSize(1000, 0.01)
	k := optimalHashCount(m, 1000)

	if math.Abs(float64(k)-7) > 2 {
		t.Errorf("optimalHashCount() = %d, want ~7 for 1%% FPR", k)
	}
}

// ─── Item count ───────────────────────────────────────────────────────────────

func TestItemCount_TracksAdditions(t *testing.T) {
	f := newTestFilter(100, 0.01)
	for i := 0; i < 50; i++ {
		f.Add(fmt.Sprintf("item-%d", i))
	}
	if f.ItemCount() != 50 {
		t.Errorf("ItemCount() = %d, want 50", f.ItemCount())
	}
}

// ─── Hasher interface ─────────────────────────────────────────────────────────

func TestFNVHasher_DifferentSeedsDifferentHashes(t *testing.T) {
	h := FNVHasher{}
	h1 := h.Hash("test", 0)
	h2 := h.Hash("test", 1)
	h3 := h.Hash("test", 2)

	if h1 == h2 || h2 == h3 || h1 == h3 {
		t.Error("different seeds should produce different hashes for the same input")
	}
}

func TestFNVHasher_SameSeedSameHash(t *testing.T) {
	h := FNVHasher{}
	if h.Hash("deterministic", 0) != h.Hash("deterministic", 0) {
		t.Error("same input and seed should always produce the same hash")
	}
}

// ─── Benchmark ────────────────────────────────────────────────────────────────

func BenchmarkAdd(b *testing.B) {
	f := newTestFilter(uint(b.N), 0.01)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		f.Add(fmt.Sprintf("item-%d", i))
	}
}

func BenchmarkCheck(b *testing.B) {
	f := newTestFilter(10000, 0.01)
	for i := 0; i < 10000; i++ {
		f.Add(fmt.Sprintf("item-%d", i))
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		f.Check(fmt.Sprintf("item-%d", i%10000))
	}
}
