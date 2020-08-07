// #include <ESP8266WiFi.h>

#ifndef ADRI_WIFI_H
    #define ADRI_WIFI_H
    #ifdef ESP32
        #include <WiFi.h>
    #else
        #include <ESP8266WiFi.h>
        #include <ESP8266mDNS.h>
    #endif
               
    enum WIFI_MOD { 
        ADWIFI_STATION, 
        ADWIFI_AP, 
        ADWIFI_STATICIP, 
        ADWIFI_NOIP
    };
    enum WIFI_STATU { 
        wifi_statu_sta_begin, 
        wifi_statu_sta_search, 
        wifi_statu_sta_isconnected, 
        wifi_statu_sta_fail
    };
    
    extern WIFI_STATU wifi_statu;

    typedef void (*wifiConnectCallBack)(int x);

    class WIFI_ID {
        public:   
            String  _subnet;
            String  _gateway;
            String  _ip;
            String  _pswd;
            String  _ssid;
            String  _ap_ip;
            String  _ap_psk;
            String  _ap_ssid;
            WIFI_MOD    stationMod;
            WIFI_MOD    ipMod;

            WIFI_ID();

    };
    extern WIFI_ID * wifi_id;

    struct wifi_id_getValue {
        char * name;
        String (*getValue)(WIFI_ID * wifi);
        int mod;
        bool toSpiff;
        String (*savValue)(WIFI_ID * wifi);
       
    };
// struct wifi_id { // 1AP
//     String  subnet;
//     String  gateway;
//     String  ip;
//     String  pswd;
//     String  ssid;
//     String  ap_ip;
//     String  ap_psk;
//     String  ap_ssid;
//     int     mod;
//     int     noip;
// };
void wfifi_getID_toSPIFF();  
void wfifi_getID_toSPIFF(String ssid, String pswd, String ip, WIFI_MOD mod);  
void wfifi_getID_toSPIFF(String ssid, String pswd, String ip, String ApIp, WIFI_MOD mod);
void wfifi_getID_toSPIFF(
    String ssid, String pswd, 
    String ip, String ApIp, 
    String wifi_subnet, String wifi_gateway, 
    WIFI_MOD mod);
void wifi_print_networkInfo(String * s);    
void wifi_loop();    
void wifi_end();    
void wifi_setup_v2(String hostName);    
boolean wfifi_retIP(String header, String sIp, IPAddress * retIp) ;    
void wifi_connect_configIp(IPAddress ip, IPAddress gateway, IPAddress subnet);    
boolean wifi_connect_sta(char* ssid, char* password, IPAddress staticIP, IPAddress gateway, IPAddress subnet);
void wifi_connect_sta(boolean * result);
void wifi_setup(String hostName);
void wifi_setup(String hostName, wifiConnectCallBack * callBack);    

String network_info(String hostName) ;
WIFI_MOD wifi_mod_stringToMod(String mod) ;
WIFI_MOD wifi_ip_stringToMod(String mod) ;
String wifi_modToString(WIFI_MOD mod) ;
WIFI_MOD wifi_mod_spiffToMod(String value) ;
String wifi_mod_toSpiff(WIFI_MOD mod) ;
boolean isValidIp(IPAddress ip);

String wifi_id_get_subnet(WIFI_ID * wifi)     ;
String wifi_id_get_gateway(WIFI_ID * wifi)    ;
String wifi_id_get_ip(WIFI_ID * wifi)         ;
String wifi_id_get_pswd(WIFI_ID * wifi)       ;
String wifi_id_get_ssid(WIFI_ID * wifi)       ;
String wifi_id_get_ap_ip(WIFI_ID * wifi)      ;
String wifi_id_get_ap_psk(WIFI_ID * wifi)     ;
String wifi_id_get_ap_ssid(WIFI_ID * wifi)    ;
String wifi_id_get_mod(WIFI_ID * wifi)        ;
String wifi_id_get_noip(WIFI_ID * wifi)       ;



void build_host_name(char * hostnamePrefix) ;

extern char b_host_name[];;
#endif

