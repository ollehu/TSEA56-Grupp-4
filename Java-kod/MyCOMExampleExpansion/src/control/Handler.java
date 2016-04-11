package control;

import serialCOM.SerialPortCOM;
import view.*;

public class Handler {

	private Animator animator;
	
	private SerialPortCOM serialPortCOM;
	
	private int speed;
	
	public int getUserPortChoice(String[] portNames) {
		// TODO get user input
		
		return 1;
	}

	public int getSpeed() {
		return speed;
	}
	
	
	
}
