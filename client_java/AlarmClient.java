package main;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;

public class AlarmClient {
	private static ClientTCP cl;
	
	public static void printMenu() {
		System.out.println("\n\n*** WELCOME TO THE ARDUINO ALARM CLOCK CLIENT ***");
		System.out.println("* Send a command to the Arduino...");
		System.out.println("");
		System.out.println("[  SET_TIME=hh:mm:ss 		");
		System.out.println("[   SET_SONG=[0/1]		");
		System.out.println("[  		0|->A-Ah, Take On me");
		System.out.println("[  		1|-> StarWars Theme");
		System.out.println("[  SET_ALARM=disable 		");
		System.out.println("[ SET_ALARM=hh:mm:ss 		");
		System.out.println("'terminate' for termination.");
		System.out.println("*************************************************");
		System.out.println("\n");
		
	}
	
	private static boolean checkInput(String input) {
		if(input.indexOf('=') == -1) {
			System.out.println("Error: command must contain \"_\" ");
			printMenu();
			System.out.println("*** Send new command: ");
			return false;
		}
		String commandName = input.substring(0, input.indexOf('='));
		
		System.out.println("COMMAND: " + commandName);
		
		if(!commandName.equals("SET_TIME") && !commandName.equals("SET_SONG") && !commandName.equals("SET_ALARM")) {
			System.out.println("Error: it must be a valid command ");
			printMenu();
			System.out.println("*** Send new command: ");
			return false;
		}
		if(commandName.equals("SET_TIME")) {
			String time = input.substring(input.indexOf('=') + 1);
			System.out.println("TIME: " + time);
			String hour = time.substring(0, time.indexOf(':'));
			String minute = time.substring(time.indexOf(':') + 1, time.indexOf(':', 2) + 2);
			String seconds = time.substring(time.indexOf(':', 2) + 4);
			
			int intHour = Integer.parseInt(hour);
			int intMinute = Integer.parseInt(minute);
			int intSeconds = Integer.parseInt(seconds);
			
			if(intHour < 0 || intHour > 23 || intMinute < 0 || intMinute > 59 || intSeconds < 0 || intSeconds > 59) {
				System.out.println("Error: wrong hour");
				printMenu();
				return false;
			}
		}
		if(commandName.equals("SET_ALARM") && !input.substring(input.indexOf('=') + 1).equals("disable")) {
			String time = input.substring(input.indexOf('=') + 1);
			System.out.println("TIME: " + time);
			String hour = time.substring(0, time.indexOf(':'));
			String minute = time.substring(time.indexOf(':') + 1, time.indexOf(':', 2) + 2);
			String seconds = time.substring(time.indexOf(':', 2) + 4);
			
			int intHour = Integer.parseInt(hour);
			int intMinute = Integer.parseInt(minute);
			int intSeconds = Integer.parseInt(seconds);
			
			if(intHour < 0 || intHour > 23 || intMinute < 0 || intMinute > 59 || intSeconds < 0 || intSeconds > 59) {
				System.out.println("Error: wrong hour");
				printMenu();
				return false;
			}
		}
		
		if(commandName.equals("SET_SONG")) {
			String value = input.substring(input.indexOf('=') + 1);
			if(Integer.parseInt(value) != 0 && Integer.parseInt(value) != 1) {
				System.out.println("Error: wrong song");
				printMenu();
				return false;
			}
		} 
		
		System.out.println("Command valid");
		
		return true;
	}
	
	public static void main(String[] args) {
		if(args.length < 2) {
			System.out.println("Usage: AlarmClient <server> <server_port>");
			System.exit(1);
		}
		
		String server = args[0];
		int port = Integer.parseInt(args[1]);
		
		cl = new ClientTCP(server, port);
		
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		PrintWriter stdOut = new PrintWriter(System.out);
		String input, output, term;
		
		Syncro syncro = new Syncro(cl);
		syncro.start();
		
		
		try {
			input = "";
			term = "terminate";
			while(!input.equals(term)) {
				printMenu();
				
				boolean inputCorrect = false;
				
				while(!inputCorrect) {
					input = stdIn.readLine();
					inputCorrect = checkInput(input);
				}
				
				
				input += '\r';
				output = cl.send_and_await(input);
				stdOut.println(output);
				stdOut.flush();	
			}
 		} catch(IOException e) {
 			System.err.println("Error when connecting to server: " + server);
 		}
	}
}
