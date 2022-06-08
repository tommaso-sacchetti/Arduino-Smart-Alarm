package main;

import java.text.SimpleDateFormat;
import java.util.Date;

public class Syncro extends Thread {
	ClientTCP cl;
	
	public Syncro(ClientTCP cl) {
		this.cl = cl;
	}
	
	public void run() {
		int i = 60;
		String input;
		String output;
		String time;
		while(true) {
			
			if(i == 60 ) {
				Date date = new Date(System.currentTimeMillis());
				
				SimpleDateFormat formatter = new SimpleDateFormat("HH:mm:ss");
				time = formatter.format(date);
				i = 0;
				
				input = "SET_TIME=" + time;
				input += '\r';
				output = cl.send_and_await(input);
				System.out.println("output: " + output);
			}
			
			try        
			{
			    Thread.sleep(1000);
			} 
			catch(InterruptedException ex) 
			{
			    Thread.currentThread().interrupt();
			}
			i++;
		}
	}

}
