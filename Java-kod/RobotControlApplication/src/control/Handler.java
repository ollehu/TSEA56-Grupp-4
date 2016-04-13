package control;

import java.awt.event.KeyEvent;

import jssc.SerialPortException;
import serialCOM.ControlID;
import serialCOM.SerialPortCOM;
import serialCOM.UnknownOperatingSystemException;
import view.*;

public class Handler {

	private Animator animator;

	private MyKeyListener myKeyListener;

	private SerialPortCOM serialPortCOM;

	private int lastSentControlCommand;

	public Handler() {
		animator = new Animator(this);
		animator.showFrame();

		serialPortCOM = new SerialPortCOM(this);

		myKeyListener = new MyKeyListener(animator, this);
	}
	
	public void connectToSerialPort(String portName) {
		try {
			serialPortCOM.connectToSerialPort(portName);
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void respondsToKeyEvent(boolean[] keysCurrentlyPressed) {
		int controlCommand = ControlID.STOP;

		// get control command
		if(keysCurrentlyPressed[KeyEvent.VK_UP]) {
			if (keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
				controlCommand = ControlID.FORWARDS_LEFT;
			} else if (keysCurrentlyPressed[KeyEvent.VK_RIGHT]){
				controlCommand = ControlID.FORWARDS_RIGHT;
			} else {
				controlCommand = ControlID.FORWARDS;
			}
		} else if (keysCurrentlyPressed[KeyEvent.VK_DOWN]) {
			if (keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
				controlCommand = ControlID.BACKWARDS_LEFT;
			} else if (keysCurrentlyPressed[KeyEvent.VK_RIGHT]){
				controlCommand = ControlID.BACKWARDS_RIGHT;
			} else {
				controlCommand = ControlID.BACKWARDS;
			}
		} else if (keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
			controlCommand = ControlID.ROTATE_LEFT;
		} else if (keysCurrentlyPressed[KeyEvent.VK_RIGHT]) {
			controlCommand = ControlID.ROTATE_RIGHT;
		}

		// send if != last sent command
		try {
			if(controlCommand != lastSentControlCommand) {

				serialPortCOM.sendControlCommand(controlCommand);
				lastSentControlCommand = controlCommand;
			}
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
	}
	
	public void setControlOn(boolean state) {
		try {
			serialPortCOM.setControlOn(state);
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void setClawOpen(boolean state) {
		try {
			serialPortCOM.setClawOpen(state);
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public int getSpeed() {
		return animator.getRobotControlPanel().getSpeed();
	}
}
