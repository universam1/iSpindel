# Local Server

This server collects and plot data for iSpindel on LAN.

Since this simple server uses HTTP, do NOT expose it to the public network!

It can be run simply with `Golang` build environment installed via `go run .` 

An optional `-p` flag can be used to specify the port to run on (Default is `8080`).

E.g. `go run . -p 9000`

Configure the iSpindel with the following settings:

* Method: HTTP
* Host: your local server IP
* Port: port you are running on (default 8080)
* Path: /data

You can run this in a tmux, or as a service.

Data collected are saved as `.csv` under `.../LocalServer/logs/<ispindel_name>.csv`.
Therefore changing the device name would start a new save file.

Author: kaihuang201
