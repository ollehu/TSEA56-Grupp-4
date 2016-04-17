package view;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowListener;
import java.util.regex.Pattern;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.basic.BasicArrowButton;

import control.Handler;
import jssc.SerialPortList;
import serialCOM.UnknownOperatingSystemException;

public class RobotControlPanel extends JPanel 
implements 	ChangeListener {

	private Handler handler;
	private Animator animator;

	private JPanel buttonPanel;
	private JPanel statusPanel;

	private BasicArrowButton upArrowKeyButton;
	private BasicArrowButton downArrowKeyButton;
	private BasicArrowButton leftArrowKeyButton;
	private BasicArrowButton rightArrowKeyButton;

	private JButton selectCOMPortButton;

	private JButton clawButton;
	private JButton controlButton;

	private JLabel controlStatusLabel;
	private JLabel clawStatusLabel;

	private boolean isControlOn = false;
	private boolean isClawOpen = true;

	private Color pressedBackgroundColor = new Color(40, 40, 40);
	private Color trueColor = new Color(40, 200, 40);
	private Color falseColor = new Color(200, 40, 40);

	private JSlider speedSlider;

	static final int SPEED_MIN = 0;
	static final int SPEED_MAX = 100;
	static final int SPEED_INIT = 50;

	private int speed = 50;

	public RobotControlPanel(Handler handler, Animator animator) {
		this.handler = handler;
		this.animator = animator;

		setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.anchor = GridBagConstraints.PAGE_START;
		constraints.fill = GridBagConstraints.HORIZONTAL;

		// add selectCOMPortButton
		selectCOMPortButton = new JButton("Select COM port");
		selectCOMPortButton.addActionListener(new SelectCOMPortListener());
		add(selectCOMPortButton, constraints);

		// add labels 
		statusPanel = new JPanel(new BorderLayout());

		controlStatusLabel = new JLabel("Control: off");
		controlStatusLabel.setForeground(falseColor);

		clawStatusLabel = new JLabel("Claw: open");
		clawStatusLabel.setForeground(trueColor);

		statusPanel.add(controlStatusLabel, BorderLayout.WEST);
		statusPanel.add(clawStatusLabel, BorderLayout.EAST);

		constraints.anchor = GridBagConstraints.CENTER;
		constraints.gridy = 1;
		add(statusPanel, constraints);

		// create and add buttons
		buttonPanel = new JPanel(new GridLayout(2, 3));

		controlButton = new JButton("Control");
		controlButton.addActionListener(new ControlListener());

		upArrowKeyButton = new BasicArrowButton(BasicArrowButton.NORTH);
		downArrowKeyButton = new BasicArrowButton(BasicArrowButton.SOUTH);
		leftArrowKeyButton = new BasicArrowButton(BasicArrowButton.WEST);
		rightArrowKeyButton = new BasicArrowButton(BasicArrowButton.EAST);

		clawButton = new JButton("Claw");
		clawButton.addActionListener(new ClawListener());

		buttonPanel.add(controlButton);
		buttonPanel.add(upArrowKeyButton);
		buttonPanel.add(clawButton);
		buttonPanel.add(leftArrowKeyButton);
		buttonPanel.add(downArrowKeyButton);
		buttonPanel.add(rightArrowKeyButton);

		constraints.gridy = 2;
		add(buttonPanel, constraints);

		// initialize and add slider
		speedSlider = new JSlider(JSlider.HORIZONTAL, SPEED_MIN, SPEED_MAX, SPEED_INIT);
		speedSlider.addChangeListener(this);

		speedSlider.setMajorTickSpacing(20);
		speedSlider.setMinorTickSpacing(5);
		speedSlider.setPaintTicks(true);
		speedSlider.setPaintLabels(true);
		speedSlider.setFocusable(false);

		constraints.anchor = GridBagConstraints.PAGE_END;
		constraints.gridy = 3;
		add(speedSlider, constraints);
	}


	public void updateKeys(boolean[] keysCurrentlyPressed) {
		upArrowKeyButton.setBackground(null);
		downArrowKeyButton.setBackground(null);
		leftArrowKeyButton.setBackground(null);
		rightArrowKeyButton.setBackground(null);

		if(keysCurrentlyPressed[KeyEvent.VK_UP]) {
			upArrowKeyButton.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_DOWN]) {
			downArrowKeyButton.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
			leftArrowKeyButton.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_RIGHT]) {
			rightArrowKeyButton.setBackground(pressedBackgroundColor);
		}

	}


	@Override
	public void stateChanged(ChangeEvent e) {
		JSlider source = (JSlider)e.getSource();

		if (!source.getValueIsAdjusting()) {
			speed = (int)source.getValue();
		}
	}


	public int getSpeed() {
		return speed;
	}

	public boolean isControlOn() {
		return isControlOn;
	}


	public boolean isClawOpen() {
		return isClawOpen;
	}

	public void setControlOn(boolean isControlOn) {
		this.isControlOn = isControlOn;
	}


	public void setClawOpen(boolean isClawOpen) {
		this.isClawOpen = isClawOpen;
	}

	private class ControlListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			// toggle isControlOn
			isControlOn = !isControlOn;

			// send command to robot
			handler.setControlOn(isControlOn);

			// change label and send control command
			if(isControlOn) {
				controlStatusLabel.setText("Control: on");
				controlStatusLabel.setForeground(trueColor);
			} else {
				controlStatusLabel.setText("Control: off");
				controlStatusLabel.setForeground(falseColor);
			}
		}

	}

	private class ClawListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			// toggle isClawOpen
			isClawOpen = !isClawOpen;

			// send command to robot
			handler.setClawOpen(isClawOpen);

			// change label
			if(isClawOpen) {
				clawStatusLabel.setText("Claw: open");
				clawStatusLabel.setForeground(trueColor);
			} else {
				clawStatusLabel.setText("Claw: closed");
				clawStatusLabel.setForeground(falseColor);
			}
		}

	}

	private class SelectCOMPortListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			
			
			String osName = System.getProperty("os.name");
			String[] portNames = {"port 1", "port 2", "port 3"};

			// get port names
			if(osName.contains("Windows")){
				portNames = SerialPortList.getPortNames();
			} else if(osName.contains("Mac")) {
				portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("tty."));
			} else {
				portNames = SerialPortList.getPortNames("/dev/", Pattern.compile("(ttyS|ttyUSB|ttyACM|ttyAMA|rfcomm)[0-9]{1,3}"));
			}

			if(portNames.length == 0) {
				JOptionPane.showMessageDialog(animator.getFrame(),
					    "No ports available!",
					    "Port error",
					    JOptionPane.ERROR_MESSAGE);
			} else {
				String selectedPort = (String)JOptionPane.showInputDialog(animator.getFrame(),
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
}
