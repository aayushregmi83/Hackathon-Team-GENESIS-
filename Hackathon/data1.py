from flask import Flask, request, jsonify
import csv
import time

app = Flask(__name__)

# Define the CSV file to store sensor data
csv_file = "sensor_data.csv"

# Initialize CSV file with headers if it does not exist
def init_csv():
    try:
        with open(csv_file, mode='x', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['timestamp', 'temperature', 'humidity', 'waterLevel', 'soilMoisture'])
    except FileExistsError:
        pass  # If the file already exists, we don't need to initialize it again

@app.route('/post-sensor-data', methods=['POST'])
def post_sensor_data():
    try:
        data = request.json  # Get the JSON data from the request

        # Extract sensor values from the received data
        temperature = data.get('temperature')
        humidity = data.get('humidity')
        water_level = data.get('waterLevel')
        soil_moisture = data.get('soilMoisture')

        # Log data to CSV file with a timestamp
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        with open(csv_file, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([timestamp, temperature, humidity, water_level,gaslevel,soil_moisture])

        # Respond back to the ESP32
        return jsonify({"message": "Data received and saved to CSV"}), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    init_csv()  # Initialize CSV file with headers
    app.run(host='0.0.0.0', port=5000)
