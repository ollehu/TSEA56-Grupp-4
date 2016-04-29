package view;

import java.awt.GridBagConstraints; 
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.util.regex.Pattern;

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

/**
 * Panel containing robot controls
 * @author isak
 *
 */
public class RobotControlPanel extends JPanel 
implements 	ChangeListener {

	private Handler handler;
	private Animator animator;

	private JPanel buttonPanel;

	private BasicArrowButton upArrowKeyButton;
	private BasicArrowButton downArrowKeyButton;
	private BasicArrowButton leftArrowKeyButton;
	private BasicArrowButton rightArrowKeyButton;

	private JButton clearMapButton;
	private JButton saveLogButton;
	private JButton commentLogButton;
	private JButton selectCOMPortButton;

	private JLabel autonomousModeLabel;
	private JLabel clawStatusLabel;

	private boolean isClawOpen = true;

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

		// add clearMapButton
		clearMapButton = new JButton("Clear map");
		clearMapButton.addActionListener(new ClearMapListener());
		add(clearMapButton, constraints);
		
		// add saveLogButton
		constraints.gridy++;
		saveLogButton = new JButton("Save log");
		saveLogButton.addActionListener(new SaveLogListener());
		add(saveLogButton, constraints);

		// add commentLogButton
		constraints.gridy++;
		commentLogButton = new JButton("Comment log");
		commentLogButton.addActionListener(new CommentLogListener());
		add(commentLogButton, constraints);

		// add selectCOMPortButton
		constraints.gridy++;
		selectCOMPortButton = new JButton("Select COM port");
		selectCOMPortButton.addActionListener(new SelectCOMPortListener());
		add(selectCOMPortButton, constraints);

		// create labels 
		autonomousModeLabel = new JLabel("Control: off");
		autonomousModeLabel.setForeground(Colors.FALSE_COLOR);

		clawStatusLabel = new JLabel("Claw: open");
		clawStatusLabel.setForeground(Colors.TRUE_COLOR);

		// create buttons
		buttonPanel = new JPanel(new GridLayout(2, 3));

		upArrowKeyButton = new BasicArrowButton(BasicArrowButton.NORTH);
		downArrowKeyButton = new BasicArrowButton(BasicArrowButton.SOUTH);
		leftArrowKeyButton = new BasicArrowButton(BasicArrowButton.WEST);
		rightArrowKeyButton = new BasicArrowButton(BasicArrowButton.EAST);

		// add buttons and labels
		buttonPanel.add(autonomousModeLabel);
		buttonPanel.add(upArrowKeyButton);
		buttonPanel.add(clawStatusLabel);
		buttonPanel.add(leftArrowKeyButton);
		buttonPanel.add(downArrowKeyButton);
		buttonPanel.add(rightArrowKeyButton);

		constraints.gridy++;
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
		constraints.gridy++;
		add(speedSlider, constraints);
	}

	public void updateKeys(boolean[] keysCurrentlyPressed) {
		upArrowKeyButton.setBackground(null);
		downArrowKeyButton.setBackground(null);
		leftArrowKeyButton.setBackground(null);
		rightArrowKeyButton.setBackground(null);

		if(keysCurrentlyPressed[KeyEvent.VK_UP]) {
			upArrowKeyButton.setBackground(Colors.PRESSED_BACKGROUND_COLOR);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_DOWN]) {
			downArrowKeyButton.setBackground(Colors.PRESSED_BACKGROUND_COLOR);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
			leftArrowKeyButton.setBackground(Colors.PRESSED_BACKGROUND_COLOR);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_RIGHT]) {
			rightArrowKeyButton.setBackground(Colors.PRESSED_BACKGROUND_COLOR);
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

	public boolean isClawOpen() {
		return isClawOpen;
	}

	public void setClawOpen(boolean isClawOpen) {
		this.isClawOpen = isClawOpen;
	}

	public void toggleClaw() {
		isClawOpen = !isClawOpen;

		// change label
		if(isClawOpen) {
			clawStatusLabel.setText("Claw: open");
			clawStatusLabel.setForeground(Colors.TRUE_COLOR);
		} else {
			clawStatusLabel.setText("Claw: closed");
			clawStatusLabel.setForeground(Colors.FALSE_COLOR);
		}
	}

	private class ClearMapListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			animator.getMapPanel().clearMap();
		}
		
	}
	
	private class SaveLogListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			String comment = JOptionPane.showInputDialog("Write a log comment");

			animator.getHandler().getLogWriter().closeLog(comment, true);
		}

	}

	private class CommentLogListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			String comment = JOptionPane.showInputDialog("Write a log comment");

			animator.getHandler().getLogWriter().appendToLog(comment);
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
				JOptionPane.showMessageDialog(animator.getFrame(),
						"No ports available!",
						"Port error",
						JOptionPane.ERROR_MESSAGE);
			} else {
				selectedPort = (String)JOptionPane.showInputDialog(animator.getFrame(),
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
