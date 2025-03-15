// ***********************************************
// Gestion de la page html paramètres 
// ***********************************************

String updatePage = R"rawliteral(
    <!DOCTYPE html>
    <head>
      <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>
      <title>Paramètres JWS</title>
      <style>
        body {
          font-family: Calibri, Arial, sans-serif;
          background-color: #333333;
          color: #FFF; /* Blanc */
          text-align: center;
          margin-top: 50px;
        }
        #status {
            font-size: 24px;
            color: #E0E0E0;
        }
        h1 {
          color: #e8f117; /* Jaune */
        }
        label {
          font-size: 1.2em;
        }        
        button {
          background-color: #E0E0E0; /* Gris */
          color: black;
          border: none;
          padding: 15px 30px;
          text-align: center;
          font-size: 16px;
          border-radius: 8px;
          cursor: pointer;
          margin: 10px;
          box-shadow: 0 4px #999;
        }
        button:hover {
            background-color: #45a049; /* Vert clair */
        }
        button:active {
            background-color: #3e8e41; /* Vert foncé */
            box-shadow: 0 2px #666;
            transform: translateY(2px);
        }
        input[type=submit] {
          background-color: #E0E0E0; /* Gris */
          color: black;
          border: none;
          padding: 15px 30px;
          text-align: center;
          font-size: 16px;
          border-radius: 8px;
          cursor: pointer;
          margin: 10px;
          box-shadow: 0 4px #999;
        }
        input[type=submit]:hover {
            background-color: #45a049; /* Vert clair */
        }
      </style>
    </head>
    <body>
    <h1>Mode de fonctionnement</h1>
     <button onclick="sendState('Maintenance')">Maintenance</button>
     <button onclick="sendState('Opérationnel')">Opérationnel</button>
     <p id="status">Mode : </p>

    <script>
        function sendState(state) {
            fetch(`/button?state=${state}`)
                .then(response => response.text())
                .then(data => {
                    document.getElementById('status').textContent = "Mode : " + data;
                })
                .catch(error => console.error('Erreur :', error));
        }

        window.onload = function () {
            fetch('/button/state')
                 .then(response => response.text())
                 .then(data => {
                    document.getElementById('status').textContent = "Mode : " + data;
                })
               .catch(error => console.error('Erreur :', error));
     };
    </script>

    <br><br>  
    <h1>Mise à jour du système de surveillance JWS</h1>
    <form method='POST' action='/update' enctype='multipart/form-data'>
      <input type='file' name='firmware'><br><br>
      <input type='submit' value='Mettre &agrave; jour le firmware'>
    </form>
    </body>
    </html>
  )rawliteral";