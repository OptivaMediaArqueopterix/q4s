//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    ClientHttpq: PROGRAMA Cliente de Q- HTTP
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
package qhttp;

import java.lang.*;
import java.util.*;
import java.io.*;
import java.text.*;
import java.net.*;


	
public class ProtoQhttp implements Runnable{
	
//fichero de properties para configuracion	
Properties prop	;
static boolean CLIENT=false;
static boolean SERVER=false;

static SDP sdp_client;
static SDP sdp_server;
static int rtt_client=0;//rtt medido por el cliente
static int rtt_client_samples=0; //muestras que toma el cliente
static int rtt_server_samples=0; //muestras que toma el servidor
static int rtt_server=0; //rtt medido por el servidor
static int jitter_downlink=0;
static int jitter_uplink=0;

static int tcp_control_server_port;
static int udp_control_server_port;
static int tcp_control_client_port;
static int udp_control_client_port;

static String server_ip;
static String client_ip;

//-----------------------------------------------------------
//constructor
ProtoQhttp(String prop_file_name)
{
		cargaProperties(prop_file_name);
		server_ip=prop.getProperty("HS.IP_SERVER");
    client_ip=prop.getProperty("HS.IP_CLIENT");
}
//-----------------------------------------------------------	
public static void main(String args[])
{
if (args.length==0) 
  {
  	System.out.println("******************************************");
  	System.out.println("  ClientHttpq                 ");
  	System.out.println("  usage: java ClientHttpq -<type> <config.properties>");
    System.out.println("  <type> : c|s (client or server)");
 	 	System.out.println("  sample: java ClientHttpq -c miconfig.properties");
  	System.out.println("*******************************************");
  	
  	}
  	
String type= new String(args[0]);  	
System.out.println (type);
if (type.equals("-c")) CLIENT=true;
else if (type.equals("-s")) SERVER=true;

  	
String prop_file_name= new String(args[1]);  	
//System.out.println(prop_file_name);
//--------------------------------------------
ProtoQhttp client=new ProtoQhttp(prop_file_name);


Thread mi_thread=new Thread(client);
mi_thread.start();

// ahora me duermo y espero por una pulsacion de tecla
while(true)
	{
	try { Thread.sleep(60000);}catch(Exception e) {}
		
  
  /*System.out.println("******************************************");
  System.out.println("  menu                                    ");
  System.out.println("  1) restart                              ");
  System.out.println("  2) exit                                 ");

  System.out.println("******************************************");
  
  String tecla=leeTeclado();// funcion bloqueante
  if (tecla.equals("1") ) {mi_thread.stop();mi_thread.start();};
  if (tecla.equals("2") ) System.exit(0);
  */
  }//end while	
}//end main
//-----------------------------------------------------------	
public static String leeTeclado()
{
//util para test
String s=null;
try{
  BufferedReader t=new BufferedReader(new InputStreamReader(System.in));
  s=t.readLine();
  }catch(Exception e ){}	
return s;	
}
//------------------------------------------------------------
public static  void traza(String cad)
{
GregorianCalendar calendar=new GregorianCalendar();
String hh=new String(""+calendar.get(Calendar.HOUR));
String mm=new String(""+calendar.get(Calendar.MINUTE));
String ss=new String(""+calendar.get(Calendar.SECOND));

String cad2=new String("["+hh+":"+mm+":"+ss+"]"+"  "+cad);
System.out.println(cad2);
if ( (cad.length()>2) && (cad.substring(0,3).equals("@ $")) )
  {
  //logfile.writeLine(cad2);
  }
System.out.flush();	
}//end traza

//-----------------------------------------------------------------
public  void cargaProperties(String cadena)
{
	
	//String nombrefile=new String("./conf/"+cadena);
	String nombrefile=new String(cadena);

	traza (nombrefile);
	try{
       File propfile= new File(nombrefile);
	prop=new Properties();
	prop.load(new FileInputStream(propfile));
	}catch (Exception e){traza("error en file de properties");
	System.exit(0);
	}
  
  
   
}
//---------------------------------------------------------------------
//------------------------------------------------------------
//el metodo run es la maquina de estados de HTTPQ. este metodo debe implementar el protocolo
//para ello saca los parametros del fichero de properties
public void run()
	{
	//inicializacion de puertos a traves de fichero properties
	
	
	//handshake
	handshake();
		
	
	//lanzar negociacion
	negotiation(0); //la stage es para el cliente. el server no la usa, pues reacciona segun el READY
	if (CLIENT)
		{
		while (ClientQOS_Evaluation()==false) // pide instrucciones 
			{
			    QOSRequestClient();//pide qos a la red
			    negotiation(0)	;// negociacion stage 0
			          
		  }
		 negotiation(1);
    while (ClientQOS_Evaluation()==false) // pide instrucciones 
	{
	    QOSRequestClient();//pide qos a la red
	    negotiation(1)	;// negociacion stage 1
			          
	 }
	 
		  
		}
     if (SERVER) 
		{
		while (ServerQOS_Evaluation()==false) 
			{
				QOSRequestServer();
				negotiation(0)	;
			}
     negotiation(1)    ;
     while (ServerQOS_Evaluation()==false) 
	{
	QOSRequestServer();
	negotiation(1)	;
			}
		}
  
  //continuidad periodicamente, tanto cli como server
  traza("");
		traza("========================");
		traza ("ENTER IN CONTINUITY....");
		traza("========================");


  //continuity();
 //esto es una continuity barara (repito stage 0 y fuera...que chapuza...)
  if (CLIENT)
    {
	while (1==1)
	{
	 traza ("+---------------+");	
	 traza ("|  waiting...   |");	
	 traza ("+---------------+");	

	 
	 try {Thread.sleep(30000);}catch(Exception e) {}	
        negotiation(0)	;// negociacion stage 0
  	 while (ClientQOS_Evaluation()==false) // pide instrucciones 
	   {
	    QOSRequestClient();//pide qos a la red
	    negotiation(0)	;// negociacion stage 0
	   }
	}//while 1=1
    }//if client
    			
  if (SERVER)
   {
    while (1==1)
	{	
	negotiation(0); 
	while (ServerQOS_Evaluation()==false) 
	  {
	  QOSRequestServer();
	  negotiation(0)	;
	  }
	}//1=1
   }//server			

	}		 
//-------------------------------------------------------------------------------------------------------------
public void handshake()
  {
  	try{Thread.sleep(100);}catch(Exception e){};

   traza("========================");
   traza ("ENTER in HANDSHAKE...");
   traza("========================");

   if (SERVER) {handshakeServer();
   traza (" el cliente es:"+client_ip);} 
  else if (CLIENT)   { handshakeClient();}

  }
//-------------------------------------------------------------------------------------------------------------
public void handshakeClient()
  {
 	//-------------parte role cliente--------------------------------
 // traza ("ENTER in HANDSHAKE...");
 	String response=new String("");
 	
	// consruye mensaje GET
	//---------------------------
      String peticion="GET /handshake Q-HTTP/1.1\n";
            peticion+="Accept: */*\n";
            peticion+="Accept-Language: es\n";
            peticion+="Accept-Charset: 8859-1,utf-8\n";
            peticion+="Accept-Encoding: deflate\n";
            peticion+="User-Agent: qhttp-ua-experimental\n";
            peticion+="Host: 192.168.1.40\n";
            peticion+="Connection: close\n\n";//sin esta cabecera esto no funciona
            
      //ahora metemos el sdp
      String sdp=new String();
      SDP mi_sdp=new SDP();
      
      sdp=mi_sdp.creaClientDefaultSDP();
      
      //System.out.println(mi_sdp.getParameter("measurement","bandwidth",1));
      //System.out.println(mi_sdp.getParameter("bandwidth","",1));

      peticion+="Content-Length:" +sdp.length();
      if (sdp!=null) {peticion+="\n"+sdp;}

		                      
  //envia mensaje 
  //---------------
  int numero_puerto=Integer.parseInt(prop.getProperty("HS.PORT_TCP_SERVER"));
  Socket miCliente; 
  try {
  traza ("intentando conectar con server:"+server_ip+":"+numero_puerto);	
   miCliente = new Socket( prop.getProperty("HS.IP_SERVER"),numero_puerto ); //} catch( IOException e ) { System.out.println( e ); } 
  
  //stream de salida para enviar peticiones
  DataOutputStream salida; //try { 
  salida = new DataOutputStream( miCliente.getOutputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  System.out.println("\n========CLIENT===========");
  System.out.println(peticion);
  System.out.println("\n=========================\n");

  //stream de entrada, para recibir la respuesta del servidor. si no me doy prisa en abrirlo...perderé parte del mensaje
  //asi que lo abro antes de enviar
  DataInputStream entrada; 
  entrada = new DataInputStream( miCliente.getInputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  byte[] b=new byte[10000];//reservo 10kb para el mensaje de respuesta
  
  salida.write(peticion.getBytes());
	int a =0;
	while(a!=-1)
        { 
	  a =entrada.read(b);
	  if (a>0) response+=new String(b,0,(a-1),"UTF-8");
	  }
  //elimino los 2 bytes de longitud de la implementacion UTF de java
  response=response.substring(2);

  //imprimo la respuesta del server
  System.out.println("\n========SERVER===========\n"+response);
  System.out.println("\n=========================\n");

  
  salida.close(); entrada.close(); miCliente.close(); 
  } catch( IOException e ) { System.out.println( e ); System.exit(0);} 
  
  //actualiza parametros sdp con la respuesta recibida	
  int ini=response.indexOf("v=");
  if (ini==-1) {try{Thread.sleep(100);}catch(Exception e){};ini=response.indexOf("v=");}
  mi_sdp.cad=response.substring(ini);
  
  System.out.println("nuevo SDP recibido:\n"+mi_sdp.cad);
  sdp_client=mi_sdp;
  
  //tenemos que asignar los puertos                                        
  tcp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink TCP",0));
  udp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink UDP",0));
  tcp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink TCP",0));
  udp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink UDP",0));

  traza("tcp_control_server_port:"+tcp_control_server_port);
  traza("udp_control_server_port:"+udp_control_server_port);
  traza("tcp_control_client_port:"+tcp_control_client_port);
  traza("udp_control_client_port:"+udp_control_client_port);

  }//end handshakeclient

//-------------------------------------------------------------------------------------------
public void handshakeServer()
  {	
  //abrimos puerto TCP a escuchar. en un UA bien programado, ambos deben hacer esto para decidir quien es
  //finalmente el client y el server
   //Socket conexion;
        ServerSocket sck;
        BufferedReader datos;
        InputStream Entradadatos;
        String texto="";
        int cliente=0;
        int leido=0;
        sdp_server=new SDP();
        String cad="";
        String body="";
        String respuesta="";
        try
        {
            //Creamos un servidor
            ServerSocket conexion=new ServerSocket(88,2,InetAddress.getByName(prop.getProperty("HS.IP_SERVER")));
            DataInputStream entrada=null;
            DataOutputStream salida=null;
            byte  bytedata[]=new byte[10000];


            while (true)
            {
                ++cliente;
                Socket conex=conexion.accept();

                entrada=new DataInputStream(conex.getInputStream());
                System.out.println("Cliente num "+Integer.toString(cliente));
                InetAddress direc=conex.getInetAddress();
                System.out.println ("Dirección de llamada"+direc.getHostAddress());
                //IMPORTANTE: ASIGNO LA IP DE CLIENTE
                client_ip=direc.getHostAddress();
                cad="";
                leido=entrada.read(bytedata);
                if (leido>0) cad=new String(bytedata,0,(leido-1),"UTF-8");
                //Sacamos lo que hemos recibido desde el cliente
                System.out.println("Recibido desde cliente");
                System.out.println(cad);
                salida=new DataOutputStream(conex.getOutputStream());
                
                sdp_server.creaServerDefaultSDP();
                body=sdp_server.cad;
               
                if (cad.indexOf("/handshake")>=0)
                {
                    respuesta=new String("Q-HTTP/1.0 200 OK\n"+
                                         "Date: Mon, 5 Nov 2001 23:59:59 GMT\n"+
                                         "Content-Type: application/sdp\n"+
                                         "Content-Length:"+Integer.toString(body.length())+"\n\n"+body);
                    //respuesta+=body;
                    //Escribimos la respuesta al cliente
                    //try{Thread.sleep(1000);}catch(Exception e){}
                    //respuesta=new String (respuesta.getBytes(),"UTF-8");
                   //                    salida.writeBytes(new String(respuesta.getBytes(),"UTF-8"));
                    //System.out.println (respuesta);
                    //salida.writeBytes(respuesta);
                    
                    //esto mete 2 bytes mas al ppio porque java es asi
                    salida.writeUTF(respuesta);
                    //Writer out = new BufferedWriter(new OutputStreamWriter(conex.getOutputStream(), "UTF-8"));
                    //out.write(respuesta);	
            
                    
                    //salida.writeBytes(respuesta);
                    //salida.flush();
                }
                else
                {
                    respuesta="Q-HTTP/1.0 404 Error no encontrado\n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";
                    respuesta+="Content-Type: text/html\n";
                    respuesta+="Content-Length:"+Integer.toString( body.length())+"\n\n";
                    respuesta+="No se ha encontrado el documento";
                    //Escribimos la respuesta al cliente
                    
                    salida.writeUTF(respuesta);

                    //salida.writeBytes(new String(respuesta.getBytes(),"UTF-8"));
                    salida.flush();
                }


                //Cerramos el Socket
                conex.close();
                tcp_control_server_port=Integer.parseInt(sdp_server.getParameter("flow","control downlink TCP",0));
                udp_control_server_port=Integer.parseInt(sdp_server.getParameter("flow","control downlink UDP",0));
                tcp_control_client_port=Integer.parseInt(sdp_server.getParameter("flow","control uplink TCP",0));
                udp_control_client_port=Integer.parseInt(sdp_server.getParameter("flow","control uplink UDP",0));
                traza("tcp_control_server_port:"+tcp_control_server_port);
                traza("udp_control_server_port:"+udp_control_server_port);
                traza("tcp_control_client_port:"+tcp_control_client_port);
                traza("udp_control_client_port:"+udp_control_client_port);
                return;//si llegamos aqui, podemos pasar a la siguiente fase
            }

        }
        catch (IOException e)
        {
            System.out.println(e.toString());
            System.out.println("Error al conectar");
        }
  
  		
	
  //}
  


  }//end handshakeServer
//----------------------------------------------------
public void negotiation(int stage)
  {
	
  traza("==================================");		
  traza("ENTER in negotiation. Stage:"+stage);
  traza("==================================");		

  if (stage==0)
    {
    if (CLIENT) negotiationClientStage0();
    if (SERVER) readyServer();//en funcion del stage que llegue en el ready, ejecutará una stage u otra


    }
   if (stage==1)
    {
    if (CLIENT) negotiationClientStage1();
    if (SERVER) readyServer();//en funcion del stage que llegue en el ready, ejecutará una stage u otra


    }
  
  }
//----------------------------------------------------
public void continuity()
	{
		traza("");
		traza("========================");
		traza ("ENTER IN CONTINUITY....");
		traza("========================");

		
	}
	
//----------------------------------------------------------------------------------------------------------
 
public void negotiationClientStage0()
  {
	
   // primero se envia el READY y se espera al 200 OK
   
   readyClient(0);
      	
   // luego tenemos que enviar mensajes PING al servidor, puerto UDP durante 5 segundos
   // para el el servidor pueda medir
		
   // primero obtengo el bw uplink
   //String cadena=sdp_client.getParameter("bandwidth","",0);
   //int bwup=Integer.parseInt(cadena);
   //traza ("el bw up es:"+bwup);	
   
   //el numero de paquetes por seg es el bw pues está en kbps
   //final Integer samples_rtt=new Integer(0);
   int samples_jitter=0;
   	
   int msg_id=0;
   String peticion;
   final long[] tabla_client_pings=new long[1000];
 

  //creamos un socket para enviar UDP 
  //--------------------------------------
  //int numero_puerto=56000;//Integer.parseInt(prop.getProperty("HS.PORT_UDP_SERVER"));
 // int numero_puerto=udp_control_server_port;


  final DatagramSocket mi_cliente; 
  try { 
    mi_cliente = new DatagramSocket();
//   mi_cliente = new DatagramSocket(udp_control_client_port,InetAddress.getByName("localhost"));

    // numero_puerto ,InetAddress.getByName("localhost"));
  	//InetAddress.getByName(prop.getProperty("HS.IP_SERVER"))); 

 //programamos una tarea periodica de envío de PINGs
 //--------------------------------------------------
  Timer timer = new Timer();
  //timer.scheduleAtFixedRate(new TimerTask(){
  timer.schedule(new TimerTask(){

  	                              int msg_id=0;
  	                              public void run()
  	                  	            {
  	                  	           	try{
  	                  	             msg_id=msg_id+1;
  	                  	             if (msg_id>=120) cancel();
  	                  	             //if (rtt_client_samples>110) cancel();

  	                  	             //traza("enviando PING:"+msg_id);
  	                  	             
                                           String peticion="PING /stage0 Q-HTTP/1.1\n"+
                                                     "Session-id:53655765\n"+
                                                     "Message-id:"+msg_id+"\n"+
                                                     "Content-Length:0\n";
                 	                         DatagramPacket dato = new DatagramPacket(
                                             peticion.getBytes("UTF-8"), // El array de bytes
                                             peticion.length(), // Su longitud
                                             //InetAddress.getByName("localhost"),  // Destinatario
                                             InetAddress.getByName(server_ip),
                                             //56000);   // Puerto del destinatario
                                             udp_control_server_port);
	                                     //miCliente.send(dato);
	                                     Calendar c=new GregorianCalendar();

	                                     long a=c.getTimeInMillis();
                                           tabla_client_pings[msg_id]=a;

	                                     //new DatagramSocket().
	                                     	mi_cliente.send(dato);
	                                     //hay que rellenar la tabla de PINGs enviados
  	                  	           
                                           System.out.print("ping->, ");

  	                  	             }catch (Exception e){}
  	                  	            } //end run	
  	                  	      },//clase anonima timertask
	                       0,//initial delay
	                       1*50);  //subsequent rate

  } catch ( IOException e ) { System.out.println( "salida:"+e ); } //falla apertura salida
 

	
  //mientras enviamos PINGs, recibimos dos cosas: 200ok a nuestros pings y PINGs 
  //----------------------------------------------------------------------------- 
  //socket para escuchar 200 OK y PING
  DatagramSocket socket=null;
  int samples_ping=0;

  try{
  socket = new DatagramSocket(
   udp_control_client_port,
   //55000,
//   InetAddress.getByName("localhost"));
   InetAddress.getByName(client_ip));
//InetAddress.getByName("192.168.1.33"));

   
  DatagramPacket dato = new DatagramPacket(new byte[1000], 1000);
  int pings_resp_received=0;
  int rtt_sum=0;
  int rtt_avg=0;
  int last_msgid_ping_received=0;
  long[] last_timestamp_ping_received=new long[3]; 
  //int samples_ping=0;
  rtt_client_samples=0;
  long jitter=0;
  socket.setSoTimeout(1000);
  while(1==1)
    {
    	
     // saldremos de aqui si tenemos 100 samples
     //if (rtt_client_samples>100) break;
     	  
     socket.receive(dato);
     String msg=new String(dato.getData(),"UTF-8");
     Calendar c=new GregorianCalendar();
     long a=c.getTimeInMillis();
     
     if (msg.indexOf("200 OK")!=-1)
    	{
    	 //leer mensaje id y correlar
    	 int ini= msg.indexOf("Message-id:");
    	 int fin= msg.indexOf("\n",ini+1);
       String ids=msg.substring(ini+11,fin);
       //traza(ids);
    	 //la latencia (RTT) del paquete ha sido:
    	 int id=Integer.parseInt(ids);
    	 int rtt= (int) (a-tabla_client_pings[id]);
    	 rtt_client_samples+=1;
    	 if (rtt>100) {
    	 traza ("rtt:"+rtt);
    	 traza ("id:"+id);
    	 traza("tabla:"+tabla_client_pings[id]);	
    	 
    	 System.exit(0);	
    	 }
    	 

    	 //actualizar latencia	
    	 pings_resp_received+=1;
    	 rtt_sum=rtt_sum+rtt;
    	 rtt_avg=rtt_sum/pings_resp_received;
    	 
    	 rtt_client=rtt_avg;
    	 
    	 System.out.print("<-ok, ");
    	 //traza(""+id+":"+rtt+"   avg_RTT:"+rtt_avg);
    	} 
     else if (msg.indexOf("PING")!=-1)
    	{
    	 //System.out.print("P");	
    	 samples_ping+=1;
    	 //actualizar jitter downlink	
       
       long current_ET=a-last_timestamp_ping_received[0];
    	 long last_ET=last_timestamp_ping_received[0]-last_timestamp_ping_received[1];
    	 if (samples_ping>2)  	 
    	 	{
    	 		jitter=jitter+(Math.abs(current_ET-last_ET)-jitter)/16;
    	 	}
    	 jitter_downlink=(int)jitter;
    	 
    	 last_timestamp_ping_received[1]=last_timestamp_ping_received[0];
    	 last_timestamp_ping_received[0]=a;
    	 	
    	 
    	 //enviamos 200 OK
    	 int secondline=msg.indexOf("\n");
    	 String response="200 OK Q-HTTP/1.1\n"+msg.substring(secondline);
       DatagramPacket resp_packet = new DatagramPacket(
                                             response.getBytes(), // El array de bytes
                                             response.length(), // Su longitud
                                             InetAddress.getByName("localhost"),  // Destinatario
                                             udp_control_server_port); // Puerto del destinatario
                                             
	 new DatagramSocket().send(resp_packet);
       System.out.print("<-ping, ok->, ");	

    	}
    

    
    
    }//end while
    	
   
    
    	
  }catch (Exception e){
  	
  	if (e instanceof SocketTimeoutException) {
  		 //el cliente deja de contestar a los pings si dejan de llegar 
  		socket.close();
  		
  		 traza("-------------------------------------------------");
  		 traza("finalizada stage 0");
  		 traza("RTT:"+rtt_client+ " samples:"+rtt_client_samples);
  		 traza("jitter downlink:"+jitter_downlink+ " samples:"+samples_ping);
  		 traza("-------------------------------------------------");
  		 //modificamos el sdp cliente
             int latencia=rtt_client/2;
             sdp_client.setParameter("measurement","latency",0,""+latencia);
             sdp_client.setParameter("measurement","latency",1,""+latencia);
             sdp_client.setParameter("measurement","jitter",1,""+jitter_downlink);



  	}

  	
  	
  	}
  }//end negotiationclientstage0
//-----------------------------------------------------------------------------------------------------------
public void readyClient(int stage)
  {
 	//-------------parte role cliente--------------------------------
      
      System.out.println("READY?");
      leeTeclado();
      traza (" working....");
      
 	String response=new String("");
 	
	// consruye mensaje READY
	//---------------------------
      String peticion="READY /ready Q-HTTP/1.1\n";
            peticion+="Session-id:53655765\n";
            peticion+="Stage:"+stage+"\n";
            peticion+="User-Agent: qhttp-ua-experimental\n";
            peticion+="Host: 192.168.1.40\n";
            peticion+="Connection: close\n\n";//sin esta cabecera esto no funciona
            peticion+="Content-Length:0";
            
    

		                      
  //envia mensaje 
  //---------------
  int numero_puerto=tcp_control_server_port;
  Socket miCliente; try { miCliente = new Socket( prop.getProperty("HS.IP_SERVER"),numero_puerto ); //} catch( IOException e ) { System.out.println( e ); } 
  
  //stream de salida para enviar peticiones
  DataOutputStream salida; try { salida = new DataOutputStream( miCliente.getOutputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  System.out.println("\n========CLIENT===========");
  System.out.println(peticion);
  System.out.println("\n=========================\n");

  //salida.write(peticion.getBytes());
  salida.writeUTF(peticion);


  //stream de entrada, para recibir la respuesta del servidor
  DataInputStream entrada; try { entrada = new DataInputStream( miCliente.getInputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  byte[] b=new byte[10000];//reservo 10kb para el mensaje de respuesta
  try{
	int a =0;
	while(a!=-1){ a =entrada.read(b);
	if (a>0)	response+=new String(b,0,(a-1),"UTF-8");

	}
	}catch(EOFException e){}
  
  //quito bytes de utf-8
  response=response.substring(2);
  
  //imprimo la respuesta del server
  System.out.println("\n========SERVER===========\n"+response);
  System.out.println("\n=========================\n");

  //Por último se cierra todo
  try { salida.close(); entrada.close(); miCliente.close(); } catch( IOException e ) { System.out.println( e ); System.exit(0);} 
  } catch( IOException e ) { System.out.println( "entrada:"+e );System.exit(0); } //falla apertura entrada
  } catch( IOException e ) { System.out.println( "salida:"+e ); System.exit(0);} //falla apertura salida
  } catch( IOException e ) { System.out.println( "cliente:"+e ); System.exit(0);} //falla apertura cliente
  
  
  }//end ReadyClient
//-------------------------------------------------------------------------------------
public void readyServer()
  {
  	 int stage=0;
 	 ServerSocket sck;
        BufferedReader datos;
        InputStream Entradadatos;
        String texto="";
        int cliente=0;
        int leido=0;
        //SDP server_sdp=new SDP();
        try
        {
            //Creamos un servidor
            ServerSocket conexion=new ServerSocket(tcp_control_server_port,2,InetAddress.getByName(server_ip));
            DataInputStream entrada=null;
            DataOutputStream salida=null;
            String cad="",body="",respuesta="";
            byte  bytedata[]=new byte[256];


            while (true)
            {
                ++cliente;
                Socket conex=conexion.accept();

                entrada=new DataInputStream(conex.getInputStream());
                System.out.println("Cliente num "+Integer.toString(cliente));
                InetAddress direc=conex.getInetAddress();
                System.out.println ("Dirección de llamada"+direc.getHostAddress());
                cad="";
                leido=entrada.read(bytedata);
                if (leido>0)
                    cad=new String(bytedata,0,(leido-1),"UTF-8");
                //Sacamos lo que hemos recibido desde el cliente
                System.out.println("Recibido desde cliente");
                
                cad=cad.substring(2);
                System.out.println(cad);
                salida=new DataOutputStream(conex.getOutputStream());
                
               
               
                if (cad.indexOf("Stage:")>=0)
                {
                	 if (cad.indexOf("Stage:0") !=-1) stage=0;
                	 if (cad.indexOf("Stage:1") !=-1) stage=1;

                	 
                    respuesta="Q-HTTP/1.0 200 OK\n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";

                    respuesta+="Content-Type: application/sdp\n";
                    respuesta+="Content-Length:0\n\n";
                    //Escribimos la respuesta al cliente
                    
                    //salida.writeBytes(respuesta);
                    salida.writeUTF(respuesta);

                    salida.flush();
                }
                else
                {
                    respuesta="Q-HTTP/1.0 404 Error no encontrado\n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";

                    respuesta+="Content-Type: text/html\n";
                    respuesta+="Content-Length:"+Integer.toString( body.length())+"\n\n";
                    respuesta+="No se ha encontrado el documento";
                    //Escribimos la respuesta al cliente
                    salida.writeBytes(respuesta);
                    salida.flush();
                }


                //Cerramos el Socket que hemos usado para contestar
                conex.close();
                conexion.close();//y tambien el master

                if (stage==0) {negotiationServerStage0();break;}
                if (stage==1) {negotiationServerStage1();break;}

                //return;//si llegamos aqui, podemos pasar a la siguiente fase
            }

        }
        catch (IOException e)
        {
            System.out.println(e.toString());
            System.out.println("Error al conectar");
        }
  
  		
	
  //}
  

  
  }//end ReadyServer
//----------------------------------------------------------------------------------------------------------- 
public void negotiationServerStage1()
  {
   traza ("+---------------------------------------------------+");	
	 traza ("|  please wait.                                     |");
	 traza ("|    bandwidth measurement on going...              |");	
	 traza ("+---------------------------------------------------+");			
   int msg_id=0;
   String peticion;
   //final long[] tabla_server_datas=new long[1000];
   long interval_rate;
   int bw=Integer.parseInt(sdp_server.getParameter("bandwidth","",1));
   
    float bwf=(float)bw/8.0f; //asi son Kbytes/seg, osea paquetes/seg
    bwf=bwf*1.05f;//para asegurar que cumplimos subo un 5%
   
   //si bwf no es submultiplo de 100 ( 500, 250, 200, 100, 50,20, 10) no funciona el calculo del intervalo
   //y si es mayor de 1000 (8megas) sale siempre 1 ms, es decir 8Mbps.
   // para 2mbit--> 2000/8 = 250 ( es multiplo y sale bien)
   
   //otra opcion es mandar N paquetes con un intervalo mayor.
   //ejemplo
   //5000 kbps= 625 kBps (625 paquetes/seg)
   // 1000 % 625 =125  
   // 1000 / (625-125=500) =2 ( cada 2 ms un paquete para cumplir con los 500)
   // 1000 / 125 = 8 cada 8ms un paquete para cumplir con los 125
   // 8 *2 =16
   // cada 16 ms podemos mandar 16/2 + 16/8 = 10 paquetes
   //1000/16 =62,5 (osea, 62) x 10 paquetes =620 OK!!!!
   
   long resto = (int) (1000.0f % bwf);
   long intervalo1= (int) (1000.0f/ (bwf-(float)resto));
   long intervalo2=(int) (1000.0f/ (float)(resto));
   //final long intervalo=intervalo1*intervalo2;
   //final long N=intervalo/intervalo1+intervalo/intervalo2;
   
   
   final long intervalo=Math.min(intervalo1,intervalo2);
   final long intervalomayor=Math.max(intervalo1,intervalo2);

   final long N=1;
   
   traza( "intervalo:"+intervalo+"  intmayor="+intervalomayor);


   //
   //final long intervalo= (long)(1000.0f/bwf);// maximo 8 megas...si son 16 tendriamos 1000/2000 = 0
   
   

   final int t_total=Integer.parseInt(sdp_server.getParameter("measurement","procedure",2));
 //  traza ("intervalo:"+intervalo+" paquetes:"+N);
 //  traza ("intervalo:"+intervalo);
   traza ("tiempo:"+t_total);
   
traza("------------------------------------------");
 // traza (" r(intervalo):"+r+"  N:"+N+"  1+r*N="+(1+r*N));
  // leeTeclado();

  //creamos un socket para enviar UDP 
  //--------------------------------------
  //int numero_puerto=56000;//Integer.parseInt(prop.getProperty("HS.PORT_UDP_SERVER"));
 // int numero_puerto=udp_control_server_port;

 Timer timer = new Timer();
final String body=new String("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
  	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	

  final DatagramSocket mi_cliente; 
  try { 
    mi_cliente = new DatagramSocket();// numero_puerto ,InetAddress.getByName("localhost"));
  	//InetAddress.getByName(prop.getProperty("HS.IP_SERVER"))); 
//mi_cliente = new DatagramSocket(udp_control_server_port,InetAddress.getByName("localhost"));

 //programamos una tarea periodica de envío de PINGs
 //--------------------------------------------------
 // Timer timer = new Timer();
  timer.scheduleAtFixedRate(new TimerTask(){
  //timer.schedule(new TimerTask(){

  	                              int msg_id=0;
  	                             long t_ini=0;
  	                              long tiempo=0;
  	                              public void run()
  	                  	            {
  	                  	            try{	
  	                  	            DatagramSocket ds=new DatagramSocket()	;
	
  	                  	            for (int i=0;i<1;i++)
  	                  	            {	
  	                  	           
  	                  	             msg_id=msg_id+1;
  	                  	             //if (msg_id>=20*5) cancel();
  	                  	             //if (rtt_server_samples>=110) cancel();
  	                  	             
  	                  	             //el cliente va a enviar 120 pings por si se pierden algunos
							  // if (msg_id>120) cancel();
							  
  	                  	             //traza("enviando PING:"+msg_id);
  	                  	             
                                           String peticion="DATA /stage0 HTTP/1.1\n"+
                                                     "Session-id:53655765\n"+
                                                     "Message-id:"+msg_id+"\n"+
                                                     "Content-Length:XXXX\n\n"+body;
                 	                         DatagramPacket dato = new DatagramPacket(
                                             peticion.getBytes(), // El array de bytes
                                             peticion.length(), // Su longitud
                                             InetAddress.getByName(client_ip),
                                             //InetAddress.getByName("localhost"),  // Destinatario
                                             //56000);   // Puerto del destinatario
                                             udp_control_client_port);
	                                     //miCliente.send(dato);
	                                     Calendar c=new GregorianCalendar();

	                                     long a=c.getTimeInMillis();
                                           //tabla_server_data[msg_id]=a;
                                           if (msg_id==1) t_ini=a;
                                         

	                                     //new DatagramSocket().
	                                     	mi_cliente.send(dato);
	                                     	tiempo = tiempo+intervalo;

	                                     	 if (tiempo>=intervalomayor)
  	                  	            	{
  	                  	            	msg_id++;
  	                  	            	//habría que modificar dato, pero esto es un juguete....
  	                  	            	mi_cliente.send(dato);
	                                          tiempo=0;
  	                  	            	}

	                                     //ds.send(dato);
                                           if (a >(t_ini+t_total)) 

                                           	{
                                           	
                                           	traza("");
                                           	traza("YA NO ENVIO MAS DATA");
                                           	traza("");
                                           	traza("t_ini:"+t_ini);
                                           	traza("t_fin:"+a);	
                                           		 cancel();
                                           		 
                                           	}

	                                     //hay que rellenar la tabla de PINGs enviados
	                                     
                                           //System.out.print(">");
  	                  	           	
  	                  	             
  	                  	            }//N veces
  	                  	            	}catch (Exception e){}

  	                  	            } //end run	
  	                  	      },//clase anonima timertask
	                       0,//initial delay
	                       1*intervalo);  //subsequent rate in milliseconds

  } catch ( IOException e ) { System.out.println( "salida:"+e ); } //falla apertura salida
 

	
  //mientras enviamos DATAS, recibimos  DATAS, con los que sacamos en bw uplink 
  //----------------------------------------------------------------------------- 
  //socket para escuchar 200 OK y PING
  DatagramSocket socket=null;
   long samples_data=0;
long timestamp_first_data_received=0;
  long timestamp_last_data_received=0;

  try{
  socket = new DatagramSocket(
   udp_control_server_port,
   //55000,
  // InetAddress.getByName("localhost"));
    InetAddress.getByName(server_ip));

  DatagramPacket dato = new DatagramPacket(new byte[1000], 1000);
  //int pings_resp_received=0;
  //int rtt_sum=0;
  //int rtt_avg=0;
  //int last_msgid_ping_received=0;
  
//  int samples_ping=0;
  //rtt_server_samples=0;
  //long jitter=0;
 // socket.setSoTimeout(2000);//tiempo maximo de espera sin recibir. menor que el del cliente, para evitar problemas
  while(1==1)
    {
    	
     // saldremos de aqui si tenemos 100 samples...lo malo es que el cliente puede no haber llegado a 100
     // por tanto, mejor hay que parar esto cuando llegue el mensaje 110
     
     //if (rtt_server_samples>100) break;
     //socket.setSoTimeout(1000);
     //socket.setSoTimeout((int)(timestamp_last_data_received-timestamp_first_data_received+1000));
	 socket.setSoTimeout(2000);
     socket.receive(dato);// aqui se puede quedar pillado....2 segundos como mucho
     	

     String msg=new String(dato.getData());
     Calendar c=new GregorianCalendar();
     long a=c.getTimeInMillis();
     samples_data+=1;

     //if (msg.indexOf("DATA")!=-1)    	{
    	// System.out.print("<");	
    	 //samples_data+=1;
    	 //actualizar jitter downlink	
       if (samples_data==1) timestamp_first_data_received=a;
       timestamp_last_data_received=a;
       
     //  traza ("<");
    //	}
    

    
    
    }//end while
    	

    //ahora hemos terminado esta stage
    //debemos pedir instrucciones con los datos que tenemos
   // traza("finalizada stage 0");
    //ahora puede llegar GET desde el cliente para pedir instrucciones	
    //ServerQOS_Evaluation();
    		
  }catch (Exception e){
  	
  	if (e instanceof SocketTimeoutException) {
  		 //el cliente deja de contestar a los pings si dejan de llegar 
  		socket.close();
  		timer.cancel();

  		 long bw_uplink=8000*samples_data/(1+timestamp_last_data_received-timestamp_first_data_received);
  		 //ej 5000/1000 =5000kbytes/seg = 40Megas
  		 traza("-------------------------------------------------");
  		 traza("finalizada stage 1");
  		 traza("BW uplink:"+bw_uplink+ " samples:"+samples_data);
  		 traza("tiempo empleado:"+(1+(timestamp_last_data_received-timestamp_first_data_received)));
  		 traza("first packet:"+timestamp_first_data_received);
  		 traza("last  packet:"+timestamp_last_data_received);
  		 traza("-------------------------------------------------");
              //modificamos el sdp server
             int latencia=rtt_server/2;
             sdp_server.setParameter("measurement","bandwidth",0,""+bw_uplink);
             //paso del packet loss....esto es solo un programa de pruebas


  		 

  	  }
  	
  	}


  
  
  
  	
  }
//----------------------------------------------------------------------------------------------------------- 
  public void negotiationClientStage1()
  {
   int msg_id=0;
   String peticion;
   //final long[] tabla_server_datas=new long[1000];
   long interval_rate;
   int bw=Integer.parseInt(sdp_client.getParameter("bandwidth","",0));
   // bw está en kbit por segundo. el periodo es calculable solo si el max es 1000 kps
   //bw=bw/8 ; //ahora esta en kbytes/seg , osea en paquetes seg
   	
    float bwf=(float)bw/8.0f;
   bwf=bwf*1.05f;//para asegurar que cumplimos subo un 5%

  long resto = (int) (1000.0f % bwf);
   final long intervalo1= (int) (1000.0f/ (bwf-(float)resto));
   final long intervalo2=(int) (1000.0f/ (float)(resto));
   //final long intervalo=intervalo1*intervalo2;
  // final long N=intervalo/intervalo1+intervalo/intervalo2;

   final long intervalo=Math.min(intervalo1,intervalo2);
   final long intervalomayor=Math.max(intervalo1,intervalo2);

   final long N=1;
   traza( "intervalo:"+intervalo+"  intmayor="+intervalomayor);


  //traza ("int1="+intervalo1+" int2="+intervalo2);
   //final long intervalo= (long)(1000.0f/bwf);// maximo 8 megas...si son 16 tendriamos 1000/2000 = 0
   
   	
   	//traza ("intervalo:"+intervalo);
   	
   //	traza(sdp_client.getParameter("measurement","procedure",1));
 final int t_total=Integer.parseInt(sdp_client.getParameter("measurement","procedure",2));
   //traza ("intervalo:"+intervalo+" paquetes:"+N);

   	traza ("tiempo:"+t_total);
   
	
   readyClient(1);

 traza ("+---------------------------------------------------+");	
	 traza ("|  please wait.                                     |");
	 traza ("|    bandwidth measurement on going...              |");	
	 traza ("+---------------------------------------------------+");		
   

  //creamos un socket para enviar UDP 
  //--------------------------------------
  //int numero_puerto=56000;//Integer.parseInt(prop.getProperty("HS.PORT_UDP_SERVER"));
 // int numero_puerto=udp_control_server_port;

 Timer timer= new Timer();

  final DatagramSocket mi_cliente; 
  
  final String body=new String("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
  	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"+
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	
  	
  try { 
    mi_cliente = new DatagramSocket();// numero_puerto ,InetAddress.getByName("localhost"));
  	
  //	mi_cliente = new DatagramSocket(udp_control_client_port,InetAddress.getByName("localhost"));

  	//InetAddress.getByName(prop.getProperty("HS.IP_SERVER"))); 

 //programamos una tarea periodica de envío de PINGs
 //--------------------------------------------------
  //Timer timer= new Timer();

  timer.scheduleAtFixedRate(new TimerTask(){
  //timer.schedule(new TimerTask(){

  	                              int msg_id=0;
  	                              long t_ini=0;
  	                              long tiempo=0;
  	                              public void run()
  	                  	            {
  	                  	            try{
	                                     
  	                  	           // DatagramSocket ds=new DatagramSocket();
  	                  	            for (int i=0;i<1;i++)//habra que hacer esto para mandar n por miliseg.
  	                  	            {	
  	                  	           	
  	                  	             msg_id=msg_id+1;
  	                  	             //if (msg_id>=20*5) cancel();
  	                  	             //if (rtt_server_samples>=110) cancel();
  	                  	             
  	                  	             //el cliente va a enviar 120 pings por si se pierden algunos
							   // if (msg_id>120) cancel();
  	                  	             //traza("enviando PING:"+msg_id);
  	                  	             
                                           String peticion="DATA /stage0 HTTP/1.1\n"+
                                                     "Session-id:53655765\n"+
                                                     "Message-id:"+msg_id+"\n"+
                                                     "Content-Length:XXXX\n\n"+body;
                 	                         DatagramPacket dato = new DatagramPacket(
                                             peticion.getBytes(), // El array de bytes
                                             peticion.length(), // Su longitud
                                             InetAddress.getByName(server_ip),
                                             	//InetAddress.getByName("localhost"),  // Destinatario
                                             //56000);   // Puerto del destinatario
                                             udp_control_server_port);
	                                     //miCliente.send(dato);
	                                     Calendar c=new GregorianCalendar();

	                                     long a=c.getTimeInMillis();
                                           //tabla_server_data[msg_id]=a;
                                           
                                            if (msg_id==1) t_ini=a;
                                          
	                                     //ds.send(dato);
	                                     mi_cliente.send(dato);
	                                     tiempo = tiempo+intervalo;
	                                     if (tiempo>=intervalomayor)
  	                  	            	{
  	                  	            	msg_id++;
  	                  	            	//habría que modificar dato, pero esto es un juguete....
  	                  	            	mi_cliente.send(dato);
	                                          tiempo=0;
  	                  	            	}
							//mi_cliente.flush();
	                                     //hay que rellenar la tabla de PINGs enviados
	                                     if (a >(t_ini+t_total))
                                           	{
                                           	
                                           	traza("");
                                           	traza("YA NO ENVIO MAS DATA");
                                           	traza("");
                                           	traza("t_ini:"+t_ini);
                                           	traza("t_fin:"+a);	
                                           	 cancel();
                                           		 
                                           	}

                                           //System.out.print(">");
  	                  	           	
  	                  	             //}catch (Exception e){}
  	                  	            }//N veces
  	                  	            }catch (Exception e){}
	
  	                  	            } //end run	
  	                  	      },//clase anonima timertask
	                       0,//initial delay
	                       1*intervalo);  //subsequent rate in milliseconds

  } catch ( IOException e ) { System.out.println( "salida:"+e ); } //falla apertura salida
 

	
  //mientras enviamos DATAS, recibimos  DATAS, con los que sacamos en bw uplink 
  //----------------------------------------------------------------------------- 
  //socket para escuchar 200 OK y PING
  DatagramSocket socket=null;
   long samples_data=0;
long timestamp_first_data_received=0;
  long timestamp_last_data_received=0;

  try{
  socket = new DatagramSocket(
   udp_control_client_port,
   //55000,
   //InetAddress.getByName("localhost"));
   InetAddress.getByName(client_ip));

   
  DatagramPacket dato = new DatagramPacket(new byte[1000], 1000);
  //int pings_resp_received=0;
  //int rtt_sum=0;
  //int rtt_avg=0;
  //int last_msgid_ping_received=0;
  
//  int samples_ping=0;
  //rtt_server_samples=0;
  //long jitter=0;
  socket.setSoTimeout(3000);//tiempo maximo de espera  menor que el del cliente, para evitar problemas
 
  while(1==1)
    {
    
     // saldremos de aqui si tenemos 100 samples...lo malo es que el cliente puede no haber llegado a 100
     // por tanto, mejor hay que parar esto cuando llegue el mensaje 110
     
     //if (rtt_server_samples>100) break;
//socket.setSoTimeout((int)(timestamp_last_data_received-timestamp_first_data_received+1000));
     socket.setSoTimeout(3000);
     socket.receive(dato);// aqui se puede quedar pillado....
     
     	//if (dato.getdata().length()==0) break;
      //socket.setSoTimeout(3000);
     	
     
     String msg=new String(dato.getData());
     Calendar c=new GregorianCalendar();
     long a=c.getTimeInMillis();
      

     //if (msg.indexOf("DATA")!=-1)   	{
    	// System.out.print("<");	
    	samples_data+=1;
    	 //actualizar jitter downlink	
       if (samples_data==1) timestamp_first_data_received=a;
       timestamp_last_data_received=a;
       
      
    	//}
    

    
    
    }//end while
    	
    //ahora hemos terminado esta stage
    //debemos pedir instrucciones con los datos que tenemos
   // traza("finalizada stage 0");
    //ahora puede llegar GET desde el cliente para pedir instrucciones	
    //ServerQOS_Evaluation();
    		
  }catch (Exception e){
  	
  	if (e instanceof SocketTimeoutException) {
  		 //el cliente deja de contestar a los pings si dejan de llegar 
  		socket.close();
  		timer.cancel();
  		 long bw_downlink=8000*samples_data/(1+(timestamp_last_data_received-timestamp_first_data_received));
  		 //ej 5000/1000 =5000kbytes/seg = 40Megas
  		 traza("-------------------------------------------------");
  		 traza("finalizada stage 1");
  		 traza("BW downlink:"+bw_downlink+ " samples:"+samples_data);
  		 traza("tiempo empleado:"+(1+(timestamp_last_data_received-timestamp_first_data_received)));
  		 traza("first packet:"+timestamp_first_data_received);
  		 traza("last  packet:"+timestamp_last_data_received);
  		 traza("-------------------------------------------------");
              //modificamos el sdp server
             int latencia=rtt_server/2;
             sdp_client.setParameter("measurement","bandwidth",1,""+bw_downlink);
             //paso del packet loss....esto es solo un programa de pruebas


  		 

  	  }
  	
  	}


  
  
  
  	
  }
//----------------------------------------------------------------------------------------------------------- 

public void negotiationServerStage0()
  {
	
   // primero se envia el READY y se espera al 200 OK
   
   //readyClient(0);
   		
   // luego tenemos que enviar mensajes PING al servidor, puerto UDP durante 5 segundos
   // para el el servidor pueda medir
		
   // primero obtengo el bw uplink
   //String cadena=sdp_server.getParameter("bandwidth","",1);
   //int bwdown=Integer.parseInt(cadena);
   //traza ("el bw down es:"+bwdown);		
   //el numero de paquetes por seg es el bw pues está en kbps
   //final Integer samples_rtt=new Integer(0);
  
   	
   int msg_id=0;
   String peticion;
   final long[] tabla_server_pings=new long[1000];
 

  //creamos un socket para enviar UDP 
  //--------------------------------------
  //int numero_puerto=56000;//Integer.parseInt(prop.getProperty("HS.PORT_UDP_SERVER"));
 // int numero_puerto=udp_control_server_port;


  final DatagramSocket mi_cliente; 
  try { 
   mi_cliente = new DatagramSocket();// numero_puerto ,InetAddress.getByName("localhost"));
  	//InetAddress.getByName(prop.getProperty("HS.IP_SERVER"))); 
//mi_cliente = new DatagramSocket(udp_control_server_port,InetAddress.getByName("localhost"));

 //programamos una tarea periodica de envío de PINGs
 //--------------------------------------------------
  Timer timer = new Timer();
  //timer.scheduleAtFixedRate(new TimerTask(){
  timer.schedule(new TimerTask(){

  	                              int msg_id=0;
  	                              public void run()
  	                  	            {
  	                  	           	try{
  	                  	             msg_id=msg_id+1;
  	                  	             //if (msg_id>=20*5) cancel();
  	                  	             //if (rtt_server_samples>=110) cancel();
  	                  	             
  	                  	             //el cliente va a enviar 120 pings por si se pierden algunos
							                        if (msg_id>120) cancel();
  	                  	             //traza("enviando PING:"+msg_id);
  	                  	             
                                           String peticion="PING /handshake HTTP/1.1\n"+
                                                     "Session-id:53655765\n"+
                                                     "Message-id:"+msg_id+"\n"+
                                                     "Content-Length:0\n";
                 	                         DatagramPacket dato = new DatagramPacket(
                                             peticion.getBytes(), // El array de bytes
                                             peticion.length(), // Su longitud
                                             InetAddress.getByName(client_ip),
                                             //InetAddress.getByName("localhost"),  // Destinatario
                                             //56000);   // Puerto del destinatario
                                             udp_control_client_port);
	                                     //miCliente.send(dato);
	                                     Calendar c=new GregorianCalendar();

	                                     long a=c.getTimeInMillis();
                                           tabla_server_pings[msg_id]=a;

	                                     //new DatagramSocket().send(dato);
	                                     mi_cliente.send(dato);

	                                     //hay que rellenar la tabla de PINGs enviados
	                                     
                                           System.out.print("ping->, ");

  	                  	             }catch (Exception e){}
  	                  	            } //end run	
  	                  	      },//clase anonima timertask
	                       0,//initial delay
	                       1*50);  //subsequent rate

  } catch ( IOException e ) { System.out.println( "salida:"+e ); } //falla apertura salida
 

	
  //mientras enviamos PINGs, recibimos dos cosas: 200ok a nuestros pings y PINGs 
  //----------------------------------------------------------------------------- 
  //socket para escuchar 200 OK y PING
  DatagramSocket socket=null;
   int samples_ping=0;

  try{
  socket = new DatagramSocket(
   udp_control_server_port,
   //55000,
   //InetAddress.getByName("localhost"));
   InetAddress.getByName(server_ip));

   
  DatagramPacket dato = new DatagramPacket(new byte[1000], 1000);
  int pings_resp_received=0;
  int rtt_sum=0;
  int rtt_avg=0;
  int last_msgid_ping_received=0;
  long[] last_timestamp_ping_received=new long[3]; 
//  int samples_ping=0;
  rtt_server_samples=0;
  long jitter=0;
  socket.setSoTimeout(300);//tiempo maximo de espera sin recibir. menor que el del cliente, para evitar problemas
  while(1==1)
    {
    	
     // saldremos de aqui si tenemos 100 samples...lo malo es que el cliente puede no haber llegado a 100
     // por tanto, mejor hay que parar esto cuando llegue el mensaje 110
     
     //if (rtt_server_samples>100) break;
     	 
     socket.receive(dato);// aqui se puede quedar pillado....
     String msg=new String(dato.getData());
     Calendar c=new GregorianCalendar();
     long a=c.getTimeInMillis();
     
     if (msg.indexOf("200 OK")!=-1)
    	{
    	 //leer mensaje id y correlar
    	 int ini= msg.indexOf("Message-id:");
    	 int fin= msg.indexOf("\n",ini+1);
       String ids=msg.substring(ini+11,fin);
       //traza(ids);
    	 //la latencia (RTT) del paquete ha sido:
    	 int id=Integer.parseInt(ids);
    	 int rtt= (int) (a-tabla_server_pings[id]);
    	 rtt_server_samples+=1;
    	 
    	 
    	 if (rtt>100) {
    	 traza ("rtt:"+rtt);
    	 traza ("id:"+id);
    	 traza("tabla:"+tabla_server_pings[id]);	
    	 traza("a:"+a);
    	 traza("tabla:"+tabla_server_pings[id-1]);
    	 System.exit(0);	
    	 }
    	 

    	 //actualizar latencia	
    	 pings_resp_received+=1;
    	 rtt_sum=rtt_sum+rtt;
    	 rtt_avg=rtt_sum/pings_resp_received;
    	 
    	 rtt_server=rtt_avg;
    	 
    	 System.out.print("<-ok, ");
    	 //traza(""+id+":"+rtt+"   avg_RTT:"+rtt_avg+" samples:"+rtt_server_samples);
    	} 
     else if (msg.indexOf("PING")!=-1)
    	{
    	// System.out.print("P");	
    	 samples_ping+=1;
    	 //actualizar jitter downlink	
       
       long current_ET=a-last_timestamp_ping_received[0];
    	 long last_ET=last_timestamp_ping_received[0]-last_timestamp_ping_received[1];
    	 if (samples_ping>2)  	 jitter=jitter+(Math.abs(current_ET-last_ET)-jitter)/16;
    	 
    	 //traza ("jitter:"+jitter);
    	 jitter_uplink=(int)jitter;
    	 last_timestamp_ping_received[1]=last_timestamp_ping_received[0];
    	 last_timestamp_ping_received[0]=a;
    	 
    	 	

    	 //Jitter = Jitter + (abs( ElapsedTime - OldElapsedTime ) - Jitter) / 16
    	 
    	 //enviamos 200OK
    	 int secondline=msg.indexOf("\n");
    	 String response="Q-HTTP/1.1 200 OK\n"+msg.substring(secondline);
       DatagramPacket resp_packet = new DatagramPacket(
                                             response.getBytes("UTF-8"), // El array de bytes
                                             response.length(), // Su longitud
                                             InetAddress.getByName("localhost"),  // Destinatario
                                             udp_control_client_port);
                                             //56000);   // Puerto del destinatario
	 new DatagramSocket().send(resp_packet);
       
       int num_ini=msg.indexOf("Message-id");
       String cad2=msg.substring(num_ini	);
       int num_fin=cad2.indexOf("\n");
       String cad3=cad2.substring(11,num_fin);
       //traza ("id:"+cad3);
       int id=Integer.parseInt(cad3);	
       //el cliente deja de contestar a los pings si dejan de llegar 
      // if (id>110) break;
       	
        System.out.print("<-ping, ok->, ");                                      
       //traza ("ping received:"+jitter);
    	}
    

    
    
    }//end while
    	
    //ahora hemos terminado esta stage
    //debemos pedir instrucciones con los datos que tenemos
   // traza("finalizada stage 0");
    //ahora puede llegar GET desde el cliente para pedir instrucciones	
    //ServerQOS_Evaluation();
    		
  }catch (Exception e){
  	
  	if (e instanceof SocketTimeoutException) {
  		 //el cliente deja de contestar a los pings si dejan de llegar 
  		socket.close();
  		 traza("-------------------------------------------------");
  		 traza("finalizada stage 0");
  		 traza("RTT:"+rtt_server+ " samples:"+rtt_server_samples);
  		 traza("jitter uplink:"+jitter_uplink+ " samples:"+samples_ping);
  		 traza("-------------------------------------------------");
              //modificamos el sdp server
             int latencia=rtt_server/2;
             sdp_server.setParameter("measurement","latency",0,""+latencia);
             sdp_server.setParameter("measurement","latency",1,""+latencia);
             sdp_server.setParameter("measurement","jitter",0,""+jitter_uplink);



  		 

  	  }
  	
  	}

 
  }//end negotiationclientstage0
//------------------------------------------------------------------------------------------------
public boolean ServerQOS_Evaluation()
{
traza("entrada en server QOS. esperando GET")	;
	//return true;
//recibimos un GET con SDP
   ServerSocket sck;
        BufferedReader datos;
        InputStream Entradadatos;
        String texto="";
        int cliente=0;
        int leido=0;
        //sdp_server=new SDP();   
        try
        {
            //Creamos un servidor
            ServerSocket conexion=new ServerSocket(tcp_control_server_port,2,InetAddress.getByName(prop.getProperty("HS.IP_SERVER")));
            DataInputStream entrada=null;
            DataOutputStream salida=null;
            String cad="",body="",respuesta="";
            byte  bytedata[]=new byte[1256];


            while (true)
            {
                ++cliente;
                Socket conex=conexion.accept();

                entrada=new DataInputStream(conex.getInputStream());
                System.out.println("Cliente num "+Integer.toString(cliente));
                InetAddress direc=conex.getInetAddress();
                System.out.println ("Dirección de llamada"+direc.getHostAddress());
                cad="";
                leido=entrada.read(bytedata);
                if (leido>0) cad=new String(bytedata,0,(leido-1),"UTF-8");
                //Sacamos lo que hemos recibido desde el cliente
                //cad=cad.substring(2);
                System.out.println("Recibido desde cliente");
                System.out.println(cad);
                
                //el sdp recibido debe contener las medidas de cliente
                String sdp_aux=cad.substring(cad.indexOf("v=0"));
                SDP aux=new SDP();
                aux.cad=sdp_aux;
                 int jd=Integer.parseInt(aux.getParameter("measurement","jitter",1));
                 sdp_server.setParameter("measurement","jitter",1,""+jd);

                 int bwdw=Integer.parseInt(aux.getParameter("measurement","bandwidth",1));
                 sdp_server.setParameter("measurement","bandwidth",1,""+bwdw);

             

                
                salida=new DataOutputStream(conex.getOutputStream());
                
                //server_sdp
                //body=new String(sdp_server.creaServerDefaultSDP());
                body=new String(sdp_server.cad);
                
                //ahora decidimos que enviamos 412 o 200
                traza(sdp_server.cad);
                boolean result=sdp_server.reachConstraints();
                
                traza("result:"+result);
                if (result==true)
            	{
                    respuesta="Q-HTTP/1.0 200 OK\n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";
                    respuesta+="Content-Type: application/sdp\n";
                    respuesta+="Content-Length:"+Integer.toString(body.length())+"\n\n";
                    respuesta+=body;
                    //Escribimos la respuesta al cliente
                    //salida.writeBytes(respuesta);
                    salida.writeUTF(respuesta);

                    
                    salida.flush();
                    //traza ("respuesta:\n"+respuesta);
                    
                }
                else
                {
                	  //primero subimos el nivel. subo los dos por que esto es una prueba de concepto
                	  //habría que subir el que haya fallado
                	  
                	  int levelup=Integer.parseInt(sdp_server.getParameter("qos-level","",0));
                	  int leveldw=Integer.parseInt(sdp_server.getParameter("qos-level","",1));
                    
                    sdp_server.setParameter("qos-level","",0,""+(levelup+1));
                    sdp_server.setParameter("qos-level","",1,""+(leveldw+1));
                    
                	  body=new String(sdp_server.cad);
	
                    respuesta="Q-HTTP/1.0 412 latency or trigger \n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";
                     respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";
                    respuesta+="Content-Type: application/sdp\n";
                    respuesta+="Content-Length:"+Integer.toString(body.length())+"\n\n";
                    respuesta+=body;
                    salida.writeUTF(respuesta);
                    salida.flush();
                }


                //Cerramos el Socket
                conex.close();
               conexion.close();
                return result;//si llegamos aqui, podemos pasar a la siguiente fase
            }

        }
        catch (IOException e)
        {
            System.out.println(e.toString());
            System.out.println("Error al conectar");
        }
  
  		
	
  //}
  



//aqui nunca llega

return true;	
  	
}//end
//------------------------------------------------------------------------------------------------
public boolean ClientQOS_Evaluation()
{
//este metodo lanza GET para pedir instrucciones

traza("Enter en clientQOS: envio de GET para pedir instrucciones");

//enviamos GET
String response=new String("");
 	
	// consruye mensaje GET
	//---------------------------
      String peticion="GET /instructions Q-HTTP/1.0\n";
            peticion+="Accept: */*\n";
            peticion+="Accept-Language: es\n";
            peticion+="Accept-Charset: 8859-1,utf-8\n";
            peticion+="Accept-Encoding: deflate\n";
            peticion+="User-Agent: qhttp-ua-experimental\n";
            peticion+="Host: 192.168.1.40\n";
            peticion+="Connection: close\n\n";//sin esta cabecera esto no funciona
            
      //ahora metemos el sdp
      String sdp=new String();
      
      
      sdp=sdp_client.cad;
      
      //System.out.println(mi_sdp.getParameter("measurement","bandwidth",1));
      //System.out.println(mi_sdp.getParameter("bandwidth","",1));

      peticion+="Content-Length:" +sdp.length();
      if (sdp!=null) {peticion+="\n"+sdp;}

		                      
  //envia mensaje 
  //---------------
  int numero_puerto=tcp_control_server_port;
  Socket miCliente; try { miCliente = new Socket( prop.getProperty("HS.IP_SERVER"),numero_puerto ); //} catch( IOException e ) { System.out.println( e ); } 
  
  //stream de salida para enviar peticiones
  DataOutputStream salida; try { salida = new DataOutputStream( miCliente.getOutputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  System.out.println("\n========CLIENT===========");
  System.out.println(peticion);
  System.out.println("\n=========================\n");

  salida.write(peticion.getBytes("UTF-8"));

  //stream de entrada, para recibir la respuesta del servidor
  DataInputStream entrada; try { entrada = new DataInputStream( miCliente.getInputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  byte[] b=new byte[10000];//reservo 10kb para el mensaje de respuesta
  try{
	int a =0;
	while(a!=-1){ a =entrada.read(b);
	if (a>0)	response+=new String(b,0,(a-1),"UTF-8");

	}
	}catch(EOFException e){}
  
  response=response.substring(2);
  //imprimo la respuesta del server
  System.out.println("\n========SERVER===========\n"+response);
  System.out.println("\n=========================\n");

  //Por último se cierra todo
  try { salida.close(); entrada.close(); miCliente.close(); } catch( IOException e ) { System.out.println( e ); System.exit(0);} 
  } catch( IOException e ) { System.out.println( "entrada:"+e ); System.exit(0);} //falla apertura entrada
  } catch( IOException e ) { System.out.println( "salida:"+e ); System.exit(0);} //falla apertura salida
  } catch( IOException e ) { System.out.println( "cliente:"+e ); System.exit(0);} //falla apertura cliente
  
  //actualiza parametros sdp con la respuesta recibida	
  int ini=response.indexOf("v=");
   if (ini==-1) {try{Thread.sleep(100);}catch(Exception e){};ini=response.indexOf("v=");}


  sdp_client.cad=response.substring(ini);
  
  System.out.println("nuevo SDP recibido:\n"+sdp_client.cad);
  
  
  //tenemos que asignar los puertos                        
  traza(sdp_client.cad);                
  tcp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink TCP",0));
  udp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink UDP",0));

  tcp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink TCP",0));
  udp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink UDP",0));

 
//recibimos si es 200 entonces fin, pero si es 412, entonces mandamos QOSrequest , esperamos respuesta
if (response.indexOf("200 OK") >0) return true;
else return false;//retornamos false si nos llegó 412
  	
}//end
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
public void QOSRequestClient()
{
	//return true;
//primero arreglamos el SDP con los datos que hemos medido


//enviamos GET
String response=new String("");
 	
	// consruye mensaje GET
	//---------------------------
      String peticion="QOS-REQUEST /qos Q-HTTP/1.0\n";
            peticion+="Accept: */*\n";
            peticion+="Accept-Language: es\n";
            peticion+="Accept-Charset: 8859-1,utf-8\n";
            peticion+="Accept-Encoding: deflate\n";
            peticion+="User-Agent: qhttp-ua-experimental\n";
            peticion+="Host: 192.168.1.40\n";
            peticion+="Connection: close\n\n";//sin esta cabecera esto no funciona
            
      //ahora metemos el sdp
      String sdp=new String();
      
      
      sdp=sdp_client.cad;
      
      //System.out.println(mi_sdp.getParameter("measurement","bandwidth",1));
      //System.out.println(mi_sdp.getParameter("bandwidth","",1));

      peticion+="Content-Length:" +sdp.length();
      if (sdp!=null) {peticion+="\n"+sdp;}

		                      
  //envia mensaje 
  //---------------
  int numero_puerto=tcp_control_server_port;
  Socket miCliente; try { miCliente = new Socket( prop.getProperty("HS.IP_SERVER"),numero_puerto ); //} catch( IOException e ) { System.out.println( e ); } 
  
  //stream de salida para enviar peticiones
  DataOutputStream salida; try { salida = new DataOutputStream( miCliente.getOutputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  System.out.println("\n========CLIENT===========");
  System.out.println(peticion);
  System.out.println("\n=========================\n");

  salida.write(peticion.getBytes("UTF-8"));

  //stream de entrada, para recibir la respuesta del servidor
  DataInputStream entrada; try { entrada = new DataInputStream( miCliente.getInputStream() ); //} catch( IOException e ) { System.out.println( e ); } 
  byte[] b=new byte[10000];//reservo 10kb para el mensaje de respuesta
  try{
	int a =0;
	while(a!=-1){ a =entrada.read(b);
	if (a>0)	response+=new String(b,0,(a-1),"UTF-8");

	}
	}catch(EOFException e){}
  
  response=response.substring(2);
  //imprimo la respuesta del server
  System.out.println("\n========SERVER===========\n"+response);
  System.out.println("\n=========================\n");

  //Por último se cierra todo
  try { salida.close(); entrada.close(); miCliente.close(); } catch( IOException e ) { System.out.println( e ); System.exit(0);} 
  } catch( IOException e ) { System.out.println( "entrada:"+e ); System.exit(0);} //falla apertura entrada
  } catch( IOException e ) { System.out.println( "salida:"+e ); System.exit(0);} //falla apertura salida
  } catch( IOException e ) { System.out.println( "cliente:"+e ); System.exit(0);} //falla apertura cliente
  
  //actualiza parametros sdp con la respuesta recibida	
  int ini=response.indexOf("v=");
   if (ini==-1) {try{Thread.sleep(100);}catch(Exception e){};ini=response.indexOf("v=");}


  sdp_client.cad=response.substring(ini);
  
  //System.out.println("nuevo SDP recibido:\n"+sdp_client.cad);
  
  
  //tenemos que asignar los puertos                        
  traza(sdp_client.cad);                
  tcp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink TCP",0));
  udp_control_server_port=Integer.parseInt(sdp_client.getParameter("flow","control downlink UDP",0));

  tcp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink TCP",0));
  udp_control_client_port=Integer.parseInt(sdp_client.getParameter("flow","control uplink UDP",0));
  
  //vamos a esperar el tiempo que diga guard-time
  //suponemos  que son 5 segundos
  traza ("");
  traza(" Esperando guard-time.....");
try{Thread.sleep(5000);}catch(Exception e){};
  	
}//end
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
public void QOSRequestServer()
{
traza("entrada en server QOS. esperando GET")	;
	//return true;
//recibimos un GET con SDP
   ServerSocket sck;
        BufferedReader datos;
        InputStream Entradadatos;
        String texto="";
        int cliente=0;
        int leido=0;
        //sdp_server=new SDP();   
        try
        {
            //Creamos un servidor
            ServerSocket conexion=new ServerSocket(tcp_control_server_port,2,InetAddress.getByName(prop.getProperty("HS.IP_SERVER")));
            DataInputStream entrada=null;
            DataOutputStream salida=null;
            String cad="",body="",respuesta="";
            byte  bytedata[]=new byte[1256];


            while (true)
            {
                ++cliente;
                Socket conex=conexion.accept();

                entrada=new DataInputStream(conex.getInputStream());
                System.out.println("Cliente num "+Integer.toString(cliente));
                InetAddress direc=conex.getInetAddress();
                System.out.println ("Dirección de llamada"+direc.getHostAddress());
                cad="";
                leido=entrada.read(bytedata);
                if (leido>0) cad=new String(bytedata,0,(leido-1),"UTF-8");
                //Sacamos lo que hemos recibido desde el cliente
                //cad=cad.substring(2);
                System.out.println("Recibido desde cliente");
                System.out.println(cad);
                
              
             

                
                salida=new DataOutputStream(conex.getOutputStream());
                
                //server_sdp
                //body=new String(sdp_server.creaServerDefaultSDP());
                body=new String(sdp_server.cad);
                
                
                    respuesta="QOS-REQUEST /qos Q-HTTP/1.0\n";
                    respuesta+="Date: Mon, 5 Nov 2001 23:59:59 GMT\n";
                    respuesta+="Content-Type: application/sdp\n";
                    respuesta+="Content-Length:"+Integer.toString(body.length())+"\n\n";
                    respuesta+=body;
                    //Escribimos la respuesta al cliente
                    //salida.writeBytes(respuesta);
                    salida.writeUTF(respuesta);

                    
                    salida.flush();
                    //traza ("respuesta:\n"+respuesta);
                    
                
                //Cerramos el Socket
                conex.close();
               conexion.close();
                //return result;//si llegamos aqui, podemos pasar a la siguiente fase
            }

        }
        catch (IOException e)
        {
            System.out.println(e.toString());
            System.out.println("Error al conectar");
        }
  
  		
	
  //}
  





//completamos un SDP

//decidimos la respuesta 200 o 412

//si enviamos 412 entonces esperamos el qos request y lo contestamos
//return true;	
  	
}//end
//------------------------------------------------------------------------------------------------

	
}//end class QHTTPclient