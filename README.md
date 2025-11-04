# IoT Environment Monitoring System

## Description

This project implements an **IoT-based environmental monitoring system** using **ESP32**, **MQTT**, and a **Django web backend**.  
The system collects temperature, humidity, and air quality data from sensors and publishes it securely over **TLS-encrypted MQTT**.  
A Python bridge receives the data and stores it in a Django database, where it is visualized on a dynamic web dashboard.

**❗ To view the complete academic paper, open `docs/documentatie.pdf`. ❗**

## System Overview

### Components

1. **ESP32 (Arduino)**
   - Reads data from DHT22 (temperature/humidity) and SGP40 (air quality) sensors.  
   - Publishes data to MQTT topics over a secure connection using TLS certificates.

2. **Python Bridge**
   - Subscribes to MQTT topics and receives sensor updates in real time.  
   - Sends received data to Django via HTTPS POST requests.

3. **Django Backend**
   - Exposes a REST API endpoint for sensor data submission.  
   - Stores received values in an SQLite database.  
   - Displays a web dashboard with:
     - Real-time sensor readings
     - Historical charts using Chart.js
     - CSV export functionality

## Project Structure

```
iot-environment-monitoring-system/
│
├── arduino/
│   ├── main.ino                 # ESP32 firmware for data collection and MQTT publish
│   └── secrets.h                # Connection settings (anonymized)
│
├── python_bridge/
│   └── mqtt_to_django.py        # Secure MQTT subscriber and data forwarder to Django API
│
├── django_backend/
│   ├── manage.py
│   ├── models.py
│   ├── serializers.py
│   ├── urls.py
│   ├── views.py
│   └── templates/senzori/
│       └── sensor_dashboard.html
│
└── docs/
    └── documentatie.pdf         # Full academic documentation
```

## Data Flow

1. Sensors → ESP32 → MQTT Broker (TLS)  
2. MQTT Broker → Python Bridge (subscriber)  
3. Python Bridge → Django API (HTTPS POST)  
4. Django → Database → Web Dashboard (Chart.js)

## Technologies Used

- **ESP32**, **Arduino C++**
- **MQTT** (Mosquitto broker with TLS)
- **Python 3**
- **Django**
- **Chart.js**, **HTML**, **JavaScript**, **CSS**
- **SQLite**

## Features

- Secure TLS communication between ESP32, MQTT, and Django
- REST API for sensor data
- Dynamic dashboard with auto-refreshing charts
- CSV data export
- Modular architecture (hardware, middleware, backend)

## How to Use

1. **Flash the ESP32** with `arduino/main.ino` (update `secrets.h` with your Wi-Fi and broker details).  
2. **Run the Python Bridge:**
   ```bash
   python3 mqtt_to_django.py
   ```
3. **Start Django Backend:**
   ```bash
   python manage.py runserver
   ```
4. Access the web dashboard at:  
   ```
   https://127.0.0.1:8000/senzori/dashboard/
   ```

## Security Notes

- `secrets.h` and `.env` files should **never** be uploaded with real credentials.  
- All communications use **TLS** for encryption.  
- Certificates and private keys must be stored securely.

## Disclaimer

This project was developed as part of an academic thesis.  
It may be used for learning, demonstration, or research purposes, but should not be submitted as original coursework.
