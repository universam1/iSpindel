# Test Server

For a quick test of an iSpindel this tool can to print out the packets send.

It can be run simply with `Golang` build environment installed via `go run .` 

Configure the iSpindel with the following settings:

* Method: 'HTTP'
* Host: your workstation IP
* Port: 8080
* Path: anything (e.g. /test)

## running in a Docker container

* cd into the `tools/TestServer` directory

```bash
docker run -it --rm -v $PWD:/src -p 8080:8080 golang:alpine sh -c "cd /src && go mod tidy && go run ."
```
