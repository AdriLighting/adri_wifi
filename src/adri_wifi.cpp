#include "adri_wifi.h"

#include <arduino.h>

#include <adri_tools.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DEBUG_WIFICONNECT
#ifdef DEBUG_WIFICONNECT
    #define DEBUG
#endif

extern boolean WIFI_CONNECTSTART;

// #define WIFIMOD WIFI_STA
// #define WIFIMOD WIFI_AP_STA
#define WIFIMOD WIFI_STA

char b_host_name[80];
void build_host_name(char * hostnamePrefix) {
   unsigned char mac[6];
   WiFi.macAddress(mac);
   strcpy(b_host_name, hostnamePrefix);
   for (int i=4; i<6; i++) {char b[2]; sprintf(b,"%02x",mac[i]); strcat(b_host_name, b); }
}

WIFI_ID * wifi_id;

String wifi_id_get_subnet(WIFI_ID * wifi)     {return wifi->_subnet;}
String wifi_id_get_gateway(WIFI_ID * wifi)    {return wifi->_gateway;}
String wifi_id_get_ip(WIFI_ID * wifi)         {return wifi->_ip;}
String wifi_id_get_pswd(WIFI_ID * wifi)       {return wifi->_pswd;}
String wifi_id_get_ssid(WIFI_ID * wifi)       {return wifi->_ssid;}
String wifi_id_get_ap_ip(WIFI_ID * wifi)      {return wifi->_ap_ip;}
String wifi_id_get_ap_psk(WIFI_ID * wifi)     {return wifi->_ap_psk;}
String wifi_id_get_ap_ssid(WIFI_ID * wifi)    {return wifi->_ap_ssid;}
String wifi_id_get_mod(WIFI_ID * wifi)        {return wifi_modToString(wifi->stationMod);}
String wifi_id_get_noip(WIFI_ID * wifi)       {return wifi_modToString(wifi->ipMod);}

String wifi_id_sav_mod(WIFI_ID * wifi)        {return wifi_mod_toSpiff(wifi->stationMod);}

 wifi_id_getValue wifi_id_getValue_list [] = {           
    {"wifi_ssid",       &wifi_id_get_ssid,      0, true,    &wifi_id_get_ssid},
    {"wifi_psk",        &wifi_id_get_pswd,      0, true,    &wifi_id_get_pswd},
    {"wifi_ip",         &wifi_id_get_ip,        0, true,    &wifi_id_get_ip},
    {"ap_ssid",         &wifi_id_get_ap_ssid,   1, false,   &wifi_id_get_ap_ssid},
    {"ap_psk",          &wifi_id_get_ap_psk,    1, false,   &wifi_id_get_ap_psk},
    {"wifiAp_ip",       &wifi_id_get_ap_ip,     1, true,    &wifi_id_get_ap_ip},
    {"wifi_subnet",     &wifi_id_get_subnet,    2, true,    &wifi_id_get_subnet},
    {"wifi_gateway",    &wifi_id_get_gateway,   2, true,    &wifi_id_get_gateway},
    {"wifi_mod",        &wifi_id_get_mod,       2, true,    &wifi_id_sav_mod},
    {"cfgIp",           &wifi_id_get_noip,      2, false,   &wifi_id_get_noip},

};
int wifi_id_getValue_count = ARRAY_SIZE(wifi_id_getValue_list);

    fs(sys_mode,            "mode");
    fs(sys_connecter,       "connecter a");
    fs(sys_ipAp,              "ipAp");
    fs(sys_ip,              "ip");
    fs(sys_mdp,             "mdp");
    fs(sys_dns,             "nom dns");
    fs(sys_webserver,       "webserver");
    #ifdef CFG_SERVER_WEBSERVER
    fs(sys_webserverName,   "ESPWebserver");
    #else
    fs(sys_webserverName,   "ESPAsyncWebServer");
    #endif
    fs(sys_ota,             "ota");
    fs(sys_websocket,       "websocket");
    fs(sys_activer,         "activer");
    fs(sys_desactiver,      "desactiver");
    fs(sys_subnet,          "gateway");
    fs(sys_gateway,         "subnet");
    fs(sys_wifistart,       "wificonnect");
    fs(sys_apHost,          "hostname");
String network_info(String hostName) {
    
    String s;
    s+="\nRESEAUX\n";


    WiFiMode_t w_mod = WiFi.getMode(); 

    if (w_mod == 1) {
        s+= info_parm(fsget(sys_mode),      String(w_mod) + ": STA"); 
        s+= info_parm(fsget(sys_connecter), wifi_id->_ssid);
        s+= info_parm(fsget(sys_ip),        WiFi.localIP().toString());
        s+= info_parm(fsget(sys_mdp),       wifi_id->_pswd);
        s+= info_parm(fsget(sys_subnet),    WiFi.subnetMask().toString());
        s+= info_parm(fsget(sys_gateway),   WiFi.gatewayIP().toString());
        s+= info_parm(fsget(sys_dns),       hostName+".local");
    }
    if (w_mod == 2) {
        s+= info_parm(fsget(sys_mode),      String(w_mod) + ": AP"); 
        s+= info_parm(fsget(sys_connecter), String(b_host_name));
        s+= info_parm(fsget(sys_ip),        WiFi.softAPIP().toString());
        s+= info_parm(fsget(sys_mdp),       wifi_id->_ap_psk);
        s+= info_parm(fsget(sys_dns),       hostName+".local");
    }   
    if (w_mod == 3) {
        s+= info_parm(fsget(sys_mode),          String(w_mod) + ": APSTA"); 
        s+= info_parm(fsget(sys_connecter),     wifi_id->_ssid);
        s+= info_parm(fsget(sys_mdp),           wifi_id->_pswd);
        s+= info_parm(fsget(sys_ip),            WiFi.localIP().toString());
        s+= info_parm(fsget(sys_subnet),        WiFi.subnetMask().toString());
        s+= info_parm(fsget(sys_gateway),       WiFi.gatewayIP().toString());        
        s+= info_parm(fsget(sys_dns),           hostName+".local");
        s+= info_parm(fsget(sys_ipAp),          WiFi.softAPIP().toString());
    }  

    s+=info_parm(fsget(sys_webserver),  fsget(sys_webserverName));
    #ifdef OTA_ENABLE
    s+=info_parm(fsget(sys_ota),        fsget(sys_activer));
    #else
    s+=info_parm(fsget(sys_ota),        fsget(sys_desactiver));
    #endif        
    #ifdef ESP_WEBSOCKETSERVER
    s+=info_parm(fsget(sys_websocket),  fsget(sys_activer));
    #else
    s+=info_parm(fsget(sys_websocket),  fsget(sys_desactiver));
    #endif      
    if (WIFI_CONNECTSTART)  s+=info_parm(fsget(sys_wifistart),  "void setup");
    else                    s+=info_parm(fsget(sys_wifistart),  "void loop");
    s+=info_parm(fsget(sys_apHost),     String(b_host_name));
    return s;
}

void wifi_connect(char* ssid, char* password) {
    #ifdef DEBUG
        fsprintf("\n[wifi_connect] ssid: %s - pswd: %s \n", ssid, password);
    #endif  

    int retries = 0;
    int maxRetries = 0;   


    WiFi.disconnect();
    delay(1000);

    WiFi.mode(WIFIMOD);   
    WiFi.begin(ssid, password);

    #ifdef DEBUG
        fsprintfs("\n");
    #endif    

    while (WiFi.status() != WL_CONNECTED)  {
        retries++;

        if (retries > 40) {  

            WiFi.disconnect();
            delay(1000);
            
            WiFi.mode(WIFIMOD);   
            WiFi.begin(ssid, password);

            retries = 0;
            maxRetries++;

            #ifdef DEBUG
                fsprintf("\n\tmaxRetries: %d\n", maxRetries);
            #endif
        }   

        delay(500);
        #ifdef DEBUG
            Serial.print(".");
        #endif

        if (maxRetries > 2) break;
    }

    #ifdef DEBUG
        Serial.println("");
        Serial.println("[wifi_connect] Done");
    #endif  
}
void wifi_connect(char* ssid, char* password, wifiConnectCallBack * drawprogress){
    #ifdef DEBUG
        fsprintf("\n[wifi_connect] ssid: %s - pswd: %s \n", ssid, password);
    #endif  

    int retries = 0;
    int maxRetries = 0;   


    WiFi.disconnect();
    delay(1000);

    WiFi.mode(WIFIMOD);   
    WiFi.begin(ssid, password);

    #ifdef DEBUG
        fsprintfs("\n");
    #endif    

    uint8_t count=0;
    while (WiFi.status() != WL_CONNECTED)  {
        retries++;

        // #ifdef OLED_ENABLE
            count+=30;
            (*drawprogress)(count);
        // #endif

        if (retries > 40) {  

            WiFi.disconnect();
            delay(1000);
            
            WiFi.mode(WIFIMOD);   
            WiFi.begin(ssid, password);

            retries = 0;
            maxRetries++;

            #ifdef DEBUG
                fsprintf("\n\tmaxRetries: %d\n", maxRetries);
            #endif
        }   

        delay(250);
        #ifdef DEBUG
            Serial.print(".");
        #endif

        if (maxRetries > 4) break;
    }

    #ifdef DEBUG
        Serial.println("");
        Serial.println("[wifi_connect] Done");
    #endif  
}

void wifi_loop_connect(char* ssid, char* password){

    WiFi.mode(WIFIMOD);   
    WiFi.setAutoReconnect(true);  
    delay(1000);
    WiFi.begin(ssid, password);
}

WIFI_STATU wifi_statu;
unsigned long wifi_statu_sta_search_last;
unsigned long wifi_statu_sta_search_max = 15000;
void wifi_loop(){
    IPAddress ap_subnet(255, 255, 255, 0);
    IPAddress staticIP; 
    IPAddress gateway;
    IPAddress subnet;  
    char sSSID[80];
    char sPSWD[80]; 

   

    switch (wifi_statu) {

        case wifi_statu_sta_begin:
            Serial.printf("\n[wifi_statu_sta_begin]\n");
            if(wifi_id->stationMod == ADWIFI_STATION) {
                sprintf(sSSID, "%s", wifi_id->_ssid.c_str());
                sprintf(sPSWD, "%s", wifi_id->_pswd.c_str());
                wifi_loop_connect(sSSID, sPSWD);

            }
            wifi_statu_sta_search_last = millis();
            wifi_statu = wifi_statu_sta_search;
        break;
        case wifi_statu_sta_search:
            if (WiFi.status() == WL_CONNECTED){
                wifi_statu = wifi_statu_sta_isconnected;
                Serial.printf("\n[wifi_statu_sta_isconnected] : %s - %s - %s\n", 
                    WiFi.localIP().toString().c_str(), 
                    WiFi.gatewayIP().toString().c_str(), 
                    WiFi.subnetMask().toString().c_str()
                );
                if (wifi_id->ipMod == ADWIFI_STATICIP) {
                    staticIP = string2ip(wifi_id->_ip);
                    wfifi_retIP("gateway", wifi_id->_gateway,   &gateway);
                    wfifi_retIP("subnet", wifi_id->_subnet,     &subnet);
                    wifi_connect_configIp(staticIP, gateway, subnet);
                }
            }
            else {
                if ((millis()-wifi_statu_sta_search_last)>wifi_statu_sta_search_max){
                    wifi_statu = wifi_statu_sta_fail;
                }
            }
        break;
        case wifi_statu_sta_fail:
            wifi_statu = wifi_statu_sta_isconnected;
            Serial.printf("\n[wifi_statu_sta_fail]\n");
            WiFi.disconnect();
            WiFi.mode(WIFI_AP);
            WiFi.softAP(b_host_name, wifi_id->_ap_psk);
            delay(500);

            if (wifi_id->ipMod == ADWIFI_STATICIP) {
                staticIP = string2ip(wifi_id->_ap_ip);
                WiFi.softAPConfig(staticIP, staticIP, ap_subnet);
            }  

        break; 
    }


    // WiFi.mode(WIFIMOD);   
    // WiFi.setAutoReconnect(true);  

    // WiFi.disconnect();
    // delay(20);

    // #ifdef OLED_ENABLE
    //     int count = 0;
    // #endif 

    // WiFi.begin(ssid, password);

     
}
boolean wifi_connect_result(){

    if (WiFi.status() == WL_CONNECTED) {
        #ifdef DEBUG
            Serial.printf("\n[wifi_connect_result] WL_CONNECTED: %s - %s - %s\n\n", 
                WiFi.localIP().toString().c_str(), 
                WiFi.gatewayIP().toString().c_str(), 
                WiFi.subnetMask().toString().c_str());
        #endif
        return true;
    } else {
        #ifdef DEBUG
            Serial.printf("\n[wifi_connect_result] Fail\n\n");
        #endif
        return false;
    }

}



String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

boolean isValidIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}
boolean isValidIp(IPAddress ip){
    String  s_ip    = toStringIp(ip);
    boolean v1      = isValidIp(s_ip);
    if (!v1) return false;

    if (ip[0] == 0) return false;
    if (ip[1] == 0) return false;    
    return true;
}

/*
    Simple connection et configuration wifi depuis les parama
*/
boolean _wifi_connect_2(IPAddress ip, IPAddress gateway, IPAddress subnet) {

    #ifdef DEBUG
        Serial.printf("\n[_wifi_connect_2] \n");
    #endif

    if (isValidIp(ip)) {
        #ifdef DEBUG
            Serial.printf("\tassign ip to: %s \n",  ip2string(ip).c_str());
        #endif   

        if (isValidIp(gateway)) {
            #ifdef DEBUG
                Serial.printf("\tassign gateway to: %s \n",  ip2string(gateway).c_str());
            #endif    
        } else gateway = WiFi.gatewayIP();

        if (isValidIp(subnet)) {
            #ifdef DEBUG
                Serial.printf("\tassign subnet to: %s \n",  ip2string(subnet).c_str());
            #endif    
        } else subnet = WiFi.subnetMask();
            // [wifi_connect_sta] Done: 192.168.0.100 - 192.168.0.254 - 255.255.255.0
        WiFi.config(ip, gateway, subnet);
        // WiFi.config(ip, {192,168,0,254}, {255,255,255,0});


        // WiFi.softAPConfig(ip, ip, subnet);        
        // String AP_psk = "adrilight1234";
        // WiFi.softAP(b_host_name, AP_psk);

    } 

    #ifdef DEBUG
        Serial.printf("\n[wifi_connect_sta] Done: %s - %s - %s\n", 
            WiFi.localIP().toString().c_str(), 
            WiFi.gatewayIP().toString().c_str(), 
            WiFi.subnetMask().toString().c_str());
    #endif 
}
boolean _wifi_connect_1(char* ssid, char* password, wifiConnectCallBack * drawprogress) {

    #ifdef DEBUG
        Serial.printf("\n[_wifi_connect_1] \n");
    #endif

    wifi_connect(ssid, password, drawprogress);
    boolean result = wifi_connect_result();
    return result;
}
boolean _wifi_connect_1(char* ssid, char* password) {

    #ifdef DEBUG
        Serial.printf("\n[_wifi_connect_1] \n");
    #endif

    wifi_connect(ssid, password);
    boolean result = wifi_connect_result();
    return result;
}

boolean wifi_connect_sta(char* ssid, char* password, IPAddress ip, IPAddress gateway, IPAddress subnet, wifiConnectCallBack * drawprogress) {
    boolean result =    _wifi_connect_1(ssid, password, drawprogress);
    if (result)         _wifi_connect_2(ip, gateway, subnet);
}
boolean wifi_connect_sta(char* ssid, char* password, IPAddress ip, IPAddress gateway, IPAddress subnet) {
    boolean result =    _wifi_connect_1(ssid, password);
    if (result)         _wifi_connect_2(ip, gateway, subnet);
}
void wifi_connect_sta(boolean * result) {
   if (wifi_id->stationMod == ADWIFI_STATION)   *result=true;
   if (wifi_id->stationMod == ADWIFI_AP)        *result=false;
}
void wifi_connect_configIp(IPAddress ip, IPAddress gateway, IPAddress subnet) {

    #ifdef DEBUG
        Serial.printf("\n[wifi_connect_configIp] \n");
    #endif

        if (isValidIp(ip)) {
            #ifdef DEBUG
                Serial.printf("\tassign ip to: %s \n",  ip2string(ip).c_str());
            #endif   

            if (isValidIp(gateway)) {
                #ifdef DEBUG
                    Serial.printf("\tassign gateway to: %s \n",  ip2string(gateway).c_str());
                #endif    
            } else gateway = WiFi.gatewayIP();

            if (isValidIp(subnet)) {
                #ifdef DEBUG
                    Serial.printf("\tassign subnet to: %s \n",  ip2string(subnet).c_str());
                #endif    
            } else subnet = WiFi.subnetMask();

            WiFi.config(ip, gateway, subnet);
            // WiFi.config(ip, WiFi.gatewayIP(), WiFi.subnetMask());
        } else {

            if (isValidIp(gateway)) {
                #ifdef DEBUG
                    Serial.printf("\tassign gateway to: %s \n",  ip2string(gateway).c_str());
                #endif    
            } else gateway = WiFi.gatewayIP();

            if (isValidIp(subnet)) {
                #ifdef DEBUG
                    Serial.printf("\tassign subnet to: %s \n",  ip2string(subnet).c_str());
                #endif    
            } else subnet = WiFi.subnetMask();

            WiFi.config( WiFi.localIP(), gateway, subnet);

        }

        #ifdef DEBUG
            Serial.printf("\n[wifi_connect_configIp] Done: %s - %s - %s\n", 
                WiFi.localIP().toString().c_str(), 
                WiFi.gatewayIP().toString().c_str(), 
                WiFi.subnetMask().toString().c_str());
        #endif            
}


int wfifi_getID_fromSPIFF(String * ret) { 
    String filename = "/wifi_id.txt";
    File f = SPIFFS.open(filename,"r");

    #ifdef DEBUG
        Serial.printf("\n[wfifi_getID_fromSPIFF]\n");
    #endif

    if (!f) {

        #ifdef DEBUG
            Serial.printf("\tERROR read file: %s\n", filename.c_str()); 
            Serial.printf("[wfifi_getID_fromSPIFF] Done\n");
        #endif

        return 0;

    } else {

        String line;

        while (f.available()) {line += char(f.read());}

        f.close();

        *ret = line;

        #ifdef DEBUG
            Serial.printf("\tSUCCES read file: %s\n", filename.c_str());
            // Serial.printf("\t%s\n", line.c_str());
            Serial.printf("[wfifi_getID_fromSPIFF] Done\n");
        #endif  

        return 1;
    }
}

boolean wfifi_retIP(String header, String sIp, IPAddress * retIp) {
    #ifdef DEBUG
    Serial.printf("\n[wfifi_retIP: %s]", header.c_str());
    #endif

    String line;  
    boolean spiffIp = false;


    if (sIp == "") {
        #ifdef DEBUG
        Serial.printf(" ERROR IP not found\n");
        #endif
    }
    else {
        String ip = sIp;
        if (ip != "") {

            IPAddress SpiffIp;
            String a[10];
            int cnt = explode(ip, '.', a);           

            if (cnt<4) {
                spiffIp = false;  
                *retIp = {0,0,0,0};                
            }
            else {
                spiffIp = true;  
                *retIp = {a[0].toInt(),a[1].toInt(),a[2].toInt(),a[3].toInt()}; 
            }

            #ifdef DEBUG
                Serial.printf(" IP: %s\n", ip.c_str());
            #endif                     
        }
    }

    return spiffIp;

}
void wifi_end(){}
void wifi_setup_v2(String hostName){

    wifi_id = new WIFI_ID();
    


    IPAddress ip;
    String  line;
    String mod;
    boolean canSta  = false;

    char buffer[80];
    sprintf(buffer, "%s_", hostName.c_str());
    build_host_name(buffer);

       wifi_id->_ap_psk    = "adrilight1234";
       wifi_id->_ap_ssid   = String(b_host_name);

    int c1 = wfifi_getID_fromSPIFF(&line);
    // Serial.printf("\n1 \tc1: %d - %s\n\n", c1, line.c_str()); 

    if ((c1 == 1) && (line != "")) {
        wifi_id->_ip        = literal_value("wifi_ip",       line );
        wifi_id->_pswd      = literal_value("wifi_psk",      line );
        wifi_id->_ssid      = literal_value("wifi_ssid",     line );
        wifi_id->_ap_ip     = literal_value("wifiAp_ip",     line );
        wifi_id->_subnet    = literal_value("wifi_subnet",   line );
        wifi_id->_gateway   = literal_value("wifi_gateway",  line );

        if (wifi_id->_ap_ip == "") wifi_id->_ap_ip = "192.168.4.1";

        mod = literal_value("wifi_mod", line ); 
        wifi_id->stationMod = wifi_mod_spiffToMod(mod);

        if ((wifi_id->_pswd != "") && (wifi_id->_ssid != "")) canSta = true;

        if (!canSta) wifi_id->stationMod = ADWIFI_AP;

    } else {
        wifi_id->stationMod = ADWIFI_AP;
        wifi_id->ipMod      = ADWIFI_NOIP;
    }


    if(wifi_id->stationMod == ADWIFI_STATION) {

        if (wifi_id->_ip == "") wifi_id->ipMod = ADWIFI_NOIP;
        else {
            wfifi_retIP("local_ip", wifi_id->_ip,   &ip);
            if (!isValidIp(ip))     wifi_id->ipMod = ADWIFI_NOIP;
            else                    wifi_id->ipMod = ADWIFI_STATICIP;
        }
        wifi_statu = wifi_statu_sta_begin;
    }

 
    if (wifi_id->stationMod == ADWIFI_AP)  {
        if (wifi_id->_ap_ip == "") wifi_id->ipMod = ADWIFI_NOIP;
        else {
            wfifi_retIP("AP_ip", wifi_id->_ap_ip,   &ip);
            if (!isValidIp(ip))     wifi_id->ipMod = ADWIFI_NOIP;
            else                    wifi_id->ipMod = ADWIFI_STATICIP;
        }
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        WiFi.softAP(b_host_name, wifi_id->_ap_psk);
        delay(500);

        if (wifi_id->ipMod == ADWIFI_STATICIP) {
            IPAddress ap_subnet(255, 255, 255, 0);
            IPAddress staticIP = string2ip(wifi_id->_ap_ip);
            WiFi.softAPConfig(staticIP, staticIP, ap_subnet);
        }   
        
        wifi_statu = wifi_statu_sta_isconnected;
    }   
}

wifiConnectCallBack * adri_wifi_callback;
boolean wifi_setup_callback = false;
void wifi_setup(String hostName, wifiConnectCallBack * callback) {
    wifi_setup_callback = true;
    adri_wifi_callback = callback;
    wifi_setup(hostName);
}

void wifi_setup(String hostName) {

    wifi_id = new WIFI_ID();


    String  line;
    String mod;
    boolean canSta  = false;
    boolean cfgIp   = false;

    char buffer[180];
    sprintf(buffer, "%s_", hostName.c_str());
    build_host_name(buffer);


    int c1 = wfifi_getID_fromSPIFF(&line);
    // Serial.printf("\n1 \tc1: %d - %s\n\n", c1, line.c_str()); 

    boolean result = false;

    if ((c1 == 1) && (line != "")) {
        wifi_id->_ip        = literal_value("wifi_ip",       line );
        wifi_id->_pswd      = literal_value("wifi_psk",      line );
        wifi_id->_ssid      = literal_value("wifi_ssid",     line );
        wifi_id->_ap_ip     = literal_value("wifiAp_ip",     line );
        wifi_id->_subnet    = literal_value("wifi_subnet",   line );
        wifi_id->_gateway   = literal_value("wifi_gateway",  line );

        if (wifi_id->_ap_ip == "") wifi_id->_ap_ip = "192.168.4.1";

        mod = literal_value("wifi_mod", line ); 
        wifi_id->stationMod = wifi_mod_spiffToMod(mod);

        if ((wifi_id->_pswd != "") && (wifi_id->_ssid != "")) canSta = true;

        if (!canSta) wifi_id->stationMod = ADWIFI_AP;

    } else {
        wifi_id->stationMod = ADWIFI_AP;
        wifi_id->ipMod      = ADWIFI_NOIP;
    }


    if(wifi_id->stationMod == ADWIFI_STATION) {

        if (wifi_id->_ip == "") wifi_id->ipMod = ADWIFI_NOIP;
        else {
            IPAddress ip;
            wfifi_retIP("local_ip", wifi_id->_ip,   &ip);
            if (!isValidIp(ip))     wifi_id->ipMod = ADWIFI_NOIP;
            else                    wifi_id->ipMod = ADWIFI_STATICIP;
        }

        #ifdef DEBUG
            Serial.println("");
            for (int i = 0; i < wifi_id_getValue_count; ++i) {
                Serial.printf("\t%s : %s\n", wifi_id_getValue_list[i].name, wifi_id_getValue_list[i].getValue(wifi_id).c_str());
            }
        #endif

        char sSSID[180];
        char sPSWD[180]; 

        sprintf(sSSID, "%s", wifi_id->_ssid.c_str());
        sprintf(sPSWD, "%s", wifi_id->_pswd.c_str());

        if (wifi_id->ipMod == ADWIFI_STATICIP) {
            IPAddress staticIP; 
            IPAddress gateway;
            IPAddress subnet;  

            staticIP = string2ip(wifi_id->_ip);
            wfifi_retIP("gateway", wifi_id->_gateway,   &gateway);
            wfifi_retIP("subnet", wifi_id->_subnet,     &subnet);
            if (wifi_setup_callback)    result = wifi_connect_sta(sSSID, sPSWD, staticIP, gateway, subnet, adri_wifi_callback);
            else                        result = wifi_connect_sta(sSSID, sPSWD, staticIP, gateway, subnet);

    
        }
        else if (wifi_id->ipMod == ADWIFI_NOIP)  {
            if (wifi_setup_callback)    result = wifi_connect_sta(sSSID, sPSWD, NULL, NULL, NULL, adri_wifi_callback);
            else                        result = wifi_connect_sta(sSSID, sPSWD, NULL, NULL, NULL);            
        }
        if (!result) {
          wifi_id->stationMod == ADWIFI_AP;  
        }
    }

 
    if ((wifi_id->stationMod == ADWIFI_AP) || (!result))  {

        if (wifi_id->_ap_ip == "") wifi_id->ipMod = ADWIFI_NOIP;
        else {
            IPAddress ip;
            wfifi_retIP("local_ip", wifi_id->_ap_ip,   &ip);
            if (!isValidIp(ip))     wifi_id->ipMod = ADWIFI_NOIP;
            else                    wifi_id->ipMod = ADWIFI_STATICIP;
        }

        #ifdef DEBUG
            Serial.println("");
            for (int i = 0; i < wifi_id_getValue_count; ++i) {
                Serial.printf("\t%s : %s\n", wifi_id_getValue_list[i].name, wifi_id_getValue_list[i].getValue(wifi_id).c_str());
            }
        #endif

        char buffer[120];
        sprintf(buffer, "%s_", hostName.c_str());
        build_host_name(buffer);

        String AP_psk = "adrilight1234";

        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        WiFi.softAP(b_host_name, AP_psk);
        delay(500);

        if (wifi_id->ipMod == ADWIFI_STATICIP) {
            IPAddress staticIP; 
            IPAddress subnet(255, 255, 255, 0);
            staticIP = string2ip(wifi_id->_ap_ip);
            WiFi.softAPConfig(staticIP, staticIP, subnet);
        }

        String ap_ssid   = String(b_host_name);
        String ap_ip     = ip2string(WiFi.softAPIP());
        #ifdef DEBUG
            Serial.printf("\n[wfifi_connect_ap] ssid: %s - pswd: %s - ip: %s\n", ap_ssid.c_str(), AP_psk.c_str(), ap_ip.c_str());
        #endif
    }   
}

void wifi_print_networkInfo(String * s){
    // char buffer[1024];
    for (int i = 0; i < wifi_id_getValue_count; ++i) {
        // sprintf(buffer, "\t%s : %s", wifi_id_getValue_list[i].name, wifi_id_getValue_list[i].getValue(wifi_id).c_str());
        String name = ch_toString(wifi_id_getValue_list[i].name);
        *s += info_parm(name, wifi_id_getValue_list[i].getValue(wifi_id)); 
        // *s += String(buffer) + "\n";
    }
    WiFiMode_t w_mod = WiFi.getMode();  
    *s += info_parm("tWiFiMode_t", String(w_mod)); 
}


// xml += literal_item(String(result), value);
WIFI_ID::WIFI_ID(){

}



WIFI_MOD wifi_mod_spiffToMod(String value) {
    if (value == "") return ADWIFI_AP;
    int nbr = value.toInt();
    if (nbr == 1)  return ADWIFI_AP;
    if (nbr == 0)  return ADWIFI_STATION;
     return ADWIFI_AP;
}
String wifi_mod_toSpiff(WIFI_MOD mod) {
    if (mod == ADWIFI_AP)           return "1";
    if (mod == ADWIFI_STATION)      return "0";
    return "1";
}
WIFI_MOD wifi_mod_stringToMod(String mod) {
    if (mod == "ap")    return ADWIFI_AP;
    if (mod == "sta")   return ADWIFI_STATION;
    return ADWIFI_AP;
}
WIFI_MOD wifi_ip_stringToMod(String mod) {
    if (mod == "static_ip") return ADWIFI_STATICIP;
    if (mod == "no_ip")     return ADWIFI_NOIP;
    return ADWIFI_NOIP;
}
String wifi_modToString(WIFI_MOD mod) {
    if (mod == ADWIFI_AP)           return "ap";
    if (mod == ADWIFI_STATION)      return "sta";
    if (mod == ADWIFI_STATICIP)     return "static_ip";
    if (mod == ADWIFI_NOIP)         return "no_ip";
    return "";
}


void wfifi_getID_toSPIFF(){
    #ifdef DEBUG
        Serial.printf("\n[wifi_sav_toSpiff]\n"); 
    #endif
    String filename = "/wifi_id.txt";
    File f = SPIFFS.open(filename,"w");
    if (!f) {  
        #ifdef DEBUG
            Serial.printf(" ERROR read file:/pswd.txt\n"); 
        #endif

        f.close(); 
    }
    else {

        String xml;
        for (int i = 0; i < wifi_id_getValue_count; ++i) {
            char * cName = wifi_id_getValue_list[i].name;
            String value = wifi_id_getValue_list[i].savValue(wifi_id);
            if (wifi_id_getValue_list[i].toSpiff) xml += literal_item(ch_toString(cName), value);
        } 

        f.print(xml);
        f.close();     

        #ifdef DEBUG
            Serial.printf(" Succes write file:/wifi_id.txt\n%s[wifi_sav_toSpiff] Done\n", xml.c_str()); 
        #endif

    }     

}
void wfifi_getID_toSPIFF(String ssid, String pswd, String ip, WIFI_MOD mod){
    #ifdef DEBUG
        Serial.printf("\n[wfifi_getID_toSPIFF]\n"); 
    #endif
    String filename = "/wifi_id.txt";
    File f = SPIFFS.open(filename,"w");
    if (!f) {  
        #ifdef DEBUG
            Serial.printf(" ERROR read file:/wifi_id.txt\n"); 
        #endif

        f.close(); 
    }
    else {

        String xml;
        xml += literal_item("wifi_ssid",    ssid);
        xml += literal_item("wifi_psk",     pswd);
        xml += literal_item("wifi_ip",      ip);
        xml += literal_item("wifi_mod",     wifi_mod_toSpiff(mod));
        f.print(xml);
        f.close();     

        #ifdef DEBUG
            // Serial.printf(" Succes write file:/wifi_id.txt\n%s[wfifi_getID_toSPIFF] Done\n", xml.c_str()); 
            Serial.printf(" Succes write file:/wifi_id.txt [wfifi_getID_toSPIFF] Done\n"); 
        #endif

    }     
}
void wfifi_getID_toSPIFF(String ssid, String pswd, String ip, String ApIp, WIFI_MOD mod){
    #ifdef DEBUG
        Serial.printf("\n[wfifi_getID_toSPIFF]\n"); 
    #endif
    String filename = "/wifi_id.txt";
    File f = SPIFFS.open(filename,"w");
    if (!f) {  
        #ifdef DEBUG
            Serial.printf(" ERROR read file:/wifi_id.txt\n"); 
        #endif

        f.close(); 
    }
    else {

        String xml;
        xml += literal_item("wifi_ssid",    ssid);
        xml += literal_item("wifi_psk",     pswd);
        xml += literal_item("wifi_ip",      ip);
        xml += literal_item("wifiAp_ip",    ApIp);
        xml += literal_item("wifi_mod",     wifi_mod_toSpiff(mod));
        f.print(xml);
        f.close();     

        #ifdef DEBUG
            // Serial.printf(" Succes write file:/wifi_id.txt\n%s[wfifi_getID_toSPIFF] Done\n", xml.c_str()); 
            Serial.printf(" Succes write file:/wifi_id.txt [wfifi_getID_toSPIFF] Done\n"); 
        #endif

    }     
}
void wfifi_getID_toSPIFF(
    String ssid, String pswd, 
    String ip, String ApIp, 
    String wifi_subnet, String wifi_gateway, 
    WIFI_MOD mod){

    #ifdef DEBUG
        Serial.printf("\n[wfifi_getID_toSPIFF]\n"); 
    #endif
    String filename = "/wifi_id.txt";
    File f = SPIFFS.open(filename,"w");
    if (!f) {  
        #ifdef DEBUG
            Serial.printf(" ERROR read file:/wifi_id.txt\n"); 
        #endif

        f.close(); 
    }
    else {

        String xml;
        xml += literal_item("wifi_ssid",    ssid);
        xml += literal_item("wifi_psk",     pswd);
        xml += literal_item("wifi_ip",      ip);
        xml += literal_item("wifiAp_ip",    ApIp);
        xml += literal_item("wifi_subnet",  wifi_subnet);
        xml += literal_item("wifi_gateway", wifi_gateway);
        xml += literal_item("wifi_mod",     wifi_mod_toSpiff(mod));
        f.print(xml);
        f.close();     

        #ifdef DEBUG
            // Serial.printf(" Succes write file:/wifi_id.txt\n%s[wfifi_getID_toSPIFF] Done\n", xml.c_str()); 
            Serial.printf(" Succes write file:/wifi_id.txt [wfifi_getID_toSPIFF] Done\n"); 
        #endif

    }     
}
