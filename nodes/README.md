# LoRa Node
This section contains all source files of our LoRaWan Node.
The current version contains the following node types.

 - [ ] Temprature-node 
 - [ ] Gas-node
 - [x] GPS-LoRa-range-test-node
 - [ ] Gas-temperature-GPS-node


# Compile Node
To compile the LoRa Node you need the following dependencies.

 ## 1. Install the arduino IDE
Download the arduino IDE installer for your OS from: [arduino.cc](https://www.arduino.cc/en/Main/Software)
Start the installer and install the IDE.

## 2. Install Feather Wing M0 dependecies
After you have downloaded and installed **the latest version of Arduino IDE**, you will need to start the IDE and navigate to the **Preferences** menu. You can access it from the **File** menu in _Windows_ or _Linux_, or the **Arduino** menu on _OS X_.

Copy and paste the link below into the **Additional Boards Manager URLs**  option in the Arduino IDE preferences.

### `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`
_** If you use multiple URLS, it is required to separat them with commas**_.

Once done click **OK** to save the new preference settings. 

Now that you have added the appropriate URLs to the Arduino IDE preferences in the previous page, you can open the **Boards Manager** by navigating to the **Tools->Board** menu. First up, install the latest  **Arduino SAMD Boards (**version  **1.6.11** or later). You can type  **Arduino SAMD**  in the top search bar, then when you see the entry, click  **Install**. Next you can install the Adafruit SAMD package to add the board file definitions. Make sure you have  **Type All**  selected to the left of the  _Filter your search..._  box. You can type  **Adafruit SAMD**  in the top search bar, then when you see the entry, click  **Install**.  At last, restart your IDE.

## 3. Install RFM95 dependecies
You will need to download the [RadioHead library](http://www.airspayce.com/mikem/arduino/RadioHead/). You can do that by visiting the github repo and manually downloading the zip file corresponding to version 1.62.

Uncompress the zip and find the folder named **RadioHead** and check that the **RadioHead** folder contains **RH_RF95.****cpp** and **RH_RF95****.h** (as well as a few dozen other files for radios that are supported)  
  
Place the **RadioHead** library folder your **arduinosketchfolder/libraries/** folder.  
You may need to create the **libraries** subfolder if its your first library. Restart the IDE.

## 4. Compile the sketch and download it to the Board
At first, connect your node over USB. Now select the board **Adafruit Feather M0** and the port "COMXX (Adafruit Feather M0)". The repository contains a **lora-node.ino** file in the **node/lora-node** folder. Open this file in your ide and click upload!
 
 If you have a successful upload, you'll get a bunch of red text that tells you that the device was found and it was programmed, verified & reset.

