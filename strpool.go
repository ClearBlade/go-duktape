package duktape

/*
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"math"
	"sort"
	"strings"
	"unsafe"
)

var MaxMallocBytes = 10_000_000 // 10MB default, can be overwritten in importing package

func (d *Context) GetStringPtr(src string) *StringPointer {
	return d.strPool.GetStringPointer(src)
}

func (d *Context) FreeStringPtr(sp *StringPointer) {
	d.strPool.FreeStringPointer(sp)
}

func (d *Context) StrPoolStats() string {
	return d.strPool.Stats()
}

func (d *Context) StrPoolDump() string {
	return d.strPool.String()
}

type strPool struct {
	pool       []*StringPointer
	allocated  int
	statAllocs int
	statReuses int
	statFrees  int
}

type StringPointer struct {
	p     unsafe.Pointer
	cap   int
	inuse bool
}

func (sp *StringPointer) CString() *C.char {
	return (*C.char)(sp.p)
}

func (sp *StringPointer) String() string {
	return fmt.Sprintf("SP(cap: %d;inuse: %t)", sp.cap, sp.inuse)
}

func NewStrPool() *strPool {
	return &strPool{
		pool: []*StringPointer{},
	}
}

func (s *strPool) String() string {
	pointers := make([]string, len(s.pool))
	for i, sp := range s.pool {
		pointers[i] = sp.String()
	}
	return fmt.Sprintf("POOL{size: %d; items: %s}", s.allocated, strings.Join(pointers, ", "))
}

func (s *strPool) Stats() string {
	return fmt.Sprintf("allocs: %d; reuses: %d; frees: %d", s.statAllocs, s.statReuses, s.statFrees)
}

func (s *strPool) get(cap int) *StringPointer {
	for i := 0; i < len(s.pool); i++ {
		if !s.pool[i].inuse && s.pool[i].cap >= cap {
			s.pool[i].inuse = true
			s.statReuses++
			return s.pool[i]
		}
	}
	// give them mem with cap that's the next power of 2
	normalizedCap := 2 << int(math.Log2(float64(cap)))
	ret := &StringPointer{
		p:     C.malloc(C.ulong(normalizedCap)),
		cap:   normalizedCap,
		inuse: true,
	}
	s.statAllocs++
	s.pool = append(s.pool, ret)
	s.allocated += normalizedCap
	sort.Slice(s.pool, s.lessThan) // sort pool so new alloc requests get best fit
	if s.allocated > MaxMallocBytes {
		s.freeExcess()
	}
	return ret
}

func (s *strPool) lessThan(i, j int) bool {
	return s.pool[i].cap < s.pool[j].cap
}

func (s *strPool) freeExcess() {
	for s.allocated > MaxMallocBytes {
		found := false
		// s.pool is sorted
		// iterate backwards so we free biggest chunks first
		for i := len(s.pool) - 1; i >= 0; i-- {
			if !s.pool[i].inuse {
				found = true
				C.free(s.pool[i].p)
				s.statFrees++
				s.allocated -= s.pool[i].cap
				s.pool = append(s.pool[:i], s.pool[i+1:]...)
				break
			}
		}
		if !found {
			// if we can't find any to free, just bail out and allow it
			// we don't want to halt execution or panic here
			return
		}
	}
}

func (s *strPool) GetStringPointer(str string) *StringPointer {
	// request one extra byte for null termination
	sp := s.get(len(str) + 1)
	// from here down is exactly how C.String() works - see go src/cmd/cgo/out.go
	ss := (*[1 << 30]byte)(sp.p)
	copy(ss[:], str)
	ss[len(str)] = 0
	return sp
}

func (s *strPool) FreeStringPointer(sp *StringPointer) {
	sp.inuse = false
	if s.allocated > MaxMallocBytes {
		s.freeExcess()
	}
}

func (s *strPool) destroy() {
	for i := 0; i < len(s.pool); i++ {
		C.free(s.pool[i].p)
		s.statFrees++
		s.allocated -= s.pool[i].cap
	}
}
