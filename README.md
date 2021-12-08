# esp32-ip-cam

Over the internet available live vide stream from ESP32 camera (tested ob `M5CAMERA X` module).

> **Prerequisites** 
>
> Install [Visual Studio Code](https://code.visualstudio.com/) with [PlatformIO extension](https://platformio.org/install/ide?install=vscode).

## Quick start

1. Click **[Use this template](https://github.com/husarnet/esp32-internet-ota/generate)** button to create your own copy of this repo.

2. Clone the repo you have just created and open it in Visual Studio Code. Platformio should automatically install all project dependencies.

3. Rename `credentials-template.h` to `credentials.h` and type your WiFi an Husarnet credentials there (you will find you Husarnet Join Code at https://app.husarnet.com).

4. Click "PlatformIO: upload" button to flash your ESP32 board connected to your laptop. You will find the following log in the serial monitor:

    ```bash
    **************************************
    GitHub Actions OTA example
    **************************************
    
    📻 1. Connecting to: FreeWifi Wi-Fi network .. done

    ⌛ 2. Waiting for Husarnet to be ready ... done

    🚀 HTTP server started

    Visit:
    http://my-esp32-cam:8080/stream

    Known hosts:
    my-laptop (fc94:a4c1:1f22:ab3b:b04a:1a3b:ba15:84bc)
    my-esp32-cam (fc94:f632:c8d9:d2a6:ad18:ed16:ed7e:9f3f)
    ```

## Tips

### Erasing flash memory of ESP32

1. Connect ESP32 to your laptop

2. Install platformio CLI

    ```bash
    pip install -U platformio
    ```

3. Make flash erase:

    ```bash
    pio run --target erase
    ```

### Monitoring network traffic on `hnet0` interface

```bash
sudo tcpflow -p -c -i hnet0
```

### Accesing a webserver hosted by ESP32 using a public domain

Here is a blog post showing how to configure Nginx Proxy Manager to **provide a public access to web servers hosted by Husarnet connected devices**: https://husarnet.com/blog/reverse-proxy-gui

It can be also used  o provide the access to a web server hosted by ESP32 using a nice looking link like: `https://my-awesome-esp32.mydomain.com`.