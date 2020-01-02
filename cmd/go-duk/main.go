package main

import (
	"io/ioutil"
	"log"
	"os"

	"github.com/clearblade/go-duktape"
)

func main() {
	ctx := duktape.NewWithEventLoop()
	ctx.PevalString(`var console = {log:print,warn:print,error:print,info:print}`)
	if len(os.Args) < 2 {
		log.Fatal("expected an input file")
	}

	f, err := os.Open(os.Args[1])
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	b, err := ioutil.ReadAll(f)
	if err != nil {
		log.Fatal(err)
	}

	in := duktape.GetStringPtr(string(b))
	if err := ctx.PevalStringPtrWithLoop(in); err != nil {
		log.Fatal(err)
	}
}
