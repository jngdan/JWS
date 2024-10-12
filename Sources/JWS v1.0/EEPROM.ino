// **********************************************************************
// Gestion des données stockées en EEPROM : lecture, écriture, effacement
// **********************************************************************

// ----------------------------------------------------------------------
// Plan de stockage des paramètres dans l'EEPROM
// ----------------------------------------------------------------------
#define EEPROM_SIZE  256               // Utilisation de 256 octets sur les 4Mb de l'ESP32         
#define SSID_ADDR      0               // Adresse de départ du SSID dans l'EEPROM (32 caractères)
#define PASS_ADDR     32               // Adresse de départ du mot de passe dans l'EEPROM (32 caractères)
#define SERVER_ADDR   64               // Adresse de départ de l'@IP du server MQTT dans l'EEPROM (16 caractères)
#define JEEDOM_ADDR   80               // Adresse de départ de l'@IP de la box Jeedom dans l'EEPROM (16 caractères)
#define USER_ADDR     96               // Adresse de départ du nom d'user MQTT dans l'EEPROM (32 caractères)
#define MQTTPASS_ADDR 128              // Adresse de départ du mot de passe MQTT dans l'EEPROM (80 caractères)
#define PORT_ADDR     208              // Adresse de départ du port MQTT dans l'EEPROM (32 octets)

// ----------------------------------------------------------------------
// Initialisation de l'EEPROM
// ----------------------------------------------------------------------
void INIT_EEPROM(void) {

  if (!EEPROM.begin(EEPROM_SIZE)) {
    return;
  }
}

// ----------------------------------------------------------------------
// Lecture d'un nombre entier depuis l'EEPROM
// ----------------------------------------------------------------------
int readIntFromEEPROM(int address) {

  int value = 0;
  byte* valuePtr = (byte*) &value;
  for (int i = 0; i < sizeof(value); i++) {
    valuePtr[i] = EEPROM.read(address + i);
  }
  return value;
}

// ----------------------------------------------------------------------------------------------------------
// Effacement de l'ensemble des paramètres stockés dans l'EEPROM si l'ESP doit être complètement réinitialisé
// ----------------------------------------------------------------------------------------------------------
void EEPROM_ERASE() {

  // Efface toutes les adresses de l'EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);                // Écrit la valeur 0 à chaque adresse
  }
  EEPROM.commit();                     // Sauvegarde des modifications dans l'EEPROM
}

// ----------------------------------------------------------------------
// Lecture des paramètres Wifi et MQTT de l'EEPROM
// ----------------------------------------------------------------------
void lecture_parametres() {
  char ssid_eeprom[32];
  char pass_eeprom[32];
  char server_eeprom[16];
  char user_eeprom[32];
  char mqttpass_eeprom[80];
  char jeedom_eeprom[16];

  // Lecture du SSID 
  for (int i = 0; i < 32; i++) {
    ssid_eeprom[i] = EEPROM.read(SSID_ADDR + i);
  }
   // Lecture du le mot de passe Wifi
  for (int i = 0; i < 32; i++) {
    pass_eeprom[i] = EEPROM.read(PASS_ADDR + i);
  }
  // Lecture de l'@IP du serveur MQTT
  for (int i = 0; i < 15; i++) {
    server_eeprom[i] = EEPROM.read(SERVER_ADDR + i);
  }
  // Lecture du nom d'user MQTT
  for (int i = 0; i < 32; i++) {
    user_eeprom[i] = EEPROM.read(USER_ADDR + i);
  }
  // Lecture du mot de passe MQTT
  for (int i = 0; i < 80; i++) {
    mqttpass_eeprom[i] = EEPROM.read(MQTTPASS_ADDR + i);
  }
  // Lecture de l'@IP de la box Jeedom
  for (int i = 0; i < 15; i++) {
    jeedom_eeprom[i] = EEPROM.read(JEEDOM_ADDR + i);
  }

  ssid = String(ssid_eeprom);
  password = String(pass_eeprom);
  mqtt_server = String(server_eeprom);
  mqtt_user = String(user_eeprom);
  mqtt_password = String(mqttpass_eeprom);
  mqtt_port = readIntFromEEPROM(PORT_ADDR); // Lecture du port MQTT
  jeedom_server = String(jeedom_eeprom);

  //Elimination des caractères d'espace en début et fin de chaîne
  ssid.trim();
  password.trim();
  mqtt_server.trim();
  mqtt_user.trim();
  mqtt_password.trim();
  jeedom_server.trim();
}

// ----------------------------------------------------------------------
// Ecriture des paramètres Wifi et MQTT dans l'EEPROM
// ----------------------------------------------------------------------
void sauvegarde_parametres(String ssid, String password, String mqtt_server, String mqtt_user, String mqtt_password, int mqtt_port, String jeedom_server) {

  // Sauvegarde du SSID
  for (int i = 0; i < 32; i++) {
    EEPROM.write(SSID_ADDR + i, i < ssid.length() ? ssid[i] : 0);
  }
  // Sauvegarde du mot de passe Wifi
  for (int i = 0; i < 32; i++) {
    EEPROM.write(PASS_ADDR + i, i < password.length() ? password[i] : 0);
  }
  // Sauvegarde l'adresse IP du serveur MQTT
  for (int i = 0; i < 15; i++) {
    EEPROM.write(SERVER_ADDR + i, i < mqtt_server.length() ? mqtt_server[i] : 0);
  }
    // Sauvegarde du nom d'user MQTT
  for (int i = 0; i < 32; i++) {
    EEPROM.write(USER_ADDR + i, i < mqtt_user.length() ? mqtt_user[i] : 0);
  }
    // Sauvegarde du mot de passe MQTT
  for (int i = 0; i < 80; i++) {
    EEPROM.write(MQTTPASS_ADDR + i, i < mqtt_password.length() ? mqtt_password[i] : 0);
  }
    // Sauvegarde du numéro de port MQTT
  byte* valuePtr = (byte*) &mqtt_port;
  for (int i = 0; i < sizeof(mqtt_port); i++) {
    EEPROM.write(PORT_ADDR + i, valuePtr[i]);
  } 
    // Sauvegarde de l'adresse IP de la box Jeedom
  for (int i = 0; i < 15; i++) {
    EEPROM.write(JEEDOM_ADDR + i, i < jeedom_server.length() ? jeedom_server[i] : 0);
  }
  EEPROM.commit(); // Sauvegarde des paramètres dans l'EEPROM
}

