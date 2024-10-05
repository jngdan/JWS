// ***********************************************************
// Gestion de la connexion au réseau MQTT et Ex/Rx de messages
// ***********************************************************


// Reconnexion au serveur MQTT (non bloquante)
boolean reconnect() {
  
  if (client.connect(ESP_type, mqtt_user.c_str(), mqtt_password.c_str())) {
  }
  return client.connected();
}

// Emission d'un ping MQTT
void ping_jeedom() {

 client.subscribe(topic1);             // Abonnement au topic des interrogations
 client.publish(topic1, "ping");
 client.subscribe(topic2);             // Retour sur le topic des réponses
} 

// ----------------------------------------------------------------------
// Test de la connexion MQTT : lien réseau MQTT 
// ----------------------------------------------------------------------

void connexion_MQTT() {

  if (!client.connected()) {           // Reconnexion MQTT si nécessaire
      compteur_echecMQTT++;
      digitalWrite (LED_CNXMQTT, LOW); // Signale l'erreur de connexion MQTT avec la LED dédiée
      if (Avarie == 0 || Avarie == 1 || Avarie == 4 || Avarie == 5){Avarie = Avarie + 2;} // Mise à jour de l'indicateur
                                                                                          // d'avarie 
      OledAffiche();                   // Affichage des informations sur l'écran OLED 
      if (compteur_echecMQTT > nb_essais) { // Après (nb_essais) tentatives de connexion consécutives en échec, le RPi va
                                            // être arrêté électriquement et relancé. 
       if (!arret_boucle){arret();}    // Effectue un arrêt/marche uniquement s'il n'y en a pas déjà eu un dans la même 
                                       // période de tests
      }
      long now = millis();
      if (now - lastReconnectAttempt > 5000) {
       lastReconnectAttempt = now;
       // Tentative de reconnexion
        if (reconnect()) {             // Connexion MQTT OK
          lastReconnectAttempt = 0;
          compteur_echecMQTT = 0;
          digitalWrite (LED_CNXMQTT, HIGH); // Eteint la LED d'erreur de connexion MQTT 
          }
      }
    } else {
      compteur_echecMQTT = 0;
      if (Avarie == 2 || Avarie == 3 || Avarie == 6 || Avarie == 7){Avarie = Avarie - 2;} // Mise à jour de l'indicateur
                                                                                          // d'avarie 
      OledAffiche();                   // Affichage des informations sur l'écran OLED 
      ping_jeedom();                   // Test d'émission d'un message MQTT si le client est connecté
    }
}  

// ----------------------------------------------------------------------
// Test d'échange de messages MQTT
// ----------------------------------------------------------------------

void callback(char* topic, byte* payload, unsigned int length) {

  if ((char)payload[1] == 'o') {
    compteur_echecExRx = 0;
    digitalWrite (LED_MSGMQTT, HIGH);  // Eteint la LED d'erreur de message MQTT
    if (Avarie == 4 || Avarie == 5 || Avarie == 6 || Avarie == 7){Avarie = Avarie - 4;} // Mise à jour de l'indicateur
                                                                                        // d'avarie 
    OledAffiche();                     // Affichage des informations sur l'écran OLED 
    }
    else
    { compteur_echecExRx++;
      digitalWrite (LED_MSGMQTT, LOW); // Signale l'erreur de message MQTT avec la LED dédiée
      if (Avarie == 0 || Avarie == 1 || Avarie == 2 || Avarie == 3){Avarie = Avarie + 4;} // Mise à jour de l'indicateur 
                                                                                          // d'avarie 
      OledAffiche();                   // Affichage des informations sur l'écran OLED 
      if (compteur_echecExRx > nb_essais){ // Après (nb_essais) tentatives consécutives de reconnaissance du message reçu
                                           // en échec, le RPi va être arrêté électriquement et relancé 
       if (!arret_boucle){arret();}    // Effectue un arrêt/marche uniquement s'il n'y en a pas déjà eu un dans la même
                                       // période de tests
      }
    }
}
