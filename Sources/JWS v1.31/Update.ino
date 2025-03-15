// ***********************************************
// Gestion des mises à jour OTA par le réseau Wifi 
// ***********************************************

// Allumage progressif de la LED verte ou rouge pour signaler le résultat de la mise à jour
void LED_Update(bool LED_MAJ) {
  int LED;
  if (LED_MAJ) {
    LED = LED_CNXMQTT; // Mise à jour OK (pas d'erreur)
  }
  else {
    LED = LED_PINGIP;  // Mise à jour KO (en erreur)
  }
  Extinction_LED();                    // Extinction des LEDs
  for(int i = 0; i <= 2; i++){         // Provoque 3x pulsations de la LED
        ledcFade(LED, LED_OFF, LED_ON, 1000);
        delay(1000);
        ledcFade(LED, LED_ON, LED_OFF, 1000);
        delay(1000);
        }
  delay(1000);      
}

// Affichage de la progression du flash par un clignotement de la LED associée à l'erreur du ping IP
void LEDblink(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
     previousMillis = currentMillis;
     ledState = !ledState;             // Inverse l'état de la LED
     if (ledState){        
      ledcWrite(LED_PINGIP, LED_ON);   // LED allumée à 100%
     }
     else
     {
      ledcWrite(LED_PINGIP, LED_OFF); // LED éteinte
     } 
  }
  Affichage_OTA();                     // Affichage de la mise à jour en cours sur l'écran OLED
}

void update() {
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
          LED_Update(1);               // La mise à jour est OK, sans erreurs.
      } else {
        Update.printError(Serial);
      }
    } else {                           // La mise à jour a été annulée ou qu'il y a eu une erreur
      Extinction_LED();                // Eteint les LED 
      erreur_OTA = true;               // Indique l'erreur de la MAJ
      Affichage_OTA();                 // Affiche un message sur l'écran OLED
      erreur_OTA = false;
      LED_Update(0);                   // La mise à jour est KO, avec erreur(s).
    }
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
  ArduinoOTA.onError([](ota_error_t error) { // La mise à jour a été annulée ou qu'il y a eu une erreur
   Extinction_LED();                   // Eteint les LED
   erreur_OTA = true;                  // Indique l'erreur de la MAJ
   Affichage_OTA();                    // Affiche un message sur l'écran OLED
   LED_Update(0);                      // La mise à jour est KO, avec erreur(s).
  });
  ArduinoOTA.onEnd([]() {
    if (!erreur_OTA) {LED_Update(1);}  // La mise à jour est OK, sans erreurs.
    });
  erreur_OTA = false;
  ArduinoOTA.begin();                  // Initialisation des mises à jours par OTA
}
