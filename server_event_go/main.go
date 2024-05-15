package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
)

var status string = "ok"

func eventHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		//get current time
		t := time.Now()
		//format time
		t.Format("2006-01-02 15:04:05")
		message := r.FormValue("message")
		status = t.String() + message
		fmt.Printf("Received message: %s\n", message)
		fmt.Fprintf(w, "Message received: %s", message)
	} else {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
	}
}
func main() {
	http.HandleFunc("/event", eventHandler)

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, "Status: %s", status)
	})

	fmt.Println("Server listening on port 8080...")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
