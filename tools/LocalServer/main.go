package main

import (
	"encoding/csv"
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"html/template"
	"strconv"

	"github.com/TylerBrock/colorjson"
)

func main() {
	// Define a command-line flag for port with a default value of 8080
	port := flag.String("p", "8080", "Port to run the server on")
	flag.Parse() // Parse command-line flags

	fmt.Println("Test server for iSpindel: connect to <your IP>:", *port, " using HTTP method")

	// Register HTTP handlers
	http.HandleFunc("/", homePage)
	http.HandleFunc("/data", handleData)
	http.HandleFunc("/plot", handlePlot)
	http.HandleFunc("/csv/", handleCSV)
	http.HandleFunc("/delete", handleDelete)

	// Start the server
	log.Fatal(http.ListenAndServe(":"+*port, nil))
}

// homePage serves a hidden table with iSpindel names, timestamps, CSV links, and delete links.
func homePage(w http.ResponseWriter, r *http.Request) {
	logsDir := "logs"

	// Read all CSV files in logs directory
	files, err := os.ReadDir(logsDir)
	if err != nil {
		http.Error(w, fmt.Sprintf("Error reading logs directory: %v", err), http.StatusInternalServerError)
		return
	}

	// Slice to store table rows
	var ispindelData []map[string]string

	// Loop through each CSV file
	for _, file := range files {
		if !file.IsDir() && strings.HasSuffix(file.Name(), ".csv") {
			name := strings.TrimSuffix(file.Name(), ".csv")
			filePath := filepath.Join(logsDir, file.Name())

			// Open CSV file
			csvFile, err := os.Open(filePath)
			if err != nil {
				continue
			}
			defer csvFile.Close()

			// Read CSV content
			reader := csv.NewReader(csvFile)
			rows, err := reader.ReadAll()
			if err != nil || len(rows) < 2 {
				continue
			}

			// Get first and last timestamp
			earliestTimestamp := rows[1][0]         // First row after header
			latestTimestamp := rows[len(rows)-1][0] // Last row

			// Add to data slice
			ispindelData = append(ispindelData, map[string]string{
				"name":              name,
				"earliestTimestamp": earliestTimestamp,
				"latestTimestamp":   latestTimestamp,
			})
		}
	}

	// HTML Template
	tmpl := `
	<!DOCTYPE html>
	<html>
	<head>
		<title>iSpindel Plots</title>
        <meta http-equiv="refresh" content="60"> <!-- Auto-refresh every 60 seconds -->
		<style>
			table { width: 100%; }
			th, td { border: 1px solid black; padding: 8px; text-align: center; }
			th { background-color: #f2f2f2; }
		</style>
	</head>
	<body>
		<h2>iSpindel Plots</h2>
		<table id="dataTable">
	<tr>
		<th>iSpindel Name</th>
		<th>Earliest Timestamp</th>
		<th>Latest Timestamp</th>
		<th>CSV</th>
		<th>Delete</th>
	</tr>
	{{range .IspindelData}}
	<tr>
		<td><a href="/plot?ispindel_name={{.name}}">{{.name}}</a></td>
		<td>{{.earliestTimestamp}}</td>
		<td>{{.latestTimestamp}}</td>
		<td><a href="/csv/?filename={{.name}}.csv">CSV</a></td>
		<td><a href="/delete?ispindel_name={{.name}}" onclick="return confirm('Are you sure you want to delete {{.name}}?')">❌ Delete</a></td>
	</tr>
	{{end}}
	</table>

	</body>
	</html>
	`

	// Parse & execute template
	tmplParsed, err := template.New("home").Parse(tmpl)
	if err != nil {
		http.Error(w, fmt.Sprintf("Template parsing error: %v", err), http.StatusInternalServerError)
		return
	}

	err = tmplParsed.Execute(w, map[string]interface{}{
		"IspindelData": ispindelData,
	})
	if err != nil {
		http.Error(w, fmt.Sprintf("Template execution error: %v", err), http.StatusInternalServerError)
	}
}

func handlePlot(w http.ResponseWriter, r *http.Request) {
	// Get ispindel_name from query params
	ispindelName := r.URL.Query().Get("ispindel_name")
	if ispindelName == "" {
		http.Error(w, "Missing 'ispindel_name' parameter", http.StatusBadRequest)
		return
	}

	// Construct file path
	filePath := filepath.Join("logs", ispindelName+".csv")

	// Open CSV file
	csvFile, err := os.Open(filePath)
	if err != nil {
		http.Error(w, fmt.Sprintf("File not found: %s", err), http.StatusNotFound)
		return
	}
	defer csvFile.Close()

	// Read CSV data
	reader := csv.NewReader(csvFile)
	rows, err := reader.ReadAll()
	if err != nil || len(rows) < 2 {
		http.Error(w, "Invalid or empty CSV file", http.StatusInternalServerError)
		return
	}

	// **Step 1: Get the latest temperature unit from the last row**
	latestRow := rows[len(rows)-1] // Last data row
	latestTempUnit := latestRow[4] // Temperature unit (C or F)

	// **Step 2: Extract and convert data**
	var timestamps, temperatures, gravities []string
	var lastRowData []string

	for i, row := range rows[1:] { // Skip header row
		if len(row) < 10 { // Ensure row has the correct number of columns
			continue
		}

		timestamps = append(timestamps, row[0]) // Timestamp
		gravities = append(gravities, row[1])   // Gravity

		// Convert temperature
		tempValue, err := strconv.ParseFloat(row[3], 64)
		if err != nil {
			continue
		}
		tempUnit := row[4] // Current row's temp unit

		// Convert to latestTempUnit if needed
		if latestTempUnit == "C" && tempUnit == "F" {
			tempValue = (tempValue - 32) * 5 / 9
		} else if latestTempUnit == "F" && tempUnit == "C" {
			tempValue = (tempValue * 9 / 5) + 32
		}

		temperatures = append(temperatures, fmt.Sprintf("%.2f", tempValue))

		// Save last row data
		if i == len(rows)-2 { // Last data row
			lastRowData = make([]string, len(row))
			copy(lastRowData, row)
			lastRowData[3] = fmt.Sprintf("%.2f", tempValue) // Format temperature
		}
	}

	// Determine temperature trace name
	tempTraceName := "Temperature " + latestTempUnit

	// HTML Template
	tmpl := `
	<!DOCTYPE html>
	<html>
	<head>
		<title>Plot for {{.IspindelName}}</title>
		<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
		<meta id="refreshMeta" http-equiv="refresh" content="60"> <!-- Default 60s -->
		<style>
			body { font-family: Arial, sans-serif; }
			.refresh-container { position: absolute; top: 10px; right: 20px; }
			select { padding: 5px; font-size: 14px; }
			table { border-collapse: collapse; width: 80%; margin: 20px auto; }
			th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
			th { background-color: #f2f2f2; }
		</style>
	</head>
	<body>
		<h2>Plot for {{.IspindelName}}</h2>

		<!-- Refresh Interval Dropdown -->
		<div class="refresh-container">
			<label for="refreshSelect">Auto Refresh:</label>
			<select id="refreshSelect">
				<option value="5">5s</option>
				<option value="10">10s</option>
				<option value="60">1min</option>
				<option value="300">5min</option>
				<option value="600">10min</option>
				<option value="off">Off</option>
			</select>
		</div>

		<div id="plot"></div>

		<!-- Latest Data Table -->
		<h3>Latest Data</h3>
		<table>
			<tr>
				<th>Timestamp</th>
				<th>Gravity</th>
				<th>Angle</th>
				<th>Temperature</th>
				<th>Temperature Units</th>
				<th>Battery</th>
				<th>Interval</th>
				<th>Name</th>
				<th>ID</th>
				<th>RSSI</th>
				<th>Token</th>
			</tr>
			<tr>
				{{range .LastRow}}
					<td>{{.}}</td>
				{{end}}
			</tr>
		</table>

		<script>
			// Load stored refresh interval from localStorage (default: "off")
			var refreshInterval = localStorage.getItem('refreshInterval') || "off";
			document.getElementById("refreshSelect").value = refreshInterval;

			function setRefreshTimer(interval) {
				clearTimeout(window.refreshTimer); // Clear any existing timer
				if (interval !== "off") {
					window.refreshTimer = setTimeout(() => location.reload(), interval * 1000);
				}
			}

			// Apply stored refresh interval
			setRefreshTimer(refreshInterval);

			// Event Listener for Dropdown Change
			document.getElementById("refreshSelect").addEventListener("change", function() {
				var newInterval = this.value;
				localStorage.setItem('refreshInterval', newInterval); // Save choice
				setRefreshTimer(newInterval); // Apply new interval
			});
			var trace1 = {
				x: {{.Timestamps}},
				y: {{.Temperatures}},
				mode: 'lines+markers',
				name: '{{.TempTraceName}}',
				line: {color: 'red'}
			};
			var trace2 = {
				x: {{.Timestamps}},
				y: {{.Gravities}},
				mode: 'lines+markers',
				name: 'Gravity',
				line: {color: 'blue'}
			};
			var layout = {
				title: 'Temperature and Gravity Over Time',
				xaxis: {title: 'Timestamp'},
				yaxis: {title: 'Values'},
				legend: {x: 1, y: 1}
			};
			Plotly.newPlot('plot', [trace1, trace2], layout);
		</script>
	</body>
	</html>
	`

	// Parse & execute template
	tmplParsed, err := template.New("plot").Parse(tmpl)
	if err != nil {
		http.Error(w, fmt.Sprintf("Template parsing error: %v", err), http.StatusInternalServerError)
		return
	}

	err = tmplParsed.Execute(w, map[string]interface{}{
		"IspindelName":  ispindelName,
		"Timestamps":    timestamps,
		"Temperatures":  temperatures,
		"Gravities":     gravities,
		"TempTraceName": tempTraceName,
		"LastRow":       lastRowData,
	})
	if err != nil {
		http.Error(w, fmt.Sprintf("Template execution error: %v", err), http.StatusInternalServerError)
	}
}

func handleData(w http.ResponseWriter, r *http.Request) {
	var data map[string]interface{}
	fmt.Printf("%+v\n", r)
	str, err := ioutil.ReadAll(r.Body)
	if err != nil {
		log.Println(err)
	}

	err = json.Unmarshal([]byte(str), &data)
	if err != nil {
		log.Println(err)
	}

	if err := createOrAppendCSV(data); err != nil {
		fmt.Println("Error:", err)
	}

	f := colorjson.NewFormatter()
	f.Indent = 4
	s, _ := f.Marshal(data)
	fmt.Println(string(s))

	fmt.Fprint(w, "ok")
}

/*
&{Method:POST URL:/test Proto:HTTP/1.1 ProtoMajor:1 ProtoMinor:1 Header:map[Accept-Encoding:[identity;q=1,chunked;q=0.1,*;q=0] Connection:[keep-alive] Content-Length:[181] Content-Type:[application/json] User-Agent:[ESP8266HTTPClient]] Body:0xc0000e72c0 GetBody:<nil> ContentLength:181 TransferEncoding:[] Close:false Host:192.168.0.199:9000 Form:map[] PostForm:map[] MultipartForm:<nil> Trailer:map[] RemoteAddr:192.168.0.99:49605 RequestURI:/test TLS:<nil> Cancel:<nil> Response:<nil> ctx:0xc0000e7300}

{
    "ID": 6548473,
    "RSSI": -49,
    "angle": 0,
    "battery": 5.010427475,
    "gravity": -14.0539999,
    "interval": 10,
    "name": "SpinHydro",
    "temp_units": "C",
    "temperature": 26.875,
    "token": "8hyLBUGgsPvEzoDyJpzu"
}
*/
// createOrAppendCSV writes a map[string]interface{} to a CSV file based on the "name" field in the map,
// adding a "timestamp" column with the current datetime.
func createOrAppendCSV(json map[string]interface{}) error {
	// Ensure logs directory exists
	logsDir := "logs"
	if err := os.MkdirAll(logsDir, os.ModePerm); err != nil {
		return fmt.Errorf("failed to create logs directory: %w", err)
	}

	// Determine filename from "name" field
	name, exists := json["name"].(string)
	if !exists || name == "" {
		name = "default"
	}
	fileName := filepath.Join(logsDir, name+".csv")

	// Define the order of the keys to maintain consistency
	headers := []string{"timestamp", "gravity", "angle", "temperature", "temp_units", "battery", "interval", "name", "ID", "RSSI", "token"}

	// Check if the file exists
	_, err := os.Stat(fileName)
	isNewFile := os.IsNotExist(err)

	// Open file in append mode, create if it doesn't exist
	file, err := os.OpenFile(fileName, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return fmt.Errorf("failed to open file: %w", err)
	}
	defer file.Close()

	writer := csv.NewWriter(file)
	defer writer.Flush()

	// Write headers if it's a new file
	if isNewFile {
		if err := writer.Write(headers); err != nil {
			return fmt.Errorf("failed to write headers: %w", err)
		}
	}

	// Get current timestamp
	timestamp := time.Now().Format("2006-01-02 15:04:05")

	// Convert map values to strings and write to CSV
	var row []string
	for _, key := range headers {
		if key == "timestamp" {
			row = append(row, timestamp)
			continue
		}

		value, exists := json[key]
		if !exists {
			row = append(row, "") // Add empty string if key is missing
			continue
		}

		// Convert interface{} to string with proper type assertions
		switch v := value.(type) {
		case string:
			row = append(row, v)
		case int, int64:
			row = append(row, fmt.Sprintf("%d", v)) // Ensures integer formatting
		case float64:
			// Convert specific fields to integers
			if key == "ID" || key == "interval" || key == "RSSI" {
				row = append(row, fmt.Sprintf("%d", int(v))) // Explicitly cast float64 to int
			} else {
				row = append(row, fmt.Sprintf("%.2f", v)) // Keeps float format for non-integer fields
			}
		default:
			row = append(row, fmt.Sprintf("%v", v)) // Generic fallback
		}
	}

	if err := writer.Write(row); err != nil {
		return fmt.Errorf("failed to write data row: %w", err)
	}

	fmt.Printf("Data successfully written to %s\n", fileName)
	return nil
}

func handleCSV(w http.ResponseWriter, r *http.Request) {
	// Get filename from query params
	filename := r.URL.Query().Get("filename")
	if filename == "" {
		http.Error(w, "Missing 'filename' parameter", http.StatusBadRequest)
		return
	}

	// Ensure filename is safe (prevents directory traversal attacks)
	if strings.Contains(filename, "..") || !strings.HasSuffix(filename, ".csv") {
		http.Error(w, "Invalid filename", http.StatusBadRequest)
		return
	}

	// Construct file path
	filePath := filepath.Join("logs", filename)

	// Open file
	csvFile, err := os.Open(filePath)
	if err != nil {
		http.Error(w, fmt.Sprintf("File not found: %s", err), http.StatusNotFound)
		return
	}
	defer csvFile.Close()

	// Read content
	data, err := os.ReadFile(filePath)
	if err != nil {
		http.Error(w, "Error reading file", http.StatusInternalServerError)
		return
	}

	// Set response headers for plaintext
	w.Header().Set("Content-Type", "text/plain")
	w.Write(data)
}

func handleDelete(w http.ResponseWriter, r *http.Request) {
	// Get ispindel_name from query params
	ispindelName := r.URL.Query().Get("ispindel_name")
	if ispindelName == "" {
		http.Error(w, "Missing 'ispindel_name' parameter", http.StatusBadRequest)
		return
	}

	// Construct file path
	filePath := filepath.Join("logs", ispindelName+".csv")

	// Delete the file
	err := os.Remove(filePath)
	if err != nil {
		http.Error(w, fmt.Sprintf("Failed to delete file: %v", err), http.StatusInternalServerError)
		return
	}

	// Redirect back to home
	http.Redirect(w, r, "/", http.StatusSeeOther)
}
