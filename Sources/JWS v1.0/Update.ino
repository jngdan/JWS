// ***********************************************
// Gestion des mises à jour OTA par le réseau Wifi 
// ***********************************************

// Allumage progressif de la LED verte si succès MAJ
void LED_Update_OK() {
  Extinction_LED();                    // Extinction des LEDs
  for (int brightness = 255; brightness >= 0; brightness--) {
    for (int i = 255; i >= 0; i--) {
      if (i > brightness) {
        digitalWrite(LED_CNXMQTT, 0);  // Allume la LED verte
      } else {
        digitalWrite(LED_CNXMQTT, 1);  // Éteint la LED verte
      }
    }
    delay(15);                         // Délai pour matérialiser une augmentation progressive de la luminosité
  }
  Extinction_LED();                    // Extinction des LEDs avant de terminer la fonction
}

// Affichage de la page de mise à jour
void handleRoot() {
  server.send(200, "text/html", updatePage);
}

// Affichage de la progression du flash par un clignotement de la LED associée à l'erreur du ping IP
void LEDblink(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
     previousMillis = currentMillis;
     ledState = !ledState;             // Inverse l'état de la LED
     digitalWrite(LED_PINGIP, ledState);
     }
  Affichage_OTA();                     // Affichage de la mise à jour en cours sur l'écran OLED
}

// Mises à jour OTA par le serveur web
void setupWebServer() {
  Extinction_LED();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "Echec de la mise a jour" : "Mise a jour reussie. Redemarrage...");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) { // Début de la mise à jour
      Extinction_LED();                       // Extinction des LEDs
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      } else {
        Update.onProgress([](size_t progress, size_t total) {
         LEDblink();
        });
      } 
    } else if (upload.status == UPLOAD_FILE_WRITE) { // Envoi du fichier 
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) { // Fin de la mise à jour
      if (Update.end(true)) {
          LED_Update_OK();             // La mise à jour est OK, sans erreurs.
      } else {
        Update.printError(Serial);
      }
    } else {
      erreur_OTA = true;               // Indique que la mise à jour a été annulée ou qu'il y a eu une erreur
      Affichage_OTA();
      erreur_OTA = false;
      Extinction_LED();                // Eteint les LED      
    }
  });
  server.begin();
}

// Mises à jour OTA par l'interface IDE Arduino
void setupOTA() {
  ArduinoOTA.setHostname(otaHostname); // Défini le nom de l'ESP pour les mises à jour via OTA
  ArduinoOTA.onStart([]() {
    Extinction_LED();
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {                           // U_SPIFFS
      type = "filesystem";
    }
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
   LEDblink();
  });
  ArduinoOTA.onError([](ota_error_t error) { // Gestion des erreurs de mise à jour
   Extinction_LED();                   // Eteint les LED
   erreur_OTA = true;
   Affichage_OTA();
  });
  ArduinoOTA.onEnd([]() {
    if (!erreur_OTA) {LED_Update_OK();}// La mise à jour est OK, sans erreurs.
    });
  erreur_OTA = false;
  ArduinoOTA.begin();                  // Initialisation des mises à jours par OTA
}
