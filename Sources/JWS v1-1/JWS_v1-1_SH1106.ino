/*
***************************************************************************************************************************
*                                                                                                                         *
*                          Système de Surveillance et de correction d'avaries pour box jeedom                             *
*                                               Jeedom Watch System (JWS)                                                 *
*                                                                                                                         *
***************************************************************************************************************************
*                                                       Version 1.1                                                       *
***************************************************************************************************************************
*                                                                                                                         *
*      Auteur : Daniel Jung                                                                                               *
*      Développement v1.1 : 11 octobre 2024                                                                               *
*      Code développé pour ESP32 dev module 4 Mo (Generic ESP32 Dev Module)                                               *                                 
*                                                                                                                         *
***************************************************************************************************************************
*                                                                                                                         *
* Historique des versions :                                                                                               *
* v0.1 - 06 Sept 2024 - Version initiale et minimaliste                                                                   *
* v1.0 - 04 Oct  2024 - Implémentation :                                                                                  *
*                          - d'un écran OLED (type SSH1106 ou SSD1306),                                                   *
*                          - des indicateurs d'état d'erreur par diodes LED,                                              *
*                          - de la gestion de la connexion initiale à un réseau Wifi via le mode AP,                      *
*                          - de la sauvegarde des paramètres Wifi et MQTT en EEPROM,                                      *
*                          - des mises à jour par OTA via l'IDE Arduino ou un serveur web.                                * 
* v1.1 - 11 Oct  2024 - Modification de la gestion des LED en mode PWM (nécessite pour la compilation l'installation de   * 
*                        la librairie ESP32 Espressif Systems en version 3.0.2 ou supérieure),                            *
*                      - Améliorations avec prise en charge de la gestion multi-tâches (pour la phase d'initialisation)   * 
*                      - Ajout d'uune barre de progression et des étapes passées lors de l'initialisation au démarrage    *     
*                      - Correction de bugs :                                                                             *
*                           + Comportement si les identifiants Wifi entrés en mode AP sont érronés                        *
*                           + Elimination de l'attente après l'initialisation                                             *
*                                                                                                                         *
***************************************************************************************************************************

Configuration et emploi
-----------------------

1.- Principes de la détection d'une défaillance

Le JWS va effectuer une surveillance à trois niveaux :
- de la présence de la box jeedom sur le réseau IP, en effectuant un ping,
- de la présence du brocker MQTT (Mosquitto), en checkant la connexion MQTT,
- de l'activité de Jeedom, par l'échange de messages MQTT sur le principe interrogation/réponse (ping/pong).
La récurrence des tests effectués est de 30 secondes. Si l'un des résultats remonte une défaillance, le JWS va déclarer une 
anomalie (affichage de KO + LEDs) et réitérer les tests jusqu'à cinq fois, soit un délai de 2'30" accordé à la box Jeedom  
pour corriger un éventuel problème, avant une activation du relais pendant 0,5 secondes provoquant un arrêt/marche.
Après deux activations consécutives, le relais est désactivé tant que le(s) problème(s) ne sera(ont) pas résolu(s) par
ailleurs.

2.- Affichage des informations sur l'écran OLED 

------------------------------------------------------------------------------------------------------------
<Adresse IP de la box jeedom surveillée>                                           Etat de la Connexion Wifi
-----------------------------------------------------------+------------------------------------------------
Connexion IP             OK ou KO   Nb de tests en erreur  |
Connexion MQTT           OK ou KO   Nb de tests en erreur  | Nb d'activations restantes du relais (de 2 à 0)
Echange de messages MQTT OK ou KO   Nb de tests en erreur  |
-----------------------------------------------------------+------------------------------------------------
Numéro de version du firmware JWS                            Temps écoulé depuis le démarrage (H + 1/10 d'h)
------------------------------------------------------------------------------------------------------------

L'affichage ne s'active que dans les cas suivants :
- mise à jour de l'état d'un test (OK -> KO ou KO -> OK),
- mise à jour du nombre de tests en erreur,
- mise à jour du nombre d'activation du relais.

Dans tous les cas, l'affichage reste permanent tant qu'il y a erreur.

3.- Codes pour les diodes LED

-----------
LED Rouge  \
----------------------------------------------------------------------------------------------------------------
Fixe                                             | Clignotante                 | Pulsée
-------------------------------------------------+-----------------------------+--------------------------------
Au démarrage : l'ESP est en mode AP (attente des | Mise à jour OTA :  en cours | Mise à jour OTA : 3x pulsations 
               paramètres)                       |                             | = erreur(s) lors de la MAJ
En fonctionnement : erreur de ping IP            |                             | 
----------------------------------------------------------------------------------------------------------------

-----------
LED Verte  \
----------------------------------------------------------------------------------------------------------------
Fixe                                                         | Pulsée
-------------------------------------------------------------+--------------------------------------------------
En fonctionnement : erreur de connexion MQTT                 | Mise à jour OTA : 3x pulsations = MAJ réussie
----------------------------------------------------------------------------------------------------------------

-----------
LED Bleue  \
----------------------------------------------------------------------------------------------------------------
Fixe                                                         | Pulsée
-------------------------------------------------------------+--------------------------------------------------
En fonctionnement : erreur de messages MQTT                  | 1x toutes les 30" = JWS en service
----------------------------------------------------------------------------------------------------------------

Note : 
Avec une LED RGB, les codes erreurs peuvent être cumulés en une seule couleur distincte : 
- cyan (vert + bleu)          = erreur de connexion MQTT + messages MQTT
- Magenta (rouge + bleu)      = erreur de ping IP + messages MQTT
- Jaune (rouge + vert)        = erreur de ping IP + connexion MQTT 
- Blanc (bleu + rouge + vert) = erreur messages MQTT + ping IP + connexion MQTT  

4.- RAZ complète des paramètres Wifi-MQTT

Relier le GPIO 13 à la masse (GND), et effectuer un reset ou un démarrage dans le même temps. L'ESP32 effectuera une RAZ 
des paramètres enregistrés dans l'EEPROM et entrera en mode Access Point en Wifi pour la redéfinir les paramètres 
suivants :
- Wifi : SSID et mot de passe,
- MQTT : @IP du serveur MQTT, le port (par défaut 1883), l'user et le mot de passe du serveur,
- Box Jeedom : @IP de la box jeedom à surveiller (par défaut l'IP du serveur MQTT).

5.- Mises à jour du logiciel

Ce logiciel supporte les mises à jour via OTA, à la condition qu'il ait été initialement téléversé via port USB.
Les mises à jour sont matérialisées par l'affichage d'un écran spécifique sur l'écran OLED, et la LED rouge qui clignote.
En fin de mise à jour, la LED verte va pulser pour indiquer le bon déroulement de la procédure. Sinon, la LED rouge
s'allume pendant 2 secondes. 

> Avec l'IDE Arduino
Il n'y a pas de mot de passe, mais il faudra toutefois renseigner le champ qui s'affiche avec n'importe quelle valeur. 

> Par le serveur Web
Un serveur web est activé sur l'@IP du JWS pour permettre la mise à jour du firmware en injectant directement un fichier .bin
*/

#include <ESPping.h>                   // Gestion des ping IP
#include <PubSubClient.h>              // Gestion réseau MQTT
#include <WebServer.h>                 // Création d'un portail web pour la gestion du mode AP (Access Point) en Wifi
#include <ArduinoOTA.h>                // Gestion des mises à jour par réseau Wifi OTA (On The Air)
#include <EEPROM.h>                    // Gestion du stockage en EEPROM
#include <WiFi.h>                      // Gestion du WiFi
#include <esp_task_wdt.h>              // Gestion du timeout en multi-tâches

// Configuration des GPIO
#define Broche_RESET   2               // Définition du numéro de GPIO connecté au relais + LED interne
#define SDA_PIN        21              // Signal SDA - OLED  
#define SCL_PIN        22              // Signal SCL - OLED
#define RAZ_EEPROM     13              // RAZ EEPROM si mis à 0 (GND) au démarrage


#define MQTT_cnx_delay 45              // Durée minimale du maintien de la connexion MQTT
#define delai_boucle   30000           // Délai entre chaque boucle de tests (30000 = 30 secondes)
#define Version        "1.1"           // Version logicielle actuelle
#define HOSTNAME       "JWS_ESP32-AP"  // SSID par défaut affiché dans la liste des réseaux Wifi disponibles en mode AP
#define PORT_HOSTNAME  "JWS_ESP32"     // Désignation du port pour les mises à jour par OTA 

// Configuration du WiFi en mode AP
const char *ap_ssid = HOSTNAME;        // SSID du réseau Wifi en mode AP
const char *ap_password = "12345678";  // Mot de passe du réseau Wifi en mode AP (obligatoire mais non utilisé)
WebServer server(80);                  // Créé un serveur sur le port 80
String ssid = "";                      // SSID du réseau Wifi domestique (renseigné via le serveur en mode AP)
String password = "";                  // Mot de passe du réseau Wifi domestique (renseigné via le serveur en mode AP)
bool mode_AP = false;                  // Indique si on est en mode AP ou non

//Page HTML permettant d'entrer les identifiants de connexion Wifi et les paramètres MQTT. 
//@IP de connexion par défaut sur le réseau AP : 192.168.4.1
const char* loginIndex = 
"<form action=\"/connect\" method=\"POST\">"
"SSID : <input type=\"text\" name=\"ssid\"><br><br>"
"Password : <input type=\"password\" name=\"password\"><br><br>"
"MQTT IPServer : <input type=\"text\" name=\"mqtt_server\"><br><br>"
"MQTT User : <input type=\"text\" name=\"mqtt_user\"><br><br>"
"MQTT Password : <input type=\"text\" name=\"mqtt_password\"><br><br>"
"MQTT Port (default = 1883): <input type=\"text\" name=\"mqtt_port\"><br><br>"
"Jeedom IPServer (default = MQTT IPServer) : <input type=\"text\" name=\"jeedom_server\"><br><br>"
"<input type=\"submit\" value=\"Connecter\">"
"</form>";

// Page HTML pour le téléversement du firmware par le serveur web
const char* updatePage = 
"<form method='POST' action='/update' enctype='multipart/form-data'>"
"<input type='file' name='firmware'>"
"<input type='submit' value='Mettre &agrave; jour le firmware'>"
"</form>";

// Configuration MQTT
String mqtt_server = "";               // Adresse du serveur MQTT (normalement la box Jeedom)
String mqtt_user = "";                 // Nom d'utilisateur utilisé par le brocker MQTT
String mqtt_password = "";             // Mot de passe utilisé par le brocker MQTT
int    mqtt_port;                      // Port MQTT défini par l'utilisateur
const int mqtt_port_defaut = 1883;     // Port MQTT par défaut (1883)
const char* ESP_type = "ESP32Client";  // Type client (ESP32)
const char* topic1 = "ping/Ex";        // Topic de publication pour les interrogations MQTT 'Ping' 
const char* topic2 = "ping/Rx";        // Topic de publication pour les réponses MQTT 'Pong'
long lastReconnectAttempt = 0;         // Calcul du délais entre chaque reconnexion MQTT

// Configuration IP
String jeedom_server = "";             // Adresse IP de la box Jeedom sous surveillance

// Variables
int compteur_echecExRx = 0;            // Compteur d'échecs en émission/réception de messages MQTT Ping-Pong
int compteur_echecMQTT = 0;            // Compteur d'échecs de connexions MQTT
int compteur_echecIP = 0;              // Compteur d'échecs de connexions IP
int compteur_relais = 2;               // Compteur du nombre d'activation max du relais
long lastloop = 0;                     // Calcul du délai entre chaque boucle
bool boucle = true;                    // Indicateur d'attente d'éxecution des instructions dans la boucle principale 
int Avarie = 0;                        // Type d'avarie : 0= OK, 1= défaut connexion IP, 2= défaut connexion MQTT,  
                                       // 3= 1 et 2, 4= défaut échange messages MQTT, 5= 1 et 4, 6= 2 et 4, 7= 1 et 2 et 4
bool erreur_OTA = false;               // Erreur lors de la mise à jour en OTA
bool arret_boucle = false;             // Indicateur d'arret effectué
const int nb_essais = 5;               // Nombre d'essais maximum avant un Arrêt/Marche
volatile int init_progress = 0;        // Progression de l'initialisation
const TickType_t xDelay = (10 / portTICK_PERIOD_MS); // Délai non bloquant pour le fonctionnement en multi-tâche
esp_err_t ESP32_ERROR;                 // Erreur dans l'initialisation multi-coeur

// Paramètres pour la gestion des LED (frequence PWM, résolution PWM, seuils, état)
const int LED_PINGIP = 12;             // LED Rouge (fixe = erreur Ping IP, clignotante = MAJ en cours, pulse = MAJ KO)
const int LED_CNXMQTT = 14;            // LED Verte (fixe = erreur connexion MQTT, pulse = MAJ OK) 
const int LED_MSGMQTT = 27;            // LED Bleue (fixe = erreur messages MQTT, flash unique toutes les 30" = JWS en service) 
const int freq = 1000;                 // Fréquence PWM à 1000 Hz
const int resolution = 8;              // Résolution à 8 bits (plage de 0 à 255)
const int LED_OFF = 255;               // Codage de la valeur pour éteindre les LEDs de type Anode Commune (AC)
const int LED_ON = 0;                  // Codage de la valeur pour allumer à 100% les LEDs de type Anode Commune (AC)
int LED_lummax = 200;                  // Luminosité maximale des LED (255 = éteinte, 0 = maximale)
bool ledState = 1;                     // Etat initial des LED (éteintes)
unsigned long previousMillis = 0;      // Gestion du clignotement des LED
const long interval = 50;              // Intervalle de clignotement des LED (en ms)

// Mises à jour OTA
const char* otaHostname = PORT_HOSTNAME;// Nom affiché pour la sélection du port OTA

// Étapes de l'initialisation
const String steps[] = {
    "OLED",                            // Step 0 = Initialisation de l'écran OLED
    "LED",                             // Step 1 = Auto-test de la LED RGB 
    "EEPROM",                          // Step 2 = Initialisation de l'EEPROM
    "RAZ",                             // Step 3 = RAZ de l'EEPROM si le shunt est en place
    "Param",                           // Step 4 = Lecture des paramètres dans l'EEPROM
    "Wifi",                            // Step 5 = Connexion au réseau Wifi
    "OTA",                             // Step 6 = Initialisation des MAJ par OTA
    "MQTT",                            // Step 7 = Initialisation des échanges MQTT
    "GO"                               // Step 8 = Terminé
  };

// Initialisation des objets WiFi et MQTT
IPAddress remote_ip;                   // Adresse IP de l'équipement à pinger
WiFiClient espClient;
PubSubClient client(espClient);

// ----------------------------------------------------------------------
// Arrêt/marche du relais
// ----------------------------------------------------------------------

void arret() {

  if (compteur_relais > 0) {           // S'il y a déjà eu deux redémarrages consécutifs sans succès, il n'y a plus de 
                                       // tentatives d'arrêt/marche par la suite.  
   compteur_relais--;
   Affichage_RESET();
   digitalWrite (Broche_RESET, HIGH) ; 
   delay (500) ;                       // Action ON pendant 500ms sur le relais d'alimentation provoquant l'arrêt (box 
                                       // Jeedom alimentée par les broches NC et COM du relais)
   digitalWrite (Broche_RESET, LOW) ;
   arret_boucle = true;                // Indique qu'un arrêt/marche vient d'être effectué
   compteur_echecIP = 0;               // RAZ des compteurs d'échecs
   compteur_echecExRx = 0;
   compteur_echecMQTT = 0;
   Extinction_LED();                   // Extinction des LEDs d'alerte
  }
}

// ----------------------------------------------------------------------
// Extinction des LEDs
// ----------------------------------------------------------------------

void Extinction_LED(){

  ledcWrite(LED_PINGIP, LED_OFF);      // Extinction les LEDs 
  ledcWrite(LED_CNXMQTT, LED_OFF);
  ledcWrite(LED_MSGMQTT, LED_OFF);
}

// ----------------------------------------------------------------------
// Auto-test des LEDs
// ----------------------------------------------------------------------

void Test_LED(void *pvParameters){     // Auto-test des LED : allume de façon séquentielle et progressive pendant 1 sec 
                                       // les LEDs rouge, verte, et bleue
  Extinction_LED();
  vTaskDelay(10 / portTICK_PERIOD_MS);
  ledcFade(LED_PINGIP, LED_OFF, LED_ON, 500);// Test de la LED Rouge
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcFade(LED_PINGIP, LED_ON, LED_OFF , 500);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcFade(LED_CNXMQTT, LED_OFF, LED_ON, 500);// Test de la LED Verte
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcFade(LED_CNXMQTT, LED_ON, LED_OFF , 500);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcFade(LED_MSGMQTT, LED_OFF, LED_ON, 500);// Test de la LED Bleue
  vTaskDelay(500 / portTICK_PERIOD_MS);
  ledcFade(LED_MSGMQTT, LED_ON, LED_OFF , 500);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  vTaskDelete(NULL);                   // Supprime la tâche une fois exécutée
}

// -------------------------------------------------------------------------------------------
// Incrémente de façon sécurisée (multi-tâche) l'indicateur de progression de l'initialisation
// -------------------------------------------------------------------------------------------

void Increment() {
  int a;
  a=init_progress;
  a++;
  init_progress = a;
}

// ----------------------------------------------------------------------
// Initialisation
// ----------------------------------------------------------------------

void setup() {

  Serial.begin(115200);

  // Initialisation des GPIO, des LEDs, de l'écran OLED, de l'EEPROM
  pinMode (Broche_RESET, OUTPUT);      // Initialisation du GPIO connecté au relais
  pinMode (RAZ_EEPROM, INPUT_PULLUP);  // Initialisation de la broche RAZ_EEPROM au démarrage
  ledcAttachChannel(LED_PINGIP, freq, resolution, 0);  // Initialisation du GPIO connecté à la LED Rouge (ping IP)
  ledcAttachChannel(LED_CNXMQTT, freq, resolution, 1); // Initialisation du GPIO connecté à la LED Verte (connexion MQTT)
  ledcAttachChannel(LED_MSGMQTT, freq, resolution, 2); // Initialisation du GPIO connecté à la LED Bleue (messages MQTT)
  
  // Initialisation du watchdog WDT pour le fonctionnement en multi-tâche
  esp_task_wdt_deinit();                
  esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 120000,           // Timeout pour les tâches fixé à 2 munites max
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Bitmask pour les cores
        .trigger_panic = true          // Active le RESET si timeout
  };
  ESP32_ERROR = esp_task_wdt_init(&wdt_config); // Initialisation WDT
  Serial.println("Dernier Reset : " + String(esp_err_to_name(ESP32_ERROR)));
  esp_task_wdt_add(NULL);              // Ajout de la tâche en cours dans la liste des tâches surveillées du WDT
  esp_task_wdt_reset();                // RESET du watchdog
  delay(10);                           // Délai obligatoire après un RESET WDT

  // Initialisation de l'écran OLED
  xTaskCreate(InitOLED, "InitOLED", 2048, NULL, 2, NULL); // Tâche en parallèle

  // Auto-test des LEDs
  xTaskCreate(Test_LED, "Test_LED", 2048, NULL, 2, NULL); // Tâche en parallèle
  Increment();                         // 1 = test LED effectué
  delay(750);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation

  // Initialisation de l'EEPROM
  INIT_EEPROM();
  Increment();                         // 2 = Initialisation de EEPROM effectué
  delay(750);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation
  
  // RAZ complète de l'ESP32
  if (digitalRead(RAZ_EEPROM) == LOW) {EEPROM_ERASE();} // Si la broche RAZ-EEPROM est activée au démarrage (mise à la 
                                                        // masse), il y a une réinitialisation complète du module ESP par
                                                        // effacement des paramètres enregistrés dans l'EEPROM
  Increment();                         // 3 = RAZ de EEPROM effectué
  delay(250);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation

  // Initialisation des paramètres Wifi et MQTT
  lecture_parametres();                // Lit les paramètres Wifi et MQTT sauvegardés dans l'EEPROM
  Increment();                         // 4 = Lecture des paramètres de l'EEPROM effectuée
  delay(250);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation

  // Initialisation du Wifi
  tryConnect();                        // Connexion au réseau Wifi
  Increment();                         // 5 = Connexion Wifi effectuée
  delay(500);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation

  // Initialisation des mises à jour OTA
  setupOTA();                          // Démarrage des mises à jour par OTA via l'IDE Arduino 
  Increment();                         // 6 = Initialisation des mises à jour OTA effectuée
  delay(50);                           // Délai arbitraire pour simuler le déroulement des étapes d'initialisation

  // Initialisation du réseau MQTT
  client.setServer(mqtt_server.c_str(), mqtt_port); // Initialisation du serveur et du port MQTT
  client.setCallback(callback);
  client.setKeepAlive(MQTT_cnx_delay); // Défini le délai minimal de maintien de la connexion MQTT
  Increment();                         // 7 = Initialisation MQTT effectuée
  delay(100);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation
  Increment();                         // 8 = Initialisation terminée
  delay(100);                          // Délai arbitraire pour simuler le déroulement des étapes d'initialisation
  esp_task_wdt_reset();                // RESET du watchdog
  delay(10);                           // Délai obligatoire après un RESET WDT
}

// ----------------------------------------------------------------------
// Boucle principale
// ----------------------------------------------------------------------

void loop() {

  server.handleClient();
  if (!mode_AP) {                      // En mode AP, les tests ne sont pas effectués
    ArduinoOTA.handle();               // Permet de gérer les mises à jour OTA par l'IDE Arduino en arrière-plan

    // ----------------------------------------------------------------------
    // Test des échanges de messages MQTT en background
    // ----------------------------------------------------------------------
 
    client.loop();                     // Attente de la réponse par message MQTT

    if (boucle) {                      // Si on est sorti du délai entre chaque itérations, on lance les tests.
    
      init_progress = 9;               // Indique la fin de la phase d'initilisation
    
      // ----------------------------------------------------------------------
      // Test de la connexion MQTT
      // ----------------------------------------------------------------------
      connexion_MQTT();
  
      // ----------------------------------------------------------------------
      // Test de la connexion avec le réseau IP
      // ----------------------------------------------------------------------
      if (remote_ip.fromString(jeedom_server)) {
        if (Ping.ping(remote_ip)) {    // Envoi d'un ping réseau      
            compteur_echecIP = 0;
            ledcWrite(LED_PINGIP, LED_OFF);// Eteint la LED d'erreur de ping IP 
            if (Avarie == 1 || Avarie == 3 || Avarie == 5 || Avarie == 7){Avarie--;} // Mise à jour de l'indicateur d'avarie
            OledAffiche();             // Affichage des informations sur l'écran OLED 
        } 
       else {
            compteur_echecIP++;
            if (compteur_echecIP == 1) {
              ledcFade(LED_PINGIP, LED_OFF, LED_lummax, 500); // Signale l'erreur de ping IP avec la LED dédiée
              delay(500);
            }
            if (Avarie == 0 || Avarie == 2 || Avarie == 4 || Avarie == 6){Avarie++;} // Mise à jour de l'indicateur d'avarie
            OledAffiche();             // Affichage des informations sur l'écran OLED 
            if (compteur_echecIP > nb_essais) { // Après (nb_essais) tentatives de connexion consécutives en échec, le RPi va 
                                                // être arrêté électriquement et relancé. 
              if (!arret_boucle){arret();} // Effectue un arrêt/marche uniquement s'il n'y en a pas déjà eu un dans la même 
                                           // période de tests
            }
       }
      }
    if (Avarie == 0) {compteur_relais = 2;} // Si tous les tests sont OK, c'est que la box jeedom est repassée opérationnelle
                                            // après intervention extérieure. Le compteur des redémarrages est remis à zéro. 
    }

    // Attente (non bloquante) de l'écoulement du délai prévu entre chaque boucles de tests
    long loop_now = millis();
    if (loop_now - lastloop > delai_boucle) {
        lastloop = loop_now;
        boucle = true;                 // Indique que le délai depuis l'exécution de la dernière boucle est écoulé.
      }
      else {
        boucle = false;                // Indique qu'on est encore dans le délai d'attente.
        arret_boucle = false;          // Remise à zéro de l'indicateur d'arrêt/marche
      }
    }
  esp_task_wdt_reset();                // RESET du watchdog
  delay(10);                           // Délai obligatoire après un RESET WDT
  vTaskDelay(xDelay);                  // Délai non bloquant obligatoire pour permettre un fonctionnement en multi-tâche 
}
