package control;

import java.awt.Color;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.regex.Pattern;

import javax.swing.*;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

import jssc.SerialPortException;
import jssc.SerialPortList;
import model.Log;
import model.RobotData;
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
	
	/**
	 * Internal variables
	 */
	protected boolean[] directionKeysPressed = new boolean[11];

	/**
	 * Menu actions
	 */
	public DisplayKeybindingsAction displayKeybindingsAction = new DisplayKeybindingsAction();

	public SaveLogAction saveLogAction = new SaveLogAction();
	public CommentLogAction commentLogAction = new CommentLogAction();

	public SelectCOMPortAction selectCOMPortAction = new SelectCOMPortAction();
	public ConnectToCOMPortAction connectToCOMPortAction = new ConnectToCOMPortAction();

	public DebugModeAction debugModeAction = new DebugModeAction();
	public ClearMapAction clearMapAction = new ClearMapAction();

	/** 
	 * Robot control actions
	 */
	public SendControlCommandAction pressedLeftAction = new SendControlCommandAction(true, ControlID.ROTATE_LEFT);
	public SendControlCommandAction pressedRightAction = new SendControlCommandAction(true, ControlID.ROTATE_RIGHT);
	public SendControlCommandAction pressedForwardsAction = new SendControlCommandAction(true, ControlID.FORWARDS);
	public SendControlCommandAction pressedBackwardsAction = new SendControlCommandAction(true, ControlID.BACKWARDS);

	public SendControlCommandAction releasedLeftAction = new SendControlCommandAction(false, ControlID.ROTATE_LEFT);
	public SendControlCommandAction releasedRightAction = new SendControlCommandAction(false, ControlID.ROTATE_RIGHT);
	public SendControlCommandAction releasedForwardsAction = new SendControlCommandAction(false, ControlID.FORWARDS);
	public SendControlCommandAction releasedBackwardsAction = new SendControlCommandAction(false, ControlID.BACKWARDS);
	
	public ChangeRobotSpeedAction increaseSpeedAction = new ChangeRobotSpeedAction(true);
	public ChangeRobotSpeedAction decreaseSpeedAction = new ChangeRobotSpeedAction(false);
<<<<<<< HEAD
	
		public SendControlCommandAction forwardsRightAction = new SendControlCommandAction(ControlID.FORWARDS_RIGHT);
		public SendControlCommandAction forwardsLeftAction = new SendControlCommandAction(ControlID.FORWARDS_LEFT);
		public SendControlCommandAction backwardsLeftAction = new SendControlCommandAction(ControlID.BACKWARDS_LEFT);
		public SendControlCommandAction backwardsRightAction = new SendControlCommandAction(ControlID.BACKWARDS_RIGHT);
=======
>>>>>>> 6a72900b8225fb95532893a469a26231917adf10

	public SendControlSettingAction clawAction = new SendControlSettingAction("","",ControlSettingID.CLAW);
	
	public SendControlSettingAction startRunAction = new SendControlSettingAction("Start run", "Starts an autonomous run",
																					ControlSettingID.START_RUN);
	public SendControlSettingAction nextDecisionAction = new SendControlSettingAction("Next decision", "Commands robot to take next autonomous decision",
																						ControlSettingID.NEXT_DECISION);
	
	/**
	 * Listeners
	 */
	public ControlTableListener controlTableListener = new ControlTableListener();
	
	public ComboBoxListener comboBoxListener = new ComboBoxListener();
	
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
	 * Displays all keybindings used by the program	
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
			// create JDialog
			JDialog keybindingsDialog = new JDialog(animator.getFrame(), "Keybindings", Dialog.DEFAULT_MODALITY_TYPE);

			JTextArea textArea = new JTextArea();
			textArea.setAutoscrolls(true);
			textArea.setPreferredSize(new Dimension(300, 300));
			textArea.setBorder(BorderFactory.createLineBorder(Color.BLACK));
			textArea.setFont(new Font("courier new", Font.PLAIN, 12));
			textArea.setLineWrap(true);

			// add text to text area
			for(String row : OtherConstants.KEYBINDINGS_INFO_TEXT) {
				textArea.append(row + "\n");
			}
			
			JScrollPane textAreaScroll = new JScrollPane();
			textAreaScroll.setViewportView(textArea);
			textAreaScroll.setAutoscrolls(true);
			
			keybindingsDialog.add(textAreaScroll);
			keybindingsDialog.pack();
			keybindingsDialog.setVisible(true);
		}

	}

	/**
	 * Saves the active log and creates a new one
	 * @author isak
	 *
	 */
	private class SaveLogAction extends AbstractAction {

		public SaveLogAction() {
			super("Save log", UIManager.getIcon("FileView.floppyDriveIcon"));

			putValue(SHORT_DESCRIPTION, "Closes and saves the log after asking for a final comment");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			log.closeLog(true);
		}

	}

	/**
	 * Appends a comment to the current log
	 * @author isak
	 *
	 */
	private class CommentLogAction extends AbstractAction {

		public CommentLogAction() {
			super("Comment log");

			putValue(SHORT_DESCRIPTION, "Appends a comment to the log");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			String textToAppend = JOptionPane.showInputDialog("Write a log comment");
			
			log.appendToLog(textToAppend);
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
					serialCOM.setSelectedPortName(selectedPort);
				}
			}
		}
	}
	
	/**
	 * 
	 * @author Isak
	 *
	 */
	private class ConnectToCOMPortAction extends AbstractAction {

		public ConnectToCOMPortAction() {
			super("Connect to selected port");

			putValue(SHORT_DESCRIPTION, "Connects to the selected port");		
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			try {
				serialCOM.connectToSerialPort();

				JOptionPane.showMessageDialog(animator.getFrame(),
						"Port connected",
						serialCOM.getSelectedPortName(),
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

	/**
	 * Activates debug mode, both internally and on the robot through a sent command
	 * @author isak
	 *
	 */
	private class DebugModeAction extends AbstractAction {

		public DebugModeAction() {
			super("Debug mode");

			putValue(SHORT_DESCRIPTION, "Toggle robot debug mode");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			robotData.toggle(ControlSettingID.DEBUG_MODE);
			serialCOM.sendToRobot(CommunicationID.CONTROL_SETTING, ControlSettingID.DEBUG_MODE, robotData.getDebugMode());
		}

	}

	/**
	 * Clears the internal map. Does NOT affect the robots map!
	 * @author isak
	 *
	 */
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
	/**
	 * Sends a predefined control command to the robot
	 * @author isak
	 *
	 */
	private class SendControlCommandAction extends AbstractAction {

		private boolean pressed;
		
		private int controlCommand;

		public SendControlCommandAction(boolean pressed, int controlCommand) {
			super();
			
			this.controlCommand = controlCommand;
			this.pressed = pressed;
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			// send command to robot
			directionKeysPressed[controlCommand] = pressed;
			respondToKeyEvent();
		}

	}
	
	protected void respondToKeyEvent() {
		int controlCommandToSend = ControlID.STOP;
		
		if(directionKeysPressed[ControlID.FORWARDS]) {
			if(directionKeysPressed[ControlID.ROTATE_LEFT]) {
				controlCommandToSend = ControlID.FORWARDS_LEFT;
			} else if(directionKeysPressed[ControlID.ROTATE_RIGHT]) {
				controlCommandToSend = ControlID.FORWARDS_RIGHT;
			} else {
				controlCommandToSend = ControlID.FORWARDS;
			}
		} else if(directionKeysPressed[ControlID.BACKWARDS]) {
			if(directionKeysPressed[ControlID.ROTATE_LEFT]) {
				controlCommandToSend = ControlID.BACKWARDS_LEFT;
			} else if(directionKeysPressed[ControlID.ROTATE_RIGHT]) {
				controlCommandToSend = ControlID.BACKWARDS_RIGHT;
			} else {
				controlCommandToSend = ControlID.BACKWARDS;
			}
		} else if(directionKeysPressed[ControlID.ROTATE_LEFT]) {
			controlCommandToSend = ControlID.ROTATE_LEFT;
		} else if(directionKeysPressed[ControlID.ROTATE_RIGHT]) {
			controlCommandToSend = ControlID.ROTATE_RIGHT;
		}
		
		if(controlCommandToSend != robotData.getLastControlCommand()) {
			serialCOM.sendToRobot(CommunicationID.CONTROL_DATA, controlCommandToSend, robotData.getSpeed());
			robotData.update(ControlSettingID.LAST_CONTROL_COMMAND, controlCommandToSend);
		}
	}

	/**
	 * Sends a predefined control setting to the robot
	 * @author isak
	 *
	 */
	private class SendControlSettingAction extends AbstractAction {

		private int controlSetting;

		public SendControlSettingAction(String name, String description, 
										int controlSetting) {
			super(name);
			putValue(SHORT_DESCRIPTION, description);

			this.controlSetting = controlSetting;
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			if(controlSetting == ControlSettingID.NEXT_DECISION ||
					controlSetting == ControlSettingID.START_RUN) {
				serialCOM.sendToRobot(CommunicationID.CONTROL_SETTING, controlSetting, 1);

			} else {
				robotData.toggle(controlSetting);
				serialCOM.sendToRobot(CommunicationID.CONTROL_SETTING, controlSetting, robotData.getControlSetting(controlSetting));
			}
		}
	}
	
	/**
	 * Increments or decrements the robots speed by +/-10
	 * @author isak
	 *
	 */
	private class ChangeRobotSpeedAction extends AbstractAction {

		private boolean isIncrement;
		
		public ChangeRobotSpeedAction(boolean isIncrement) {
			this.isIncrement = isIncrement;
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			if(isIncrement) {
				if(robotData.getSpeed() < 100) {
					robotData.update(ControlSettingID.SPEED, robotData.getSpeed() + 10);
				}
			} else {
				if(robotData.getSpeed() > 0) {
					robotData.update(ControlSettingID.SPEED, robotData.getSpeed() - 10);
				}
			}
		}
	}
	
	//================================================================================
	// Listeners
	//================================================================================
	/**
	 * Detects change in the table
	 * @author Isak
	 *
	 */
	private class ControlTableListener implements TableModelListener {

		@Override
		public void tableChanged(TableModelEvent e) {
			// get value
			int row = e.getFirstRow();
			int column = e.getColumn();
			TableModel model = (TableModel) e.getSource();
			
			double data = Double.parseDouble((String) model.getValueAt(row, column));
			
			// convert and send value
			int sendData = 0;
			int dataID = 0;
			if(row == 0) {
				sendData = (int) (data * 100);
				dataID = ControlSettingID.PROPORTIONAL;
			} else if (row == 1) {
				sendData = (int) (data * 100);
				dataID = ControlSettingID.DERIVATIVE;
			} else if (row == 2) {
				sendData = (int) (data * 10);
				dataID = ControlSettingID.KONSTANT;
			} else if (row == 3) {
				sendData = (int) (data / 10); 
				dataID = ControlSettingID.CONSTANT_90;
			} else if (row == 4)  {
				sendData = (int) (data / 10); 
				dataID = ControlSettingID.CONSTANT_180;
			} else if (row == 5)  {
				sendData = (int) data; 
				dataID = ControlSettingID.SPEED;
			} else if (row == 6)  {
				sendData = (int) data; 
				dataID = ControlSettingID.ROTATION_SPEED;
			}
			serialCOM.sendToRobot(CommunicationID.CONTROL_SETTING, dataID, sendData);
		}
	}
	
	/**
	 * Detects change in comboboxes
	 */
	private class ComboBoxListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			JComboBox<Integer> changedBox = (JComboBox)e.getSource();
	        Integer selectedInteger = (Integer) changedBox.getSelectedItem();
	        
	        robotData.update(ControlSettingID.CURRENT_HEAT, selectedInteger);
	        serialCOM.sendToRobot(CommunicationID.CONTROL_SETTING, ControlSettingID.CURRENT_HEAT, selectedInteger);
		}
		
	}
}
