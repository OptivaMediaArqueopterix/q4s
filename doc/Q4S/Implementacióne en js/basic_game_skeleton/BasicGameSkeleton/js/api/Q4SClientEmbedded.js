// Nofication display time in ms.
var NOTIFICATION_DISPLAY_TIME = 15000;
var SERVER_ADDRESS;
var SERVER_TCP_PORT;
var SERVER_UDP_PORT;

var socketId; // For the UDP connection
var sid;
var data_received;
var result;

var LINE_SEPARATOR = '\n';
var BEGIN_HEADER0 = 'BEGIN q4s://';
var BEGIN_HEADER1 = ' Q4S/1.0\n';
var PING_HEADER0 = 'PING /stage0 Q4S/1.0';
var PING_HEADER2 = 'PING /stage2 Q4S/1.0';
var PING_ = 'PING';
var BWIDTH_HEADER = 'BWIDTH /stage1 Q4S/1.0';
var OK_HEADER = 'Q4S/1.0 200 OK';
var CANCEL_HEADER = 'CANCEL q4s://';
var SESSION_ID_HEADER = 'Session-Id';
var SEQUENCE_NUMBER_HEADER = 'Sequence-Number';
var MEASUREMENTS_HEADER = 'Measurements';
var TIMESTAMP_HEADER = 'Timestamp';
var CONTENT_LENGTH_HEADER = 'Content-Length';
var CONTENT_TYPE_HEADER = 'Content-Type: application/sdp';

var BWIDTH_CONSTRAINT; // =
// 1000;/*parseInt(localStorage.appBwConstraintClient);
// 1000 kbps;*/

var SDP_0 = 'v=0' + LINE_SEPARATOR + 'o=q4s-UA ' + navigator.userAgent
		+ LINE_SEPARATOR + 's=Q4S' + LINE_SEPARATOR + 'i=Q4S parameters'
		+ LINE_SEPARATOR;

var APP_ALERT_PAUSE;
var SDP_APP_ALERT_PAUSE;
var SDP_APP_CONSTRAINTS;
var SDP_PROCEDURE_DEFAULT;
var N_OF_PINGS_NEGOTIATION_CLIENT; // number of pings //begins at 0
var N_OF_PINGS_CONTINUITY_CLIENT;
var PING_INTERVAL_NEGOTIATION_CLIENT;// 50; //ms including proccesing time:
// 50-1
var PING_INTERVAL_CONTINUITY_CLIENT;

var BWIDTH_MESSAGE_LENGTH = 4096;// 4096;
var BWIDTH_MESSAGE_LENGTH_SERVER = 4096;// 4096;

var seq_number_bw_sent = -1;
var counterSentPackets = 0;
var bwf; // = BWIDTH_CONSTRAINT/32; //KBps i.e., packets per seconds (/8 if
// packets are 1 KB, /32 if packets are 4 KB length)
var packetsPerMs; // = bwf/1000;
var BWIDTH_INTERVAL_TIMER;

var wakeUpTime;
var numOfPackets;
var uplinkBW;
var downlinkBW;
var BWIDTH_WINDOW; // = 5000;

var ev;
var notif = [];
var sentTimePing = [];
var rtt = [];
var pingElapsed = [];
var receivedPingTime = [];
var pingIntervalServer = [];
var downlink_jitter;
var uplink_jitter;
var latency;
var seq_received_pings = [];
var downlink_packet_loss;
var uplink_packet_loss;

var packets2SendPerInterval;
var bwOriginTime;
var bwBeginningTime;
var bwidthInterval;
var bwMsgReceived = [];
var bwMsgCounter;

var socket;
var socketUdpQ4S = chrome.socket; // || chrome.experimental.socket;
console.log("socket:" + socketUdpQ4S);

var sdp;

var mySession;

var notifications = [];

function startQ4S() {

	SERVER_ADDRESS = "q4s.dit.upm.es";
	SERVER_TCP_PORT = 8001;
	SERVER_UDP_PORT = 62516;

	APP_ALERT_PAUSE = 25000;

	SDP_APP_ALERT_PAUSE = 'a=app-alert-pause:' + APP_ALERT_PAUSE
			+ LINE_SEPARATOR;

	SDP_APP_CONSTRAINTS = 'a=application:latency:100' + LINE_SEPARATOR
			+ 'a=applicacion:jitter:15/8' + LINE_SEPARATOR
			+ 'a=applicacion:bandwidth:100/100' + LINE_SEPARATOR
			+ 'a=applicacion:packetloss:1/1' + LINE_SEPARATOR;

	SDP_PROCEDURE_DEFAULT = 'a=measurement:procedure default(50/50,100/100,5000,10/10,1000/1000)';

	BWIDTH_CONSTRAINT = 100;

	bwf = BWIDTH_CONSTRAINT * 1024 / (8 * BWIDTH_MESSAGE_LENGTH); // KBps
	// i.e.,
	// packets
	// per
	// seconds
	// (/8 if
	// packets
	// are 1 KB,
	// /32 if
	// packets
	// are 4 KB
	// length)

	packetsPerMs = bwf / 1000;

	if (BWIDTH_CONSTRAINT <= 1000)
		BWIDTH_INTERVAL_TIMER = 150;// Math.ceil(1/packetsPerMs); //ms
	else if (BWIDTH_CONSTRAINT < 3000)
		BWIDTH_INTERVAL_TIMER = 30; // Math.ceil(192*packetsPerMs); //ms //48
	else if (BWIDTH_CONSTRAINT < 5000)
		BWIDTH_INTERVAL_TIMER = 20; // Math.ceil(386*packetsPerMs); //ms //24
	else
		BWIDTH_INTERVAL_TIMER = 17;

	N_OF_PINGS_NEGOTIATION_CLIENT = 10; // 255 //number of pings //begins at 0
	N_OF_PINGS_CONTINUITY_CLIENT = 999; // 1000 //number of pings //begins at 0
	PING_INTERVAL_NEGOTIATION_CLIENT = 49;// 49;//50; //ms including
	// proccesing time: 50-1*/
	PING_INTERVAL_CONTINUITY_CLIENT = 99;// 49;//50; //ms including
	// proccesing time: 50-1*/

	BWIDTH_WINDOW = 5000;

	console.log("SERVER_ADDRESS:" + SERVER_ADDRESS);
	console.log("SDP_APP_ALERT_PAUSE:" + SDP_APP_ALERT_PAUSE);
	console.log("SDP_APP_CONSTRAINTS:" + SDP_APP_CONSTRAINTS);
	console.log("SDP_PROCEDURE_DEFAULT:" + SDP_PROCEDURE_DEFAULT);
	console.log("N_OF_PINGS_NEGOTIATION_CLIENT:"
			+ N_OF_PINGS_NEGOTIATION_CLIENT);
	console.log("PING_INTERVAL_NEGOTIATION_CLIENT:"
			+ PING_INTERVAL_NEGOTIATION_CLIENT);
	console.log("N_OF_PINGS_CONTINUITY_CLIENT:" + N_OF_PINGS_CONTINUITY_CLIENT);

	var opts = {};
	opts['sync disconnect on unload'] = false;

	socket = io.connect('http://' + SERVER_ADDRESS + ':' + SERVER_TCP_PORT,
			opts);

	socket.on('error', function(e) {
		socket = null;

		console.log("connection error !!");
	});

	console.log("Socket got:" + socket);

	beginQ4SSession();
}

function stopQ4S() {
	sdp = SDP_0 + SDP_APP_ALERT_PAUSE + SDP_APP_CONSTRAINTS
			+ SDP_PROCEDURE_DEFAULT;
	var contentLengthCancel = CONTENT_LENGTH_HEADER + ": " + sdp.length;
	var cancelMsg = CANCEL_HEADER + SERVER_ADDRESS + BEGIN_HEADER1 + "\n"
			+ CONTENT_TYPE_HEADER + "\n" + contentLengthCancel + "\n" + sdp;

	socket.emit('cancel', cancelMsg, mySession);

	for (notific in notifications) {
		console.log("notific:" + notific);
		notifications[notific].cancel();
	}

}

function getQ4SMeasurements() {
	var q4sMeasures = 'Latency: ' + latency + ' ms; Jitter: ' + uplink_jitter
			+ '/' + downlink_jitter + ' PacketLoss: ' + uplink_packet_loss
			+ '/' + downlink_packet_loss + ' (%)';
	return q4sMeasures;

}

// function getQ4SLatency(){
// return latency;
// }

var Q4SClientEmbedded = {
	getQ4SLatency : function() {
		//console.log("latencia Q4SClientEmbedded: " + latency);
		return latency;
	}
};

function beginQ4SSession() {
	sdp = SDP_0 + SDP_APP_ALERT_PAUSE + SDP_APP_CONSTRAINTS
			+ SDP_PROCEDURE_DEFAULT;
	var contentLengthBegin = CONTENT_LENGTH_HEADER + ": " + sdp.length;
	var beginMsg = BEGIN_HEADER0 + SERVER_ADDRESS + BEGIN_HEADER1 + "\n"
			+ CONTENT_TYPE_HEADER + "\n" + contentLengthBegin + "\n" + sdp;

	socket.emit('begin', beginMsg);

	socket.on('200 OK BEGIN', function(sessionId) {
		// showEvent("Handshake", "OK");
		if (sessionId !== null) {
			mySession = sessionId;
			socket.emit('ready', 0, sessionId); // Stage 0
		}
	});

	socket.on('200 OK READY', function(stage, sessionId) {
		showEvent("Negotiation Stage 0", "Begin");
		if (sessionId !== null) {
			sid = sessionId;
			// remove listeners of this socket
			socket.removeAllListeners('200 OK READY');
			socketUdpQ4S.create('udp', {}, onCreate);
		}
	});
}

function negotiation_Stage1(sessionId) {
	socket.emit('ready', 1, sessionId); // Stage 1
	socket.on('200 OK READY', function(stage, sessionId) {
		// remove listeners of this socket
		socket.removeAllListeners('200 OK READY');

		// showEvent("Negotiation Stage 1", "Begin");
		bwMsgCounter = 0;
		bwOriginTime = new Date().getTime();
		bwBeginningTime = bwOriginTime;

		// packets2SendPerInterval =
		// Math.ceil(BWIDTH_INTERVAL_TIMER*packetsPerMs*1.01);
		// ***new
		packets2SendPerInterval = 1;
		BWIDTH_INTERVAL_TIMER = Math.ceil(0.9 * packets2SendPerInterval
				/ packetsPerMs);
		if (BWIDTH_INTERVAL_TIMER < 20) {
			packets2SendPerInterval = 2;
			BWIDTH_INTERVAL_TIMER = Math.ceil(0.9 * packets2SendPerInterval
					/ packetsPerMs);

		}

		numOfPackets = Math.ceil(BWIDTH_WINDOW * packets2SendPerInterval
				/ BWIDTH_INTERVAL_TIMER);
		console.log("****  numOfPackets: " + numOfPackets
				+ " packets2SendPerInterval:" + packets2SendPerInterval
				+ " BWIDTH_INTERVAL_TIMER:" + BWIDTH_INTERVAL_TIMER);

		setTimeout(function() {
			// periodic launch of bwidth
			ready2SendBWidthMessages();
		}, BWIDTH_INTERVAL_TIMER);

	});
}

function showEvent(title, ev) {

	// notification.show();
	// var icon = 'http://'+SERVER_ADDRESS+':8480/img/icon_16.png';
	// var notification = webkitNotifications.createNotification(icon, title,
	// ev);
	// notification.show();
	// notifications.push(notification);
	//		
	// // Hide the notification after the configured duration.
	// setTimeout(function(){
	// notification.cancel();
	// notifications.pop(notification);}, NOTIFICATION_DISPLAY_TIME);
}

function str2abQ4S(str) {
	var buf = new ArrayBuffer(str.length/** 2 */
	); // 2 bytes for each char
	var bufView = new Uint8Array(buf);
	for ( var i = 0, strLen = str.length; i < strLen; i++) {
		bufView[i] = str.charCodeAt(i);
	}
	return buf;
}

function str2ab_1024length(str) {
	var buf = new ArrayBuffer(BWIDTH_MESSAGE_LENGTH);
	var bufView = new Uint8Array(buf);
	var strLen = str.length;
	for ( var i = 0; i < strLen; i++) {
		bufView[i] = str.charCodeAt(i);
	}
	for ( var i = strLen; i < BWIDTH_MESSAGE_LENGTH; i++) {
		bufView[i] = '0xCF';
	}
	return buf;
}

function ab2strQ4S(buf) {
	var str = "", view = new Uint8Array(buf), len = view.length, fromCharCode = String.fromCharCode;

	for ( var i = 0; i < len; ++i) {
		str += fromCharCode(view[i]);
	}

	return str;
}

function onCreate(socketInfo) {
	socketId = socketInfo.socketId;
	socketUdpQ4S.connect(socketInfo.socketId, SERVER_ADDRESS, SERVER_UDP_PORT,
			onConnect);
}

function onConnect(result) {
	// console.log ("socketId:"+socketId);
	var seq_number_pings_sent = -1;
	var lastInterval;
	var ping_interval = setInterval(
			function() { // periodic launch of pings
				var intervalTime = new Date().getTime();

				seq_number_pings_sent++;
				if (seq_number_pings_sent > N_OF_PINGS_NEGOTIATION_CLIENT) {
					clearInterval(ping_interval);
					setTimeout(function() {
						showEvent("Negotiation Stage 0 finished", 'Latency: '
								+ latency + ' ms; Jitter: ' + uplink_jitter
								+ '/' + downlink_jitter + ' PacketLoss: '
								+ uplink_packet_loss + '/'
								+ downlink_packet_loss + ' (%)');
						// begin negotiation stage 1
						negotiation_Stage1(sid);
					}, 1000);
				} else {
					var measurements = MEASUREMENTS_HEADER + ": l=" + latency
							+ ", j=" + downlink_jitter

					if (seq_number_pings_sent === N_OF_PINGS_NEGOTIATION_CLIENT) {
						var last_ping_received = seq_received_pings[seq_received_pings.length - 1];
						// last_ping_received++; //sequence begins at 0, but not
						// added because it has not been pushed into the stack
						// yet
						downlink_packet_loss = 100 - (100 * seq_received_pings.length / last_ping_received);
						measurements += ", pl=" + downlink_packet_loss;
					}

					var ab = str2abQ4S(PING_HEADER0 + "\n" + SESSION_ID_HEADER
							+ ": " + sid + "\n" + SEQUENCE_NUMBER_HEADER + ": "
							+ seq_number_pings_sent + "\n" + measurements
							+ "\n" + TIMESTAMP_HEADER + ": " + intervalTime
							+ "\n" + CONTENT_LENGTH_HEADER + ": 0");
					var str2abTime = new Date().getTime();
					socketUdpQ4S.write(socketId, ab, function(writeInfo) {
						var currentTime = new Date().getTime();
						sentTimePing[seq_number_pings_sent] = currentTime;
						// console.log("****** PING
						// timer:"+(lastInterval-intervalTime)+" sent
						// time:"+(currentTime-intervalTime)+" parsing
						// time:"+(str2abTime-intervalTime));
						lastInterval = intervalTime;
					});

				} // end of else
			}, PING_INTERVAL_NEGOTIATION_CLIENT);

	//
	// chrome.experimental.socket.recvFrom(socketId, onDataRead);
	receivingDataQ4S(socketId);

}

function receivingDataQ4S(socketId) {
	socketUdpQ4S
			.recvFrom(
					socketId,
					function(readInfo) {
						if (readInfo.port === 0)
							return;
						var currTime = new Date().getTime();
						var newString = ab2strQ4S(readInfo.data);
						// console.log("receiving:"+readInfo+"
						// resultCode:"+readInfo.resultCode+"
						// address:"+readInfo.address+" port:"+readInfo.port+"
						// data:"+newString);

						// var timeAB2str = new Date().getTime();

						// var newString = new String(s);

						var myArray = newString.split("\n");
						var msgParsed = new Object();
						for (i = 0; i < myArray.length; i++) {
							var eachLine = myArray[i].split(":");
							var str = eachLine[0];
							msgParsed[str] = eachLine[1];
						}

						var sid2 = (msgParsed[SESSION_ID_HEADER]).trim();
						// if (sid2){
						// sid2.trim();}

						var sn2 = (msgParsed[SEQUENCE_NUMBER_HEADER]).trim();
						/*
						 * try{ sn2 } catch (err) { console.log("ERROR:"+err+";
						 * "+newString); }
						 */
						// if (sn2){
						// sn2.trim();}
						var meas = (msgParsed[MEASUREMENTS_HEADER]);
						if (meas) {
							meas.trim();
							var myMeas = meas.split(",");
							var measParsed = new Object();
							for (i = 0; i < myMeas.length; i++) {
								var eachMeas = myMeas[i].split("=");
								var strMeas = eachMeas[0];
								measParsed[strMeas] = eachMeas[1];
							}
							uplink_jitter = (measParsed[' j'])
							if (uplink_jitter)
								uplink_jitter.trim();

							var temp_uplink_packet_loss = (measParsed[' pl'])
							if (temp_uplink_packet_loss) {
								uplink_packet_loss = temp_uplink_packet_loss;
								uplink_packet_loss.trim();
							}

							uplinkBW = (measParsed[' bw'])
							if (uplinkBW)
								uplinkBW.trim();
						}
						var timest = (msgParsed[TIMESTAMP_HEADER]);
						if (timest) {
							timest.trim();
						}

						var time1 = new Date().getTime()

						if (newString.indexOf(PING_) != -1) { // PING stage
							// 0,2 has been
							// received
							// console.log("<-- PING received
							// ["+currTime+"]:"+newString);
							// console.log("<-- PING parsing time ["+time1+"]");

							var ab2 = str2abQ4S(OK_HEADER + "\n"
									+ SESSION_ID_HEADER + ": " + sid2 + "\n"
									+ SEQUENCE_NUMBER_HEADER + ": " + sn2
									+ "\n" + CONTENT_LENGTH_HEADER + ": 0");
							var time2 = new Date().getTime()

							socketUdpQ4S
									.write(
											socketId,
											ab2,
											function(writeInfo) {
												var sentTime = new Date()
														.getTime();
												// console.log("200 OK sent
												// time:"+sentTime);
												receivedPingTime[sn2] = currTime;
												pingIntervalServer[sn2] = timest;
												if (sn2 > 0) {
													if (receivedPingTime[sn2 - 1] > 0) {
														pingElapsed
																.push(Math
																		.abs(receivedPingTime[sn2]
																				- receivedPingTime[sn2 - 1]
																				- (pingIntervalServer[sn2] - pingIntervalServer[sn2 - 1])));
														downlink_jitter = Math
																.round(utilsClientQ4S
																		.mean(pingElapsed) * 100) / 100; // mean
														// of
														// the
														// elapsed
														// time
														// between
														// pings
														// reception
														latency = Math
																.round(utilsClientQ4S
																		.median(rtt) / 2);
														if (seq_received_pings.length >= N_OF_PINGS_CONTINUITY_CLIENT) {
															seq_received_pings
																	.shift(); // Remove
															// the
															// first
															// item
														}
														var seq2store = sn2++;
														seq_received_pings
																.push(seq2store); // store
														// sequence
														// number
														// for
														// calculating
														// packet
														// loss

														// console.log("--> 200
														// OK ["+sn2+"]:
														// receivedPingTime:"+receivedPingTime[sn2]+"
														// -
														// "+receivedPingTime[sn2-1]+";
														// pingIntervalServer:"+pingIntervalServer[sn2]+"
														// -
														// "+pingIntervalServer[sn2-1]);

														// console.log("--> 200
														// OK ["+sn2+"]:
														// pingElapsed:"+Math.abs(receivedPingTime[sn2]-receivedPingTime[sn2-1]-(pingIntervalServer[sn2]-pingIntervalServer[sn2-1]))+"
														// pingIntervalServer:"+(pingIntervalServer[sn2]-pingIntervalServer[sn2-1]));

													}
												}
												// console.log("--> 200 OK
												// ["+sn2+"]: time
												// ab2str:"+(timeAB2str-currTime)+"
												// str2ab time
												// ["+(time2-currTime)+"]
												// responseTime:"+(sentTime-currTime));
											});
						} else if (newString.indexOf(OK_HEADER) != -1) { // 200
							// OK
							// has
							// been
							// received
							rtt[sn2] = currTime - sentTimePing[sn2];
							// console.log("<-- 200 OK received
							// ["+currTime+"]:"+newString);
							// console.log("<-- 200 OK parsing time
							// ["+time1+"]");

							// console.log();
						} else if (newString.indexOf(BWIDTH_HEADER) != -1) { // BWIDTH
							// has
							// been
							// received
							bwMsgReceived[sn2] = currTime;
							bwMsgCounter++;
							downlinkBW = Math.round(8
									* BWIDTH_MESSAGE_LENGTH_SERVER
									* bwMsgCounter * 100
									/ (bwMsgReceived[sn2] - bwMsgReceived[0])) / 100;
							var expectedReceivedPackets = parseInt(sn2) + 1;
							downlink_packet_loss = Math
									.round((10000 * (expectedReceivedPackets - bwMsgCounter))
											/ expectedReceivedPackets) / 100;
							console.log("<-- BW Message received [" + currTime
									+ "]: " + sn2 + ";  "
									+ downlink_packet_loss);

							// console.log("downlink BW:"+ downlinkBW+"
							// counter:"+bwMsgCounter+" seqNumber:"+sn2+"
							// expected:"+expectedReceivedPackets+"
							// downlink_pl:"+downlink_packet_loss);
						}

						receivingDataQ4S(socketId); // recursively to receive
						// more data on the same
						// socket
					});
}

function ready2SendBWidthMessages() {
	wakeUpTime = new Date().getTime();

	// console.log("Loop:"+(wakeUpTime-bwOriginTime));
	counterSentPackets = 0;
	// packets2SendPerInterval =
	// Math.ceil((wakeUpTime-bwOriginTime)*packetsPerMs);

	// console.log("packets2SendPerInterval
	// ["+seq_number_bw_sent+"]:"+packets2SendPerInterval);
	sendBWidthMessages(packets2SendPerInterval);
}

function sendBWidthMessages(packets2Send) {

	counterSentPackets++;
	seq_number_bw_sent++;

	var measurements = MEASUREMENTS_HEADER + ": bw=";// +latency+",
	// j="+downlink_jitter
	measurements += ", pl=" + downlink_packet_loss;

	var bwMessage = BWIDTH_HEADER + "\n" + SESSION_ID_HEADER + ": " + sid
			+ "\n" + SEQUENCE_NUMBER_HEADER + ": " + seq_number_bw_sent + "\n"
			+ measurements + "\n" + CONTENT_LENGTH_HEADER + ": ";
	var contentLength = BWIDTH_MESSAGE_LENGTH - bwMessage.length - 4; // 4
	// bytes
	// for
	// content-length
	bwMessage += contentLength + "\n";

	var ab_bw = str2ab_1024length(bwMessage);

	socketUdpQ4S.write(socketId, ab_bw, function(writeInfo) {
		if (counterSentPackets === packets2Send) {
			// sleep;
			var currentTime = new Date().getTime();
			var timeo = BWIDTH_INTERVAL_TIMER - (currentTime - wakeUpTime);
			if (timeo < 0) {
				timeo = 0;
			}
			// console.log("Time after sending "+counterSentPackets+"
			// packets:"+(currentTime-wakeUpTime)+" timeo:"+timeo);

			bwOriginTime = currentTime;// new Date().getTime();
			if (seq_number_bw_sent < numOfPackets) {
				setTimeout(function() {
					ready2SendBWidthMessages();
				}, timeo);

			} else {
				console.log("Total time per " + (seq_number_bw_sent + 1)
						+ " BW messages:" + (currentTime - bwBeginningTime));
				setTimeout(function() {
					showEvent("Negotiation Stage 1 finished", 'BandWith:'
							+ uplinkBW + '/' + downlinkBW
							+ ' (kbps)  PacketLoss: ' + uplink_packet_loss
							+ '/' + downlink_packet_loss + ' (%)');
					// begin continuity stage
					// reset array with received pings
					seq_received_pings = [];
					continuity_Stage(sid);
				}, 2000);
			}
		} else {
			setTimeout(function() {
				sendBWidthMessages(packets2Send); // recursively
			}, 1);
		}
	});

}

function continuity_Stage(sid) {

	socket.emit('ready', 2, sid); // Continuity Stage
	socket.on('200 OK READY', function(stage, sessionId) {
		showEvent("Continuity Stage", "Begin continuous quality measurements");
		continuity_Pinging(sid);
		setInterval(function() {
			showEvent("Quality measurements", 'Latency: ' + latency
					+ ' ms; Jitter: ' + uplink_jitter + '/' + downlink_jitter
					+ ' PacketLoss: ' + uplink_packet_loss + '/'
					+ downlink_packet_loss + ' (%)');
		}, APP_ALERT_PAUSE);
	});
}

function continuity_Pinging(sid) {
	var seq_number_pings_sent = -1;
	var lastInterval;
	var last_ping_received;
	var ping_interval = setInterval(
			function() { // periodic launch of pings
				var intervalTime = new Date().getTime();

				seq_number_pings_sent++;
				if (seq_number_pings_sent > N_OF_PINGS_CONTINUITY_CLIENT) {
					// reset sequence number
					seq_number_pings_sent = 0;
				}
				var measurements = MEASUREMENTS_HEADER + ": l=" + latency
						+ ", j=" + downlink_jitter

				if (seq_received_pings.length > 0) {
					last_ping_received = seq_received_pings[seq_received_pings.length - 1];

					if (last_ping_received === N_OF_PINGS_CONTINUITY_CLIENT) {
						// last_ping_received++; //sequence begins at 0, but not
						// added because it has not been pushed into the stack
						// yet
						downlink_packet_loss = 100 - (100 * seq_received_pings.length / last_ping_received);
						measurements += ", pl=" + downlink_packet_loss;
						// reset array with received pings
						seq_received_pings = [];
					}
				}

				var ab = str2abQ4S(PING_HEADER2 + "\n" + SESSION_ID_HEADER
						+ ": " + sid + "\n" + SEQUENCE_NUMBER_HEADER + ": "
						+ seq_number_pings_sent + "\n" + measurements + "\n"
						+ TIMESTAMP_HEADER + ": " + intervalTime + "\n"
						+ CONTENT_LENGTH_HEADER + ": 0");
				var str2abTime = new Date().getTime();
				socketUdpQ4S.write(socketId, ab, function(writeInfo) {
					var currentTime = new Date().getTime();
					sentTimePing[seq_number_pings_sent] = currentTime;
					lastInterval = intervalTime;
				});

			}, PING_INTERVAL_CONTINUITY_CLIENT);

}
