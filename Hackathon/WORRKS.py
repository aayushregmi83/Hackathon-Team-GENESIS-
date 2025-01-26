from flask import Flask, request, jsonify
import csv
import os
import logging

app = Flask(__name__)

CSV_FILE = "sensordata.csv"

def init_csv():
    # Explicitly print current working directory
    print(f"Current working directory: {os.getcwd()}")
    
    # Check if file can be created
    try:
        with open(CSV_FILE, mode='a', newline='') as file:
            writer = csv.writer(file)
            if file.tell() == 0:  # If file is empty
                writer.writerow(['Temperature', 'Humidity', 'Water_Level', 'SoilMoisture'])
    except Exception as e:
        print(f"CSV initialization error: {e}")

@app.route('/post-sensor-data', methods=['POST'])
def post_sensor_data():
    try:
        # Enhanced logging
        print("Received POST request")
        
        if not request.is_json:
            print("Not a JSON request")
            return jsonify({"error": "Invalid content type"}), 400

        data = request.json
        print(f"Received data: {data}")

        # Extract and print each field
        temperature = data.get('temperature')
        humidity = data.get('humidity')
        water_level = data.get('waterLevel')  # Note: Changed from 'Water_Level'
        soil_moisture = data.get('soilMoisture')

        print(f"Extracted values: Temp={temperature}, Humidity={humidity}, Water Level={water_level}, Soil Moisture={soil_moisture}")

        if None in [temperature, humidity, water_level, soil_moisture]:
            print("Missing data fields")
            return jsonify({"error": "Missing sensor data fields"}), 400

        # Write to CSV with print to verify
        with open(CSV_FILE, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([temperature, humidity, water_level, soil_moisture])
            print("Data written to CSV successfully")

        return jsonify({"message": "Data received and saved"}), 200

    except Exception as e:
        print(f"Error processing request: {e}")
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    init_csv()
    app.run(host='0.0.0.0', port=5000, debug=True)