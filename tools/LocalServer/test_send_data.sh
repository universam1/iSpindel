curl --header "Content-Type: application/json" \
    --request GET \
    --data '
        {
            "ID": 6548473,
            "RSSI": -49,
            "angle": 0,
            "battery": 5.010427475,
            "gravity": 1.099,
            "interval": 10,
            "name": "TestHydro",
            "temp_units": "C",
            "temperature": 26.875,
            "token": "myToken"
        }' \
    http://localhost:9000/data

curl --header "Content-Type: application/json" \
    --request GET \
    --data '
        {
            "ID": 6548473,
            "RSSI": -49,
            "angle": 0,
            "battery": 5.010427475,
            "gravity": 1.099,
            "interval": 10,
            "name": "Test//Hydro",
            "temp_units": "C",
            "temperature": 26.875,
            "token": "myToken"
        }' \
    http://localhost:9000/data
