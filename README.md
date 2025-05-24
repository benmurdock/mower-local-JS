# ESP32 Mower Control Web Server

This project transforms an ESP32 microcontroller into a Wi-Fi Access Point (AP) that hosts a web server. This web server provides a simple web interface to control a robotic mower, including its ground speed, blade status, and joystick-based movement. It also displays real-time uptime and information about connected Wi-Fi clients.

The codebase is structured into multiple files for improved readability, maintainability, and modularity, making it easier to manage and extend.

## Features

- **Wi-Fi Access Point:** ESP32 creates its own Wi-Fi network (`ESP32 WIFI` with password `11111111`).
    
- **Web Server:** Hosts a responsive web interface accessible via `http://mower.local` (thanks to mDNS) or the ESP32's assigned IP address.
    
- **Mower Control:**
    
    - Toggle ground speed (Slow/Fast).
        
    - Toggle blade status (Off/5000 fpm/12000 fpm).
        
    - Joystick control for differential steering (Left Track/Right Track speed).
        
- **Real-time Feedback:**
    
    - Displays device uptime.
        
    - Shows a list of connected Wi-Fi clients, including their MAC addresses and RSSI (signal strength).
        
- **Modular Codebase:** Organized into separate files for clarity and easier development.
    

## File Structure

The project is divided into the following files:

- `mower_control.ino`: The main Arduino sketch file containing `setup()` and `loop()` functions, global variable definitions, and the primary program flow.
    
- `wifi_config.h`: A header file containing the Wi-Fi Access Point's SSID and password.
    
- `html_content.h`: A header file that stores the entire HTML, CSS, and JavaScript content for the web interface. This keeps the main sketch file clean.
    
- `web_handlers.h`: A header file declaring the global variables (using `extern`) and function prototypes for the web server route handlers.
    
- `web_handlers.cpp`: The implementation file for the web server route handlers, defining how the server responds to various HTTP requests (e.g., `/groundSpeed`, `/joystick`).
    

## Dependencies

This project relies on the following Arduino libraries, which should be installed in your Arduino IDE:

- `WiFi.h` (comes with ESP32 Arduino Core)
    
- `WebServer.h` (comes with ESP32 Arduino Core)
    
- `ESPmDNS.h` (comes with ESP32 Arduino Core)
    
- `esp_wifi.h` (part of ESP-IDF, accessible via ESP32 Arduino Core)
    

Ensure you have the ESP32 Board Manager installed in your Arduino IDE (`Tools > Board > Boards Manager`).

## How to Compile and Upload

1. **Open Arduino IDE:** Launch the Arduino IDE.
    
...
    
8. **Upload:** Click the "Upload" button (right arrow icon) to compile and upload the code to your ESP32.
    

## Usage

1. **Power On ESP32:** Connect your ESP32 to a power source.
    
2. **Connect to Wi-Fi:** On your computer or mobile device, connect to the Wi-Fi network named `ESP32 WIFI` using the password `11111111`.
    
3. **Access Web Interface:** Open a web browser and navigate to `http://mower.local`. If mDNS is not working on your network, you can find the ESP32's IP address in the Arduino Serial Monitor after it starts up (e.g., `AP IP address: 192.168.4.1`) and use that directly in your browser.
    
4. **Control Mower:** Use the buttons to toggle ground speed and blade settings. Drag the joystick handle to control the mower's movement.
    
5. **Monitor Info:** Observe the uptime and connected client information updating in real-time on the webpage. You can also monitor client information in the Arduino Serial Monitor.