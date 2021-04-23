package duktape

import "C"
import (
	"fmt"
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

func NewStrPool() *strPool {
	return &strPool{
		pool:  make([]*cstr, 0),
	}
}

var (
	PoolReuseCount int
	PoolAllocCount int
)

func (s *strPool) get(cap int) *cstr {
	for i := 0; i < len(s.pool); i++ {
		if !s.pool[i].inuse && s.pool[i].cap >= cap {
			PoolReuseCount++
			s.pool[i].inuse = true
			return s.pool[i]
		}
	}
	PoolAllocCount++
	fmt.Printf("new with cap %d\n", cap)
	ret := &cstr{
		p:     C.malloc(C.ulong(cap)),
		cap:   cap,
		inuse: true,
	}
	s.pool = append(s.pool, ret)
	return ret
}

func (s *strPool) CString(str string) *cstr {
	cs := s.get(len(str) + 1)
	ss := (*[1 << 30]byte)(cs.p)
	copy(ss[:], str)
	ss[len(str)] = 0
	return cs
}
