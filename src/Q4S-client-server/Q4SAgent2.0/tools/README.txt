/*****COPYRIGHT EXPERIS IT *********/
/*****AUTHOR FRANCISCO ABRIL*******/

El presente SW permite conectar el Actuador con el Policy Server realizando las siguientes Acciones:

----LOGIN----
PREP - 1) Se debe crear un fichero auth.json con las credenciales de acceso. (Ver ejemplo)
REQ - 1) Ejecutar el BAT "REQ1_startLogin.bat"
RESP - 1) Se genera el fichero token.txt

----GET SESSION----
PREP - 2) Se debe crear un fichero sdp.json con los datos de flujo principal. (Ver ejemplo)
REQ - 2) Ejecutar el BAT "REQ2_startSession.bat"
RESP - 2) Se genera el fichero response.file que contiene el ID de sesión.

----UPGRADE SESSION----
PREP - 3A) Se debe crear un fichero alert.json con los datos de flujo principal + ID Sesión llamada anterior, flujo de medidas y resultado de las medidas. (Ver ejemplo)
REQ - 3A) Ejecutar el BAT "REQ3_upgrade.bat"
RESP - 3A) Se genera el fichero response.file que contiene el ID de alerta.

----DOWNGRADE SESSION----
PREP - 3B) Se debe crear un fichero alert.json con los datos de flujo principal + ID Sesión llamada 2, flujo de medidas y resultado de las medidas. (Ver ejemplo)
REQ - 3B) Ejecutar el BAT "REQ3_downgrade.bat"
RESP - 3B) Se genera el fichero response.file que contiene el ID de alerta.
