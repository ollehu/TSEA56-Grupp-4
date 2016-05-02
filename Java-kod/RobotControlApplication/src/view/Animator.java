package view;

import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.security.KeyStore.PrivateKeyEntry;
import java.util.regex.Pattern;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import control.*;
import jssc.SerialPortException;
import jssc.SerialPortList;
import serialCOM.ControlSettingID;
import serialCOM.DataID;

/**
 * Handles the GUI
 * @author isak
 *
 */
public class Animator {
	
	/**
	 * 
	 */
	private Handler handler;
	
	/**
	 * 
	 */
	private JFrame frame;
	
	/**
	 * 
	 */
	private JMenuBar menuBar;
	
	/**
	 * File menu with its items
	 */
	private JMenu fileMenu;
	private JMenuItem saveLogAction;
	private JMenuItem commentLogAction;
	private JMenuItem selectCOMPortAction;
	
	/**
	 * Edit menu with its items
	 */
	private JMenu editMenu;
	private JCheckBoxMenuItem debugModeAction;
	private JMenuItem clearMapAction;
	
	/**
	 * Panel containing robot controls
	 */
	private RobotControlPanel robotControlPanel;
	
	/**
	 * Panel containing map
	 */
	private MapPanel mapPanel;
	
	/**
	 * Panel containing sensor value graphs
	 */
	private GraphPanel graphPanel;
	
	/**
	 * Panel containing sensor values
	 */
	private TablePanel tablePanel;
	
	public void updateKeys(boolean[] keysCurrentlyPressed) {
		robotControlPanel.updateKeys(keysCurrentlyPressed);
	}
	
	public Animator(Handler handler) {
		this.handler = handler;
		
		// create frame
		frame = new JFrame("Robot remote control");
		frame.setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		frame.addWindowListener(new WindowAdapter()
		{
		    public void windowClosing(WindowEvent e)
		    {
		        exitProgram();
		    }
		});
		
		// create and add menu bar
		menuBar = new JMenuBar();
		frame.setJMenuBar(menuBar);
		
		// create file menu and items
		fileMenu = new JMenu("File");
		menuBar.add(fileMenu);

		saveLogAction = new JMenuItem("Save log");
		saveLogAction.addActionListener(new SaveLogListener());
		
		commentLogAction = new JMenuItem("Comment log");
		commentLogAction.addActionListener(new CommentLogListener());
		
		selectCOMPortAction = new JMenuItem("Select COM Port");
		selectCOMPortAction.addActionListener(new SelectCOMPortListener());
		
		fileMenu.add(saveLogAction);
		fileMenu.add(commentLogAction);
		fileMenu.addSeparator();
		fileMenu.add(selectCOMPortAction);
		
		// create edit menu and items
		editMenu = new JMenu("Edit");
		menuBar.add(editMenu);

		debugModeAction = new JCheckBoxMenuItem("Debug mode on");
		debugModeAction.addActionListener(new DebugModeListener());
		
		clearMapAction = new JMenuItem("Clear map");
		clearMapAction.addActionListener(new ClearMapListener());
		
		editMenu.add(debugModeAction);
		editMenu.addSeparator();
		editMenu.add(clearMapAction);
		
		// create and add content panels
		robotControlPanel = new RobotControlPanel(handler, this);
		mapPanel = new MapPanel();
		graphPanel = new GraphPanel(this);
		tablePanel = new TablePanel(this);
		
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		constraints.anchor = GridBagConstraints.FIRST_LINE_START;
		frame.add(mapPanel, constraints);
		
		constraints.gridx = 1;
		constraints.fill = GridBagConstraints.VERTICAL;
		constraints.anchor = GridBagConstraints.FIRST_LINE_END;
		frame.add(tablePanel, constraints);
		tablePanel.setAutonomousMode(false);
		
		constraints.gridx = 0;
		constraints.gridy = 1;
		constraints.fill = GridBagConstraints.HORIZONTAL;
		constraints.anchor = GridBagConstraints.LAST_LINE_START;
		frame.add(graphPanel, constraints);
		
		constraints.gridx = 1;
		constraints.fill = GridBagConstraints.NONE;
		constraints.anchor = GridBagConstraints.LAST_LINE_END;
		frame.add(robotControlPanel, constraints);
		
		setDebugMode(false);
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}
	
	private void exitProgram() {
		// write comment to log file
		String comment = JOptionPane.showInputDialog("Write a log comment");
		
		// if cancel is NOT pressed
		if(comment != null) {
			// close serial port
			handler.closeSerialPort();
			
			// delete or close log
			handler.getLogWriter().exit(comment);
			
			System.exit(0);
		} 	
	}

	public RobotControlPanel getRobotControlPanel() {
		return robotControlPanel;
	}

	public JFrame getFrame() {
		return frame;
	}
	
	public void setAutonomousMode(boolean isAutonomousModeOn) {
		tablePanel.setAutonomousMode(isAutonomousModeOn);
	}

	public Handler getHandler() {
		return handler;
	}

	public GraphPanel getGraphPanel() {
		return graphPanel;
	}

	public MapPanel getMapPanel() {
		return mapPanel;
	}

	public TablePanel getTablePanel() {
		return tablePanel;
	}
	
	private class SaveLogListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			handler.getLogWriter().closeLog(LogWriter.SAVE);
		}

	}

	private class CommentLogListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			String comment = JOptionPane.showInputDialog("Write a log comment");

			handler.getLogWriter().appendToLog(comment);
		}

	}
	
	private class SelectCOMPortListener implements ActionListener {

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
				JOptionPane.showMessageDialog(frame,
						"No ports available!",
						"Port error",
						JOptionPane.ERROR_MESSAGE);
			} else {
				selectedPort = (String)JOptionPane.showInputDialog(frame,
						"Select COM port", 
						"Select COM port",
						JOptionPane.PLAIN_MESSAGE,
						null,
						portNames,
						portNames[0]);
				
				if(selectedPort != null) {
					handler.connectToSerialPort(selectedPort);
				}
			}
		}
	}
	

	private class ClearMapListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			mapPanel.getMap().clear();
		}
		
	}
	
	private class DebugModeListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			// send debug mode to robot
			int value = 0;
			if(debugModeAction.isSelected()) {
				value = 1;
			}
			
			try {
				handler.getSerialPortCOM().sendToRobot(DataID.CONTROL_SETTING, ControlSettingID.DEBUG_MODE, value);
			} catch (SerialPortException e1) {
				e1.printStackTrace();
			}
			
			// adjust window
			setDebugMode(debugModeAction.isSelected());
		}
	}
	
	public void setDebugMode(boolean state) {
		saveLogAction.setVisible(state);
		commentLogAction.setVisible(state);
		
		tablePanel.setDebugMode(debugModeAction.isSelected());
		robotControlPanel.setDebugMode(debugModeAction.isSelected());
	}
}
