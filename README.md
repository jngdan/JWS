# JWS
JWS (Jeedom Watch System) is a ESP module wich is able to monitor any wrong behavior of Jeedom/HA box and able to reset it by a short main power switch off. 

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
-	De la quincaillerie standard (vis, rondelles, et écrous de 1.6, 2.5 et 3mm), du câblage (calibre 24 AWG), de la gaine thermo-rétractable.


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


