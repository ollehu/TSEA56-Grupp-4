package control;

import java.awt.event.ActionEvent;
import java.util.regex.Pattern;

import javax.swing.AbstractAction;
import javax.swing.JOptionPane;
import javax.swing.JTextArea;
import javax.swing.UIManager;

import jssc.SerialPortException;
import jssc.SerialPortList;
import model.Log;
import model.RobotData;
import model.RobotStatus;
import resources.*;
import view.*;

public class ActionHandler {

	/**
	 * Classes that are modified
	 */
	private Animator animator;
	private Log log;
	private RobotData robotData;
	private SerialCommunicationHandler serialCOM;
	
	//TODO fundera över implementeringen av denna!
	
	/**
	 * Public actions for use
	 */
	public DisplayKeybindingsAction displayKeybindingsAction = new DisplayKeybindingsAction();
	
	public SaveLogAction saveLogAction = new SaveLogAction();
	public CommentLogAction commentLogAction = new CommentLogAction();
	
	public SelectCOMPortAction selectCOMPortAction = new SelectCOMPortAction();
	
	public DebugModeAction debugModeAction = new DebugModeAction();
	public ClearMapAction clearMapAction = new ClearMapAction();
	
	public SendControlCommandAction rotateLeftAction = new SendControlCommandAction(ControlID.ROTATE_LEFT);
	public SendControlCommandAction rotateRightAction = new SendControlCommandAction(ControlID.ROTATE_RIGHT);
	public SendControlCommandAction forwardsAction = new SendControlCommandAction(ControlID.FORWARDS);
	public SendControlCommandAction backwardsAction = new SendControlCommandAction(ControlID.BACKWARDS);
	
//	public SendControlCommandAction forwardsRightAction = new SendControlCommandAction(ControlID.FORWARDS_RIGHT);
//	public SendControlCommandAction forwardsLeftAction = new SendControlCommandAction(ControlID.FORWARDS_LEFT);
//	public SendControlCommandAction backwardsLeftAction = new SendControlCommandAction(ControlID.BACKWARDS_LEFT);
//	public SendControlCommandAction backwardsRightAction = new SendControlCommandAction(ControlID.BACKWARDS_RIGHT);
	
	public SendControlCommandAction stopAction = new SendControlCommandAction(ControlID.STOP);
	public SendControlCommandAction clawAction = new SendControlCommandAction(ControlID.ROTATE_LEFT);
	
	/**
	 * Initialize action handler
	 */
	public void initializeActionHandler(Animator animator, Log log, RobotData robotData, SerialCommunicationHandler serialCOM) {
		this.animator = animator;
		this.log = log;
		this.robotData = robotData;
		this.serialCOM = serialCOM;
	}
	
	
	//================================================================================
    // Menu actions
    //================================================================================
	/**
	 * action	
	 * @author isak
	 *
	 */
	private class DisplayKeybindingsAction extends AbstractAction {

		private JTextArea textArea;
		
		public DisplayKeybindingsAction() {
			super("Display keybindings");

			putValue(SHORT_DESCRIPTION, "Displays all hotkeys used by the program");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			//TODO display keybindings popup
		}
		
	}
	
	private class SaveLogAction extends AbstractAction {

		public SaveLogAction() {
			super("Save log", UIManager.getIcon("FileView.floppyDriveIcon"));
			
			putValue(SHORT_DESCRIPTION, "Closes and saves the log after asking for a final comment");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO save log
			animator.simulateReceivedData();
		}
		
	}
	
	private class CommentLogAction extends AbstractAction {

		public CommentLogAction() {
			super("Comment log");
			
			putValue(SHORT_DESCRIPTION, "Appends a comment to the log");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO comment log
		}
		
	}
	
	/**
	 * Connects to robot communications port through user input
	 * @author isak
	 *
	 */
	private class SelectCOMPortAction extends AbstractAction {

		public SelectCOMPortAction() {
			super("Select communications port");
			
			putValue(SHORT_DESCRIPTION, "Select a port for robot communication from the available ports");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			String osName = System.getProperty("os.name");
			String[] portNames = {"port 1", "port 2", "port 3"};
			String selectedPort = "";

			// get port names
			if(osName.contains("Windows")){
				portNames = SerialPortList.getPortNames();
			} else if(osName.contains("Mac")) {
				portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("tty."));
			} else {
				portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("(ttyS|ttyUSB|ttyACM|ttyAMA|rfcomm)[0-9]{1,3}"));
			}

			if(portNames.length == 0) {
				// no ports available
				JOptionPane.showMessageDialog(animator.getFrame(),
						"No ports available!",
						"Port error",
						JOptionPane.ERROR_MESSAGE);
			} else {
				// get user port selection
				selectedPort = (String)JOptionPane.showInputDialog(animator.getFrame(),
						"Select COM port", 
						"Select COM port",
						JOptionPane.PLAIN_MESSAGE,
						null,
						portNames,
						portNames[0]);
				
				if(selectedPort != null) {
					// connect to port
					try {
						serialCOM.connectToSerialPort(selectedPort);

						JOptionPane.showMessageDialog(animator.getFrame(),
								"Port connected",
								selectedPort,
								JOptionPane.INFORMATION_MESSAGE);
					} catch (SerialPortException e2) {
						String toolTip;
						
						if(e2.getExceptionType().equals("Port busy")) {
							toolTip = "! Try 'lsof | grep ...' in console \nif the problem persists";
						} else {
							toolTip = "";
						}
						
						JOptionPane.showMessageDialog(animator.getFrame(),
								e2.getExceptionType() + toolTip,
								e2.getPortName(),
								JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		}
	}
	
	private class DebugModeAction extends AbstractAction {

		public DebugModeAction() {
			super("Debug mode");
			
			putValue(SHORT_DESCRIPTION, "Toggle robot debug mode");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			robotData.toggle(ControlSettingID.DEBUG_MODE);
			//TODO add send 
		}
		
	}
	
	private class ClearMapAction extends AbstractAction {

		public ClearMapAction() {
			super("Clear map");
			
			putValue(SHORT_DESCRIPTION, "Clears the map. Does NOT affect the robot's internal map");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			animator.getMapPanel().clear();
		}
		
	}
	
	//================================================================================
    // Robot control actions
    //================================================================================
	private class SendControlCommandAction extends AbstractAction {

		private int controlCommand;
		
		public SendControlCommandAction(int controlCommand) {
			super("Send control command");
			
			this.controlCommand = controlCommand;
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			robotData.update(ControlSettingID.LAST_CONTROL_COMMAND, controlCommand - 1);
		}
		
	}
	
	private class SendControlSettingAction extends AbstractAction {

		private int controlSetting;
		
		public SendControlSettingAction(int controlSetting) {
			super("Send control setting");
			
			this.controlSetting = controlSetting;
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
		}
		
	}
}
