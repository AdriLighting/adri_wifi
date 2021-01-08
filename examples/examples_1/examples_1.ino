#include <adri_wifi.h>
boolean WIFI_CONNECTSTART 	= true;

void setup()
{
    Serial.begin(115200);

    // CONNECT TO WIFI
    IPAddress no_ip = {0, 0, 0, 0};
	wifi_connect_sta("xx", "xxx", no_ip, no_ip, no_ip);

}
void loop()
{

}
