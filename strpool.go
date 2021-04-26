package duktape

/*
#include <stdlib.h>
*/
import "C"
import (
	"math"
	"unsafe"
)

type strPool struct {
	pool []*cstr
}

type cstr struct {
	p     unsafe.Pointer
	cap   int
	inuse bool
}

func (cs *cstr) CString() *C.char {
	return (*C.char)(cs.p)
}

func NewStrPool() *strPool {
	return &strPool{
		pool: []*cstr{},
	}
}

func (s *strPool) get(cap int) *cstr {
	for i := 0; i < len(s.pool); i++ {
		if !s.pool[i].inuse && s.pool[i].cap >= cap {
			s.pool[i].inuse = true
			return s.pool[i]
		}
	}
	// give them mem with cap that's the next power of 2
	normalizedCap := 2 << int(math.Log2(float64(cap)))
	ret := &cstr{
		p:     C.malloc(C.ulong(normalizedCap)),
		cap:   normalizedCap,
		inuse: true,
	}
	s.pool = append(s.pool, ret)
	return ret
}

func (s *strPool) GetString(str string) *cstr {
	cs := s.get(len(str) + 1)
	ss := (*[1 << 30]byte)(cs.p)
	copy(ss[:], str)
	ss[len(str)] = 0
	return cs
}

func (s *strPool) FreeString(cs *cstr) {
	cs.inuse = false
}

func (s *strPool) destroy() {
	for i := 0; i < len(s.pool); i++ {
		C.free(s.pool[i].p)
	}
}
