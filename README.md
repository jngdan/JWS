# JWS
JWS (Jeedom Watch System) is an ESP based module wich is able to monitor any wrong behavior of Jeedom/HA box and able to reset it by a short main power switch off. 

![image](https://github.com/user-attachments/assets/9ce92a17-9502-4d74-9f6a-3c34c440ea9e)

[FR]


Présentation générale <br/>

Le module de surveillance JWS s’articule autour d’un microcontrôleur ESP32 et permet, en toute autonomie, de détecter des défaillances éventuelles d’un système domotique (Jeedom, Home Assistant, ou autre) hébergé sur une box autonome (type Atlas, Luna, RPi, mini-PC, etc…). 
La RAZ étant effectuée par un redémarrage électrique (arrêt/marche), ce module ne convient pas pour des systèmes domotiques virtuels hébergés sur des NAS ou autres serveurs assurant d’autres fonctions.  
Les défaillances sont détectées au niveau applicatif (Jeedom/HA via protocole MQTT) et réseau (IP), et le module va redémarrer électriquement jusqu’à deux fois la box domotique pour tenter de récupérer un fonctionnement normal.
Il se connecte d’une part sur une alimentation 5v par un câble USB-C mâle (15W max), et d’autre part sur la box domotique également avec un câble USB-C mâle. Il s’auto-alimente directement via la prise USB-C qu’il commande, et ne nécessite donc aucune alimentation externe.  

L’ensemble du matériel s’articule autour :
-	D’un ESP32 4Mo/240Mhz/Wifi standard 30 broches, avec son support, 
-	D’un module relais avec contacts NO (Normally Open) et NC (Normally Closed), 
-	D’un écran OLED de type SH1106 ou SSD1306 offrant une résolution de 128 (L) x 64 (H) pixels avec interface I2c (4 broches : GND, VCC, SCL, SDA), 
-	D’une LED RGB (ou 3 LED rouge, verte, et bleue) avec ses 3 résistances de limitation de 180 à 270 ohms,
-	De deux prises USB-C type châssis disposant au minimum de 6 contacts (VBUS, GND, D+, D-, CC1, CC2),
-	D’un câble USB-C to USB-C mâle (prise USB-C OUT vers la box),
-	D’un boitier (dimensions intérieures minimales : L120 x l75 x h27 mm),
-	D’une feuille acrylique translucide fumée de 75x35x1mm (cache pour l’écran OLED),
-	De la quincaillerie standard (vis, rondelles, et écrous de 1.6, 2.5 et 3mm), du câblage (calibre 22 AWG), de la gaine thermo-rétractable.

Principe de fonctionnement<br/>

Au démarrage, l’ESP32 va effectuer un autotest des LED (allumage successif pendant 0,5 secondes des LED rouge, verte et bleue), et vérifier la présence d’un écran OLED et afficher le cas échéant un logo. 
Puis il se connecte sur le même réseau Wifi que celui utilisé par la box domotique, à l’exception du premier lancement. 
En phase de surveillance, il va ensuite effectuer toutes les 30 secondes l’ensemble des tests suivants :  
-	Ping réseau vers la box domotique.<br> 
Ce ping permet de s’assurer que la box domotique est toujours connectée au réseau, et qu’elle n’est pas figée ou plantée.  
-	Maintien de la connexion MQTT.<br>
Une connexion MQTT doit être activée et gérée par un serveur MQTT (brocker Mosquitto,…), qui se signale via la messagerie associée. L’absence de ce signalement peut résulter d’un arrêt logiciel du serveur. Généralement, les box domotiques s’appuient sur ce protocole pour gérer les modules présents sur le réseau Zigbee ou les modules connectés en Wifi. Il n’est donc pas nécessaire dans la plupart des cas de prévoir l’installation et l’activation spécifique d’un plugin de gestion de ce protocole.   
-	Emission et attente de la réponse à un message transmit en MQTT.<br>
Outre la vérification de la présence d’un brocker, le module JWS va émettre un message MQTT spécifique (ping) et attendre en retour la réponse de la box domotique (pong). S’il n’y a pas de réponse, il est possible que le démon du plugin se soit arrêté.
<br>
Dans tous les cas, toute(s) détection(s) d’erreur(s) seront signalées par un affichage d’un écran récapitulatif sur l’écran OLED et l’allumage des LEDs correspondantes.
<br/><br/>
[UK]
<br/>
<br/>
General<br/>

The JWS monitoring module is an ESP32 based microcontroller and allows, in complete autonomy, detection of possible failures of a home automation system (like Jeedom, Home Assistant, or any other system) hosted on an autonomous box (Atlas, Luna , RPi, mini-PC, etc…). 
Since the reset is carried out by an electrical restart (off/on), this module is not suitable for virtual home automation systems hosted on NAS or servers providing other functions.
Failures are detected at the application level (Jeedom/HA via MQTT protocol) and network level (IP), and in this case module will electrically restart the home automation box up to two times in order to try to recover normal operation. 
It connects in one hand to a 5v power supply via a male USB-C cable (15W max), and in other hand to the home automation box also with a male USB-C cable. It is self-powered directly via the USB-C socket it controls, and therefore does not require any external power supply.

Material needed :
- A standard 30-pin ESP32 4MB/240Mhz/Wifi, with its dedicated support, 
- A relay module with NO (Normally Open) and NC (Normally Closed) contacts, 
- A SH1106 or SSD1306 type OLED screen offering a resolution of 128 (W) x 64 (H) pixels, with I2c interface (4 pins: GND, VCC, SCL, SDA), 
- A RGB LED (can be replaced by 3 red, green and blue LEDs) with its 3 limiting resistors from 180 to 270 ohms,
- Two chassis type USB-C sockets with at least 6 contacts (VBUS, GND, D+, D-, CC1, CC2),
- A male USB-C to USB-C cable (USB-C OUT socket to the box),
- A case (inside dimensions must be at least L120 x W75 x H27 mm),
- A translucent smoked acrylic sheet L75 x W35 x H1mm (cover for the OLED screen),
- Standard hardware (1.6, 2.5 and 3mm screws, washers, and nuts), wiring (22 AWG gauge), heat shrink tubing.

Operations <br>

When starting, ESP32 will perform a self-test of the LEDs (successive lighting for 0.5 seconds of the red, green and blue LEDs), and check the availibity of an OLED screen, and display a logo if true. 
Then it connects to the same WiFi network as that used by the home automation box, with the exception of the first launch. 
In the monitoring phase, it will then perform all of the following tests every 30 seconds :  
- Network ping to the home automation box. 
This ping ensures that the home automation box is still connected to the network, and that it is not frozen or crashed.
- Maintaining the MQTT connection.
An MQTT connection must be activated and managed by an MQTT server (Mosquitto brocker, etc...), which is reported via the associated messaging. The absence of this signal may result from a software shutdown of the server. Generally, home automation boxes rely on this protocol to manage modules present on the Zigbee network or modules connected via Wifi. It is therefore not necessary in most cases to provide for the installation and specific activation of a plugin for managing this protocol.
- Sending and waiting for the response to a message transmitted in MQTT.<br>
In addition to checking the presence of a brocker, the JWS module will send a specific MQTT message (ping) and wait in return for a response from the home automation box (pong). If there is no response, the plugin daemon may have stopped.
<br>
In all cases, all failures detection will be highlighted with a display of a summary screen on the OLED screen, and lighting of the corresponding LEDs.
