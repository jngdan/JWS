# JWS
JWS (Jeedom Watch System) is a ESP module wich is able to monitor any wrong behavior of Jeedom/HA box and able to reset it by a short main power switch off. 

[FR]

Objectifs<br/>
Ce module de surveillance JWS à base d’un microcontrôleur ESP32 permet, en toute autonomie, de détecter des défaillances éventuelles d’une box Jeedom (Atlas, RPi, mini-PC, etc…), et de la redémarrer électriquement jusqu’à deux fois pour tenter de récupérer un fonctionnement normal. 

L’ensemble du matériel s’articule autour :
-	D’un ESP32 standard avec son support 30 broches, 
-	D’un module relais avec contacts NO (Normally Open) et NC (Normally Closed), 
-	D’un écran OLED de type SH1106 ou SSD1306 offrant une résolution de 128 (L) x 64 (H) pixels avec bus I2c (4 broches : GND, VCC, SCL, SDA), 
-	D’une LED RGB (ou 3 LED rouge, verte, et bleue) avec ses 3 résistances de limitation de 220 ohms,
-	De deux prises USB-C type châssis disposant au minimum de 6 contacts (VBUS, GND, D+, D-, CC1, CC2),
-	Un câble USB-C to USB-C mâle coudé (USB-C OUT vers box),
-	D’un boitier,
-	D’une quincaillerie standard (vis, rondelles, et écrous de 1.6, 2.5 et 3mm), câblage, gaine thermo-rétractable.

Logiciel<br/>
Le logiciel est téléversé dans l’ESP32 via l’interface Arduino IDE (ou un autre compilateur) après sa compilation. 
Il pourra par la suite être mis à jour par une procédure OTA (Other The Air), mais le premier téléversement initial devra être effectué directement par la prise USB de l’ESP32.

Principe de fonctionnement<br/>
Le module JWS est auto-alimenté via la prise USB-C qu’il commande, et ne nécessite donc aucune alimentation externe. 
Au démarrage, l’ESP32 va effectuer un autotest des LED (allumage pendant 0,5 secondes des LED rouge, verte et bleue), et vérifier la présence d’un écran OLED. 
Puis il se connecte sur le réseau Wifi sur lequel est présent la box Jeedom (à l’exception du premier lancement, voir chapitre ‘Premier lancement’). 
Toutes les 30 secondes, il va effectuer ensuite les tests suivants :  
-	Ping réseau vers la box Jeedom,
-	Maintien de la connexion MQTT,
-	Emission et attente de la réponse à un message transmit en MQTT.
En cas de détection d’erreur(s), il va signaler celle(s)-ci avec un affichage sur l’écran OLED et les LEDs. 

![image](https://github.com/user-attachments/assets/9ce92a17-9502-4d74-9f6a-3c34c440ea9e)
