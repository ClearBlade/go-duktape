package duktape

/*
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"math"
	"strings"
	"unsafe"
)

func (d *Context) GetStringPtr(src string) *StringPointer {
	return d.strPool.GetStringPointer(src)
}

func (d *Context) FreeStringPtr(sp *StringPointer) {
	d.strPool.FreeStringPointer(sp)
}

type strPool struct {
	pool []*StringPointer
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
	return fmt.Sprintf("POOL{%s}", strings.Join(pointers, ", "))
}

func (s *strPool) get(cap int) *StringPointer {
	for i := 0; i < len(s.pool); i++ {
		if !s.pool[i].inuse && s.pool[i].cap >= cap {
			s.pool[i].inuse = true
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
	s.pool = append(s.pool, ret)
	return ret
}

func (s *strPool) GetStringPointer(str string) *StringPointer {
	sp := s.get(len(str) + 1)
	ss := (*[1 << 30]byte)(sp.p)
	copy(ss[:], str)
	ss[len(str)] = 0
	return sp
}

func (s *strPool) FreeStringPointer(sp *StringPointer) {
	sp.inuse = false
}

func (s *strPool) destroy() {
	for i := 0; i < len(s.pool); i++ {
		C.free(s.pool[i].p)
	}
}
