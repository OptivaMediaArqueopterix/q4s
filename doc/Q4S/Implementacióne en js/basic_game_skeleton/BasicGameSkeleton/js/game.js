/**
 * Mark pulse of the game
 */
window.requestAnimFrame = (function() {
	return window.requestAnimationFrame 
			|| window.webkitRequestAnimationFrame
			|| window.mozRequestAnimationFrame 
			|| window.oRequestAnimationFrame
			|| window.msRequestAnimationFrame
			|| function( callback, element) {
					window.setTimeout(callback, 1000 / 60);
			};
})();

/**
 * Game control
 */
var game = (function() {
	//TODO Canvas variables
	
	//TODO Define keys variables
	
	//TODO Define control variables
	
	//TODO Define margins variables
	
	//TODO Define players variables
	
	/**
	 * Function that is called from the api to start the game, this method will be called from the api.
	 * 
	 * @param pRoom (room that connects)
	 * @param pUser (user id will be in the game)
	 * @param pNumUser (max users in this room)
	 */
	function preInit(pRoom, pUser, pNumUser) {
		//TODO save session highs

		//TODO do you need to start the game, images, canvas, text		
		
		//TODO preload images

		//TODO initiate canvas

		//TODO animate	
		
		//Clean textBox
		try {
			$('#idRomm').html(pRoom);
		} catch (e) {}
		try {
			$('#idPlayer').html(pUser);			
		} catch (e) {}
		try {
			$('#idLatencia').html(limitLatency);			
		} catch (e) {}
		
		//Players begin
		iniciarJugadores(pUser);
	}
	
	/**
	 * Players begin as the pUser (player number assigned)
	 */
	function iniciarJugadores(pUser) {
		userId = parseInt(pUser);
		if(pUser == 1){		
			//Put master			
			MASTER = true;
			connected = true;
			
			//TODO take messages if necessary, initiate local player with 1
			localPlayer = new LocalPlayer(1);
		} else {
			//Put other player		
			MASTER = false;
			connected = true;			
			
			//TODO take messages if necessary, local player start with pUser and put the other players					
			
			//Messages to the API		
			if (pUser == maxSess) {															//The last
				priorWriteAction(MEN_TYPE_BEGIN + SEP + userId + SEP + MEN_ULTIMO);			//START GAME ALL JOINED
			}  else {
				priorWriteAction(MEN_TYPE_BEGIN + SEP + userId + SEP + MEN_CONECTADO);		//I log and I say to others			
			}			
			
			//TODO take messages if necessary
		}
	}
	
	/**
	 * Method responsible for managing incoming messages from the server, this method will be called from the api.
	 */
	function readResponse(data) {
		//Vapture message
		var messageData = data.split(SEP);
		
		if (messageData[0] == MEN_TYPE_BEGIN) {			//Message type BEGIN
			var idCon = parseInt(messageData[1]);		//User online
			var men = messageData[2];					//Message to begin
									
			if(men == MEN_CONECTADO){					//If Online
				//TODO a user has connected											
			} else if(men == MEN_ULTIMO){				//If last
				//TODO the last user has logged							
			}								
		} else if (messageData[0] == MEN_TYPE_TECLAS) {	//Message type TECLAS
			//TODO a user has moved
			var userIdAux = parseInt(messageData[1]);
			var keyCodeAux = parseInt(messageData[2]);
			var posXAux = parseInt(messageData[3]);
			var posYAux = parseInt(messageData[4]);
			var direction = parseInt(messageData[5]);	
			var playerAux = new Object();			
		} else if (messageData[0] == MEN_TYPE_JUGAR) {	//Message type JUGAR
			//TODO the game starts all connected
		}
	}
	
	/**
	 * End room, this method will be called from the api.
	 */
	function endSala() {			
		//TODO end room, take messages if necessary and restart the game variables	
	}
	
	/**
	 * Managing keys
	 */
	function keyDown(e) {
		//TODO capture key				
	}
	
	/**
	 * Create Localplayer
	 */
	function LocalPlayer(id) {
		//TODO Local player
	}
	
	/**
	 * Crear Enemigos (demas jugadores)
	 */
	function Enemy(id) {
		//TODO Other player
	}			
	
	/**
	 * Public Methods
	 */
	return {
		readResponse : readResponse,
		preInit : preInit,
		endSala : endSala
	};
})();

/**
 * To call var endSala of the game
 */
function preInit(pRoom, pUser, pNumUser) {
	game.preInit(pRoom, pUser, pNumUser);
}

/**
 * To call var redresponse of the game
 */
function readResponse(data) {
	game.readResponse(data);
}

/**
 *To call var endSala of the game
 */
function endSala() {
	game.endSala();
}