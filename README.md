# **MiniMotorways ESP32 Project**

This project is an ESP32-based application for controlling a servo motor and fetching traffic data using the TomTom API. It includes a webserver for configuring Wi-Fi, servo settings, and TomTom API details.

Check out my youtube video for the full project!


## **Features**
- Servo motor control with configurable pin and pulse settings.
- Fetches real-time traffic data using the TomTom API.
- Webserver for configuring Wi-Fi, servo, and API settings.
- Stores configuration in NVS (Non-Volatile Storage) for persistence across reboots.

---

## **Requirements**
- **Hardware**:

    All hardware BOM and full guide are available on my patreon here!

- **Software**:
  - [PlatformIO](https://platformio.org/) for building and flashing the firmware.
  - [Python](https://www.python.org/) with `esptool` installed for manual flashing.
  - A TomTom API key for fetching traffic data.

---

## **Installation**

#### Using Precompiled Firmware from GitHub Releases:
1. Download the latest `.bin` file from the [Releases](https://github.com/your-repo/minimotorways/releases) page.
2. Install the latest version of Python from [here](https://www.python.org/downloads/) and ensure you add it to your system PATH.
3. Open a command prompt and install `esptool.py`:
   ```bash
   pip install esptool
4. Connect your ESP32 board to your computer via USB.
5. Identify the COM port (e.g., COM3 on Windows or /dev/ttyUSB0 on Linux/Mac). You can find this in your device manager. In my case
    -  ![device-manager](resources/device_manager.png)
6. Navigate to the directory where you downloaded the `.bin` file.
6. Flash the firmware, replace <COM_PORT> with the port where your ESP32 is connected (e.g., COM3 or /dev/ttyUSB0).
    ```bash
    python -m esptool --chip esp32-c3 --port <COM_PORT> --baud 460800 write_flash -z 0x1000 firmware.bin
7. Once the flashing process is complete, the ESP32 will reboot and start running the firmware.

#### **Configuration**
1. After flashing the firmware, you should see a Wi-Fi SSID called `MiniMotorways`:

    ![wifi](resources/wifi.png)
2. Connect to it with the password:
    ```
    minimotorways
    ```
3. Once connected to the ESP32, on a browser navigate to the ip address:
    ```
    192.168.4.1
    ```
    A form should show up like this:

    <img src="resources/config.png" alt="config" width="300">

4. For each of the config items refer to the table below to figure out the config item:

| Config Item       | Description                     | Info                           |
|-------------------|---------------------------------| ------------------------------ |
| WiFi SSID         | Your Wi-Fi network name         |                                |
| WiFi Password     | Your Wi-Fi network password     |                                |
| Servo Pin         | GPIO pin connected to the servo | If following my build use `2`    |
| Servo Min Pulse   | Minimum pulse width (in µs)     | Put default value as `510` however may need to calibrate      |
| Servo Max Pulse   | Maximum pulse width (in µs)     | Put default value as `2490` however may need to calibrate     |
| TomTom API Key    | Your TomTom API key             | Follow the guide [here](https://developer.tomtom.com/knowledgebase/platform/articles/how-to-get-an-tomtom-api-key/), to get your free api key. |
| Latitude          | Latitude of your location       | Use a site like [this](https://www.latlong.net/) to input your address to find your latitude. |
| Longitude         | Longitude of your location      | Use the same site as above to find your longitude. |
| Zoom Level        | Zoom level for traffic data     | Zoom level should be 14. Tracks traffic in a 2-5km radius around your coordinates.|


5. After entering all config items and clicking save, the esp32 should reset, connect to your wifi and start pulling traffic data from TomTom!

6. To check if everything works, the servo should rotate 30 degrees twice before starting the constant rotation from -90deg to 90deg if it can connect to wifi and access the TomTom traffic data.
7. If it doesn't show the correct behavior or you believed you entered the wrong config you will need to erase the flash and reinstall the firmware.
```bash
python -m esptool --chip esp32-c3 --port <COM_PORT> erase_flash
```

---

## **Contributing**

If you'd like to help maintain the project, fix bugs, or add new features, here’s how you can get started:

1. **Fork the Repository**:
   - Click the "Fork" button on the top-right corner of the GitHub repository page to create your own copy of the project.

2. **Clone the Repository**:
   - Clone your forked repository to your local machine:
     ```bash
     git clone https://github.com/<your-username>/minimotorways.git
     cd minimotorways
     ```

3. **Create a New Branch**:
   - Create a branch for your feature or bug fix:
     ```bash
     git checkout -b feature-or-bugfix-name
     ```

4. **Make Your Changes**:
   - Modify the code, documentation, or other files as needed. Ensure your changes follow the project's coding style and guidelines.

5. **Test Your Changes**:
   - Test your changes thoroughly to ensure they work as expected and do not introduce new issues.

6. **Submit a Pull Request**:
   - Push your changes to your forked repository:
     ```bash
     git push origin feature-or-bugfix-name
     ```
   - Open a pull request on the original repository and provide a clear description of your changes.

7. **Contact for Collaboration**:
   - If you'd like to help maintain the project, email me at **[your-email@example.com]** with your GitHub profile.

---
