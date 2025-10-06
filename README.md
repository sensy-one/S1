
## 🚀 See More. Sense More. Automate Like Never Before.

![Sensy-One Banner](https://github.com/sensy-one/S1/blob/main/assets/images/banner-s1.jpg)

Introducing the Sensy-One S1 – the smallest and most powerful mmWave sensor on the market.
After four months of prototyping, fine-tuning the firmware, and testing, we have developed a sensor that sets a new performance standard. The S1 delivers near-instant motion detection, exceptional accuracy, and seamless Home Assistant integration. [Explore on YouTube](https://www.youtube.com/watch?v=H-Rij8gbK3s)

## ✨ Standout Specs

**Instant Home Assistant Integration**  

The sensor features built-in auto-discovery via the BLE Improv protocol, so it’s instantly recognized by Home Assistant as soon as it’s powered on. No BLE? No problem! It automatically switches to AP mode for an effortless, rock-solid integration.

> Note: BLE Improv is temporarily disabled due to performance issues.

**Precision Motion Tracking**  

Track up to three targets simultaneously in real time. The Hi-Link LD2450 accurately detects X and Y coordinates, movement speed, and more, with a 6-meter range, 120° field of view, and a 35° pitch angle. Whether detecting subtle movements or following dynamic motion, you can count on precise, reliable performance.

**Customizable Detection Zones**  

Our sensor offers flexible monitoring with up to 3 customizable detection zones and one exclusion zone. Each detection zone includes three dedicated sensors that measure movement, presence, and target count, providing detailed insights into activity. Adjustable motion thresholds and timeouts allow you to fine-tune the sensor’s sensitivity and response, ensuring optimal performance in any environment.

**Power-Packed Performance**  

Under the hood, the sensor is driven by the ESP32-S3 Pico microcontroller, boasting dual-core processing and an optimized Wi-Fi module. That means faster response times, real-time data processing, and rock-solid performance for lightning-fast, ultra-reliable automations.

**Compact Design**  

At just 25 mm × 25 mm × 50 mm, this sensor proves that big performance comes in a small package. The rotating wall mount, complete with an adhesive strip for easy installation, lets you direct the sensor exactly where you need it. Available in sleek black or crisp white, it becomes even slimmer, down to 20 mm in thickness when you remove the wall mount for an even subtler installation.

## ⚡ From Power On to Home Assistant

Power your sensor with any 5V supply via its USB-C port. It only needs a little juice to get started. Once powered, the sensor automatically tries to connect to your Home Assistant system.

**For Home Assistant Systems with BLE:**  
- The sensor will appear under **Devices and Services** as **Discovered**.
- Click **Add** and enter your Wi-Fi credentials to connect it to your network.

> Note: BLE Improv is temporarily disabled due to performance issues.

**For Home Assistant Systems without BLE:**  
- The sensor will create its own access point named **I am Sensy!**.
- Connect to the AP and visit `http://192.168.4.1` in your browser.
- Enter your Wi-Fi credentials and click **Save**.
- Once connected, the sensor will appear under **Devices and Services** as **Discovered**.

## 📊 Real-Time Visualization with Plotly Graph Card

Add an interactive map to your Home Assistant dashboard. The Plotly Graph Card automatically displays live target positions, speeds, and any zones you’ve configured. Offering at-a-glance insights into every movement.

**Install HACS and the Plotly Graph Card**  
- If you haven’t already installed HACS, follow the [Official instructions](https://www.hacs.xyz/docs/use/download/download/).
- Once HACS is installed, open it in Home Assistant.
- Search for and install the **Plotly Graph Card** integration.

**Add a Custom Plotly Graph Card**  
- Go to your Home Assistant dashboard and click **Edit Dashboard**.
- Select **Add Card** and choose **Plotly Graph Card**.
- Click **Show Code Editor** to open the YAML configuration editor.
- Copy and paste the custom configuration from the [Git repository](https://github.com/sensy-one/mmwave-sensor/blob/main/assets/config/) into the editor.

**Replace the Placeholder IDs**  
- Use your text editor’s find & replace feature (usually **Ctrl+F** on Windows or **Command+F** on Mac) to locate any `replace_me` placeholders in the YAML configuration.
- Replace them with your sensor name. By default, this might be `sensy_one_1617c8`. If you renamed the sensor (e.g., “Living Room”), use `living_room`.
- If you see a **Visual editor not supported** message, you can safely ignore it. This is normal for custom cards.

## 📍 Get in the Zone

The S1 now supports up to 3 detection zones and 1 exclusion zone — all configurable as custom polygons with up to 8 points each.
Instead of adjusting multiple number entities, you can now set zones visually using the Zone Editor tool:

**How to use the Zone Editor**
- Download the [zone_editor.html](https://github.com/sensy-one/S1/tree/main/assets/config) file.
- Open the Zone Editor on a desktop computer using any modern browser (Chrome, Edge, Firefox, Safari).
- In the Zone Editor, enter the IP address of your S1 sensor (the same one you see in Home Assistant).
- Select which zone you want to configure (Zone 1, Zone 2, Zone 3, or Exclusion).
- Click directly on the radar canvas to place up to 8 points and draw the shape of your zone.
- Save your configuration — it will be applied immediately to your device.

> Note: Make sure your desktop is connected to the same network as your Home Assistant / Sensor.

> Tip: If you run into trouble, you can always have a look at the [YouTube video](https://www.youtube.com/watch?v=raLACrPG8EM) for guidance.

## 🔄 Firmware on the Fly

Keep your sensor up to date with regular OTA updates. We continuously refine performance and add new functionalities to keep your sensor reliable and current.

**Install OTA Updates**  
- Download the latest OTA firmware from the [releases](https://github.com/sensy-one/S1/releases).
- Go to **Devices and Services** and select **ESPHome**. Choose the sensor you want to update.  
- Under **Device Info**, click **Visit** to open the sensor’s web server.
- Scroll down to the **OTA Update**, choose the downloaded firmware file, and click **Update**.  
  *(If the update page times out, simply refresh the page.)*

## ⚙️ Build Your Own Solution

For advanced users looking to create custom firmware, simply integrate the settings below into your yaml.

```yaml
esp32:
  board: esp32-s3-devkitc-1

uart:
  id:
  tx_pin: GPIO15
  rx_pin: GPIO14
  baud_rate: 256000
  parity: NONE
  stop_bits: 1
```

## 🔧 Troubleshooting

Even the most cutting-edge tech can have its off moments. If your sensor isn’t performing exactly as expected, don’t worry. A quick factory reset might be all you need for a fresh start.

**Install Factory Firmware**  
- Download the latest factory firmware from the [releases](https://github.com/sensy-one/S1/releases). 
- Plug the sensor into your PC via USB-C.  
- Open the [Official ESPHome Web Wizard](https://web.esphome.io/?dashboard_wizard).  
- Click **Connect** and select the appropriate COM port for your sensor.  
- Click **Install**, choose the downloaded firmware file, and click **Install** once more.
 
## 💬 Let's Connect

Your feedback fuels our innovation. Whether you're encountering a hiccup or have a brilliant idea to share, we're here to listen.

**Discord:**  
- Chat with our community and get instant help on our [Discord server](https://discord.gg/TB78Wprn66).

**GitHub Issues:**   
- Encountered a bug or have a suggestion? Report it on our [GitHub Issues page](https://github.com/sensy-one/mmwave-sensor/issues).
