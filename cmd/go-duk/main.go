package main

import (
	"io/ioutil"
	"log"
	"os"

	"github.com/clearblade/go-duktape"
)

func main() {
	t := int64(0)
	ctx := duktape.NewWithDeadline(&t)
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

	if err := ctx.PevalString(string(b)); err != nil {
		log.Fatal(err)
	}
}
