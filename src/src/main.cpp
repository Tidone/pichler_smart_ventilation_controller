#include <Arduino.h>
#include <ArduinoHA.h>
#include <WiFi.h>

#include <HTTPUpdateServer.h>
#include <LEAmDNS.h>
#include <WebServer.h>

#include "StateHandler.h"

#define YELLOW_LED_PIN 12
#define GREEN_LED_PIN 13
#define BUTTON_PIN 14

WebServer httpServer(80);
HTTPUpdateServer httpUpdater;

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HASelect ha_select("fan_control");

ControllerStateHandler controllerStateHandler(YELLOW_LED_PIN, GREEN_LED_PIN, BUTTON_PIN);
StateHandler stateHandler(controllerStateHandler);

const char *ssid = "";<TODO>
const char *password = "";<TODO>
const char *mqtt_server = "";<TODO>
const char *host = "wohnraumlueftung";

void onSelectCommand(int8_t index, HASelect *sender)
{
    Serial1.print("Selected Mode: ");

    switch (index)
    {
        case 0:
            Serial1.println("low");
            stateHandler.setState(ControllerState::FAN_LOW);
            break;
        case 1:
            Serial1.println("medium");
            stateHandler.setState(ControllerState::FAN_MEDIUM);
            break;
        case 2:
            Serial1.println("high");
            stateHandler.setState(ControllerState::FAN_HIGH);
            break;
        default:
            Serial1.println("unknown");
            break;
    }
}

void onControllerStateChanged(ControllerState newState, bool force)
{
    Serial1.printf("Controller state has changed to %s\n", getControllerStateString(newState));

    switch (newState)
    {
        case ControllerState::FAN_LOW:
            ha_select.setState(0, force);
            break;
        case ControllerState::FAN_MEDIUM:
            ha_select.setState(1, force);
            break;
        case ControllerState::FAN_HIGH:
            ha_select.setState(2, force);
            break;
    }
}

void onControllerStateChanged(ControllerState newState)
{
    onControllerStateChanged(newState, false);
}

void setup_wifi()
{
    delay(10);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial1.println();
        Serial1.print("Connecting to ");
        Serial1.println(ssid);

        uint8_t nbr_connection_attempts = 0;
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        while (WiFi.status() != WL_CONNECTED && nbr_connection_attempts < 120)
        {
            // The WiFi module will abort the connection attempt after some time.
            // Therefore we wait for a minute and then restart the connection attempt again.
            nbr_connection_attempts++;
            delay(500);
            Serial1.print(".");
        }
    }

    randomSeed(micros());

    Serial1.println("");
    Serial1.println("WiFi connected");
    Serial1.print("IP address: ");
    Serial1.println(WiFi.localIP());
}

void setup_ha()
{
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));
    device.setName("Wohnraumlüftung");
    device.enableExtendedUniqueIds();
    device.enableSharedAvailability();
    device.enableLastWill();

    ha_select.setName("Betriebsart");
    ha_select.setOptions("Min;Comfort;Max");
    ha_select.setState(0);
    ha_select.onCommand(onSelectCommand);
    ha_select.setIcon("mdi:fan");

    mqtt.begin(mqtt_server);
    mqtt.loop();

    onControllerStateChanged(controllerStateHandler.getState(), true);
}

void check_wifi_connection()
{
    if (!WiFi.connected())
    {
        Serial1.println("Wifi disconnected");
        /* for some reason this will fall through because WiFi.status() will immediately return WL_CONNECTED, even if no connection has been established */
        setup_wifi();
    }
}

void setup()
{
    stateHandler.onStateChanged(onControllerStateChanged);
    stateHandler.begin();

    Serial1.begin(115200);
    setup_wifi();
    setup_ha();

    MDNS.begin(host);

    httpUpdater.setup(&httpServer);
    httpServer.begin();

    MDNS.addService("http", "tcp", 80);
    Serial1.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop()
{
    check_wifi_connection();
    mqtt.loop();
    httpServer.handleClient();
    MDNS.update();
    stateHandler.loop();
}
