// **********************************************************************************
// Gestion de la connexion initiale au réseau Wifi ou de la définition des paramètres 
// **********************************************************************************

// Essai de connexion automatique à un réseau Wifi au démarrage
void tryConnect() {
  
  if (ssid != "" && password != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    for (int i = 0; i < 10; i++) {
      if (WiFi.status() == WL_CONNECTED) { // Connecté au réseau Wifi.
        mode_AP = false;
        Extinction_LED();
        setupWebServer();              // Lance l'interface web des mises à jour par OTA pour le téléversement des firmwares
        return;                        
      }
      delay(1000);                     // Délai d'attente avant de retenter une connexion Wifi
    }
  }
  mode_AP = true;                      // Indique qu'il n'y a pas d'identifiants renseignés ou ne sont pas valides.        
  Extinction_LED();
  ledcWrite(LED_PINGIP, LED_ON);       // Allume la LED rouge tant qu'on reste dans le mode AP
  setupWebServerAP();                  // La connexion a donc échouée, on lance l'interface web et l'ESP en mode AP
}

// Mise en place d'un réseau Wifi en mode AP et du serveur web
void setupWebServerAP() {              // Démarrage en mode Access Point (AP)

  WiFi.softAP(ap_ssid, ap_password);   
  server.on("/", handleRootAP);        // Démarre le serveur Web
  server.on("/connect", handleConnect);
  server.begin();
}

// Affichage du formulaire a remplir par l'utilisateur en mode AP
void handleRootAP() {

  server.send(200, "text/html", loginIndex);
}

// Récupération des paramètres SSID, mot de passe, @IP du serveur MQTT, @IP de la box Jeedom, nom d'user MQTT, mot 
// de passe MQTT et numéro de port MQTT définis dans le formulaire.
void handleConnect() {

  ssid = server.arg("ssid");
  password = server.arg("password");
  mqtt_server = server.arg("mqtt_server");
  mqtt_user = server.arg("mqtt_user");
  mqtt_password = server.arg("mqtt_password");
  String mqtt_port_string = server.arg("mqtt_port");
  jeedom_server = server.arg("jeedom_server");

  // Vérifie si les champs sont vides et défini les paramètres par défaut
  if (ssid.isEmpty() || password.isEmpty()) {
    server.send(400, "text/plain", "Erreur : SSID ou mot de passe vide.");
    return;}
  if (mqtt_port_string.isEmpty()){mqtt_port = mqtt_port_defaut;} 
   else {mqtt_port = mqtt_port_string.toInt();}
  if (jeedom_server.isEmpty()){jeedom_server = mqtt_server;}

  // Sauvegarde des informations dans l'EEPROM
  sauvegarde_parametres(ssid, password, mqtt_server, mqtt_user, mqtt_password, mqtt_port, jeedom_server);
  server.send(200, "text/plain", "Connexion en cours...");

  // Déconnecte l'ESP du point d'accès actuel
  WiFi.softAPdisconnect(true);
  delay(1000);
  esp_restart();                       // Déclenche un reset logiciel une fois que les paramètres sont définis et enregistrés. 
}