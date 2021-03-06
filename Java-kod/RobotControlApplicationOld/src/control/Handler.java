package control;

import java.awt.event.KeyEvent;

import javax.swing.JOptionPane;

import jssc.SerialPortException;
import serialCOM.ControlID;
import serialCOM.ControlSettingID;
import serialCOM.CommunicationID;
import serialCOM.SerialPortCOM;
import view.*;

/**
 * Handles the animator, the key listener, the serial port and the log writer
 * @author isak
 *
 */
public class Handler {

	/**
	 * Handles the GUI
	 */
	private Animator animator;

	/**
	 * Handles key events
	 */
	private MyKeyListener myKeyListener;

	/**
	 * Handles serial communication with the robot
	 */
	private SerialPortCOM serialPortCOM;
	
	/**
	 * Handles logs
	 */
	private LogWriter logWriter;

	private int lastSentControlCommand;
	private boolean isClawOpen = true;

	public Handler() {
		animator = new Animator(this);
		animator.showFrame();

		serialPortCOM = new SerialPortCOM(this);

		myKeyListener = new MyKeyListener(animator, this);

		setAutomousMode(true);
		
		logWriter = new LogWriter();
		logWriter.createNewLog();
	}

	/**
	 * Connects to serial port
	 * @param portName
	 */
	public void connectToSerialPort(String portName) {
		try {
			serialPortCOM.connectToSerialPort(portName);

			JOptionPane.showMessageDialog(animator.getFrame(),
					"Port connected",
					portName,
					JOptionPane.INFORMATION_MESSAGE);
		} catch (SerialPortException e) {
			String toolTip;
			
			if(e.getExceptionType().equals("Port busy")) {
				toolTip = "! Try 'lsof | grep ...' in console \nif the problem persists";
			} else {
				toolTip = "";
			}
			
			JOptionPane.showMessageDialog(animator.getFrame(),
					e.getExceptionType() + toolTip,
					e.getPortName(),
					JOptionPane.ERROR_MESSAGE);
		}
	}

	public void closeSerialPort() {
		serialPortCOM.closeSerialPort();
	}

	/**
	 * Sends the appropriate control command based on keys currently pressed
	 * 
	 * @param keysCurrentlyPressed the keys currently held down
	 */
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

				serialPortCOM.sendToRobot(CommunicationID.CONTROL_DATA, controlCommand, getSpeed());
				lastSentControlCommand = controlCommand;
			}
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Toggles internal variables related to the robot's claw
	 */
	public void toggleClaw() {
		isClawOpen = !isClawOpen;
		
		try {
			if(isClawOpen) {
				serialPortCOM.sendToRobot(CommunicationID.CONTROL_DATA, ControlID.CLAW, 1);
			} else {
				serialPortCOM.sendToRobot(CommunicationID.CONTROL_DATA, ControlID.CLAW, 0);
			}


		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		animator.getRobotControlPanel().toggleClaw();
	}

	public int getSpeed() {
		return animator.getRobotControlPanel().getSpeed();
	}

	public void setAutomousMode(boolean isAutonomousModeOn) {
		myKeyListener.setAutonomousMode(isAutonomousModeOn);

		animator.setAutonomousMode(isAutonomousModeOn);
	}

	public SerialPortCOM getSerialPortCOM() {
		return serialPortCOM;
	}

	public Animator getAnimator() {
		return animator;
	}

	public LogWriter getLogWriter() {
		return logWriter;
	}

	public void setDebugMode(boolean state) {
		
	}
}
