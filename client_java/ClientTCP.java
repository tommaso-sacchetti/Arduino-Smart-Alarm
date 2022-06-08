package main;

import java.net.*;
import java.io.*;

public class ClientTCP {
  private Socket socket = null;
  private PrintWriter os = null;
  private BufferedReader is = null;
  private String host;
  private int port;
  private String output;
  private boolean connected  = false;
  
  public ClientTCP(String host, int port) {
    this.host= host; this.port =port;
  }

  public String send_and_await(String input)
  {
	output = null;
    connect(); //Se conecta con el servidor
    if (socket != null && os != null && is != null) {
  	try {
             os.println(input); //Se encola el mensaje para el servidor
             os.flush(); //Se fuerza el envio de los mensajes encolados
             output= is.readLine(); //Se espera y se lee la respuesta del servidor
      } catch (IOException e) {
        System.err.println("I/O failed in reading/writing socket");
      }
    }
    disconnect(); //Se desconecta del servidor
    return output;
  }

  private synchronized void connect()
  {
	  if(!connected){
		  try {
				socket = new Socket(host, port); //Se crea el socket y se realiza la conexion con el servidor
				is = new BufferedReader(new InputStreamReader(socket.getInputStream())); //Se vincula el stream de entrada del socket con un BufferReader
				os = new PrintWriter(socket.getOutputStream()); //Se vincula el stream de salida del socket con un PrintWriter
				connected=true;
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} 
	  }	
  }

  private synchronized void disconnect(){ 
	  if(connected){
		  try {
				socket.close();
				is.close();
				os.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		  connected=false;
	  }
  }
}
