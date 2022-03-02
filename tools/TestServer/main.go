package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"github.com/TylerBrock/colorjson"
)

func main() {
	fmt.Println("Test server for iSpindle: connect to <your IP>:8080 using HTTP method")
	http.HandleFunc("/", serve)
	log.Fatal(http.ListenAndServe(":8080", nil))
}

func serve(w http.ResponseWriter, r *http.Request) {
	var obj map[string]interface{}
	fmt.Printf("%+v\n", r)
	str, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Fatal(err)
	}
	err = json.Unmarshal([]byte(str), &obj)
	if err != nil {
		log.Fatal(err)
	}
	f := colorjson.NewFormatter()
	f.Indent = 4
	s, _ := f.Marshal(obj)
	fmt.Println(string(s))

	fmt.Fprint(w, "ok")
}
