// **********************************************************************************
// Gestion de la connexion initiale au réseau Wifi ou de la définition des paramètres 
// **********************************************************************************

// Essai de connexion automatique à un réseau Wifi au démarrage
void tryConnect() {
  
  WiFi.setHostname(hostname_st);
  if (ssid != "" && password != "") {
    WiFi.begin(ssid.c_str(), password.c_str());
    for (int i = 0; i < 200; i++) {    // On va effectuer max 200 essais x delai d'attente (200x1", soit 3'20") avant d'abandonner 
      if (WiFi.status() == WL_CONNECTED) { // Connecté au réseau Wifi.
        setupWebServer();              // Lance l'interface web html
        return;                        
      }
      delay(1000);                     // Délai d'attente d'une seconde avant de retenter une connexion Wifi
    }
  }
  Extinction_LED();
  ledcWrite(LED_PINGIP, LED_ON);       // Allume la LED rouge tant qu'on reste dans le mode AP
  setupWebServerAP();                  // La connexion a échouée, on lance donc l'interface web et l'ESP en mode AP
}

// Mise en place d'un réseau Wifi en mode AP et du serveur web
void setupWebServerAP() {              // Démarrage de l'interface web html spécifique au mode Access Point (AP)
  WiFi.softAP(ap_ssid, ap_password);   // Démarre une liaison WiFI en mode AP (point à point) 
  server.on("/", handleRootAP);        // Démarre le serveur Web en mode AP
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

// Test de la connexion Wifi en mode STA (point d'accès) 
void WiFiConnect() {
  if (WiFi.status() != WL_CONNECTED && WiFi.getMode() == WIFI_STA) {
    WiFi.reconnect();                  // Tentative de reconnexion au réseau Wifi
    if (WiFi.waitForConnectResult(10000) != WL_CONNECTED) { // Résultat de la tentative de reconnexion (10 secs de time-out)
      esp_restart();                   // Déclenche un reset logiciel s'il n'y a toujours pas de connexion. 
    }
  }
}

// Gestion de l'état du mode
void handleButton() {
    if (server.hasArg("state")) {
        buttonState = server.arg("state");
     ;   if (buttonState == "Opérationnel") {mode=0;} else {mode=1;}
        Serial.println(mode);
        server.send(200, "text/plain", buttonState);
    } else {
        server.send(400, "text/plain", "Argument manquant");
    }
}

// Gestion de l'affichage du mode sur la page html
void handleButtonState() {
    server.send(200, "text/plain", buttonState);
}

// Affichage de la page principale html
void handleRoot() {
  server.send(200, "text/html", updatePage);
}

// Lancement de la page du serveur web html
void setupWebServer() {
  Extinction_LED();
  server.on("/", handleRoot);          // Route pour la page principale
  server.on("/button", HTTP_GET, handleButton); // Route pour gérer les appuis sur les boutons Maintenance ou Opérationnel
  server.on("/button/state", HTTP_GET, handleButtonState); // Route pour gérer l'affichage du mode actuel
  server.on("/update", HTTP_POST, []() { // Route pour gérer les mises à jours par OTA
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "Echec de la mise a jour" : "Mise a jour reussie. Redemarrage...");
    ESP.restart();
  }, update); 
  server.begin();
}
