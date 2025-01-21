// ****************************************************************
// Gestion de la page html de la définition initiale des paramètres 
// ****************************************************************

// Page HTML en mode AP permettant d'entrer les identifiants de connexion Wifi et les paramètres MQTT. 
// @IP de connexion par défaut sur le réseau AP : 192.168.4.1

String loginIndex = R"rawliteral( 
       <!DOCTYPE html>
    <head>
      <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>
      <title>Initialisation JWS</title>
      <style>
        body {
          font-family: Calibri, Arial, sans-serif;
          background-color: #333333;
          color: #FFF;
          text-align: left;
          margin-top: 50px;
        }
        h1 {
          color: #e8f117;
        }
      </style>
    </head>
    <body>
    <h1>Initialisation des paramètres JWS</h1>
    <form action="connect" method="POST">
       SSID : <input type="text" name="ssid"><br><br>
       Password : <input type="password" name="password"><br><br>
       MQTT IPServer : <input type="text" name="mqtt_server"><br><br>
       MQTT User : <input type="text" name="mqtt_user"><br><br>
       MQTT Password : <input type="text" name="mqtt_password"><br><br>
       MQTT Port (default = 1883): <input type="text" name="mqtt_port"><br><br>
       Jeedom IPServer (default = MQTT IPServer) : <input type="text" name="jeedom_server"><br><br>
       <input type="submit" value="Valider">
    </form>
    </body>
    </html>
  )rawliteral";