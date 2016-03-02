//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    SDP: 
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
package qhttp;

import java.lang.*;
import java.util.*;
import java.io.*;
import java.text.*;
import java.net.*;


public class SDP {

String cad;

public String toString(){return "";}
//--------------------------------------------------------------------------------------
public String getParameter(String atributo, String parametro, int posicion){
	
	int ini=cad.indexOf("a="+atributo+":"+parametro);
  int fin=cad.indexOf("a=",ini+1);//el +1 es para que no me salga igual que ini

//System.out.println("ini:"+ini+" fin:"+fin);
//System.out.println(cad.substring(ini+24,fin));
  String aux=new String("a="+atributo+":"+parametro);
	//String cad2=cad.substring(ini+aux.length()+1,fin);
	String cad2=cad.substring(ini+aux.length(),fin);
 cad2.replace(" ","");
 
	StringTokenizer tok=new StringTokenizer(cad2, "/");
	String value=new String("");
	for (int i=0;i<=posicion;i++) value=tok.nextToken();
	  value=value.replace(" ","");
	if (value.indexOf("\n")!=-1) value=value.substring(0,value.indexOf("\n"));
	 //value.replace(" ","");
	return value;
	}
//--------------------------------------------------------------------------------------	
public String setParameter(String atributo, String parametro, int posicion, String valor){
	
  int ini=cad.indexOf("a="+atributo+":"+parametro);
  int fin=cad.indexOf("a=",ini+1);//el +1 es para que no me salga igual que ini
  
  String cad_head=cad.substring(0,ini);
  String cad_cola=cad.substring(fin,cad.length());
  String linea=new String("a="+atributo+":"+parametro+" ");
  
  String aux=new String("a="+atributo+":"+parametro);
  String cad2=cad.substring(ini+aux.length(),fin);
  cad2=cad2.replace(" ","");
  
  StringTokenizer tok=new StringTokenizer(cad2, "/"); //cad2 tiene la cadena de valores
	String value=new String("");

int i=-1;
int maxpos=0;
while (tok.hasMoreTokens() ){
	value=tok.nextToken();
	i++;
	if (i>0) linea+="/";
	if (i==posicion) { linea+=valor;}
	else linea+=value;
	

      
}
if (i==posicion) {//era el ultimo
	linea+="\n";}
	cad=cad_head+linea+cad_cola;
	
	
	
	return cad;}
	
	
//--------------------------------------------------------------------------------------
public boolean reachConstraints()
	{
		
		int LUC=Integer.parseInt(getParameter("latency","",0));
    int LDC=Integer.parseInt(getParameter("latency","",1));
    int TUC=Integer.parseInt(getParameter("jitter","",0));
    int TDC=Integer.parseInt(getParameter("jitter","",1));
  
    int TUM=Integer.parseInt(getParameter("measurement","jitter",0));
    int TDM=Integer.parseInt(getParameter("measurement","jitter",1));
    int LUM=Integer.parseInt(getParameter("measurement","latency",0));
    int LDM=Integer.parseInt(getParameter("measurement","latency",1));
    
    
                           
    System.out.println("LaUpC:"+LUC+" LaUpM:"+LUM);
    System.out.println("LaDwC:"+LDC+" LaDwM:"+LDM);
    System.out.println("TrUpC:"+TUC+" TrUpM:"+TUM);
    System.out.println("TrDwC:"+TDC+" TrDwM:"+TDM);
    //-----------
    int BWUC=Integer.parseInt(getParameter("bandwidth","",0));
    int BWDC=Integer.parseInt(getParameter("bandwidth","",1));
    int BWUM=Integer.parseInt(getParameter("measurement","bandwidth",0));
    int BWDM=Integer.parseInt(getParameter("measurement","bandwidth",1));
System.out.println("BWUC:"+BWUC+" BWUM:"+BWUM);
    System.out.println("BWDC:"+BWDC+" BWDM:"+BWDM);

                           
    if  (LUM>LUC) return false;
    if  (LDM>LDC) return false;
    if  (TUM>TUC) return false;
    if  (TDM>TDC) return false;
    
    if (BWUC>BWUM) return false;
    if (BWDC>BWDM) return false;

System.out.println ("sale true");
    return true;




		
		
	}
//--------------------------------------------------------------------------------------
public String creaClientDefaultSDP()
	{
		cad=new String("v=0\n"
                  + "o=q-http-UA 53655765 2353687637 IN IP4 10.47.16.5\n"+
"s=Q-HTTP\n"+
"i=Q-HTTP parameters\n"+
"t=0 0\n"+
"a=qos-level:0/0\n"+
"a=latency:40/35\n"+
"a=jitter:10/10\n"+
"a=bandwidth:20/5000\n"+
"a=packetloss:5/5\n"+
"a=flow:data downlink TCP/10000-20000\n"+
"a=flow:control downlink UDP/55000\n"+
"a=flow:control downlink TCP/55001\n"+
"a=flow:data uplink TCP/56000\n"+
"a=flow:control uplink UDP/56000\n"+
"a=flow:control uplink TCP/56001\n"+
"a=measurement:procedure default/50/5000\n"+
"a=measurement:latency 10000/10000\n"+
"a=measurement:jitter 10000/10000\n"+
"a=measurement:bandwidth 100000/100000\n"+
"a=measurement:packetloss 0/0");
		
		
	return cad;	

	}
//---------------------------------------------------------------------------	
public String creaServerDefaultSDP()
	{
		cad=new String("v=0\n"
                  + "o=q-http-UA 53655765 2353687637 IN IP4 10.47.16.5\n"+
"s=Q-HTTP\n"+
"i=Q-HTTP parameters\n"+
"t=0 0\n"+
"a=qos-level:0/0\n"+
"a=latency:40/35\n"+
"a=jitter:0/0\n"+
"a=bandwidth:20/5000\n"+
"a=packetloss:5/5\n"+
"a=flow:data downlink TCP/10000-20000\n"+
"a=flow:control downlink UDP/55000\n"+
"a=flow:control downlink TCP/55001\n"+
"a=flow:data uplink TCP/56000\n"+
"a=flow:control uplink UDP/56000\n"+
"a=flow:control uplink TCP/56001\n"+
"a=measurement:procedure default/50/5000\n"+
"a=measurement:latency 10000/10000\n"+
"a=measurement:jitter 10000/10000\n"+
"a=measurement:bandwidth 100000/100000\n"+
"a=measurement:packetloss 0/0\n");
		
		
	return cad;	

	}


	
}