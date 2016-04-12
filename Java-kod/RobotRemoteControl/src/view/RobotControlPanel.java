package view;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.plaf.basic.BasicArrowButton;

public class RobotControlPanel extends JPanel 
implements 	ChangeListener {

	private BasicArrowButton upArrowKey;
	private BasicArrowButton downArrowKey;
	private BasicArrowButton leftArrowKey;
	private BasicArrowButton rightArrowKey;
	
	private Color pressedBackgroundColor = new Color(40, 40, 40);

	private JSlider speedSlider;

	static final int SPEED_MIN = 0;
	static final int SPEED_MAX = 100;
	static final int SPEED_INIT = 50;

	private int speed;

	public RobotControlPanel() {
		setLayout(new GridBagLayout());

		// create and add arrow buttons
		upArrowKey = new BasicArrowButton(BasicArrowButton.NORTH);
		downArrowKey = new BasicArrowButton(BasicArrowButton.SOUTH);
		leftArrowKey = new BasicArrowButton(BasicArrowButton.WEST);
		rightArrowKey = new BasicArrowButton(BasicArrowButton.EAST);

		GridBagConstraints constraints = new GridBagConstraints();
		constraints.weightx = 0.5;
		constraints.weighty = 0.5;
		constraints.fill = GridBagConstraints.HORIZONTAL;
		
		constraints.gridx = 1;
		constraints.gridy = 0;
		add(upArrowKey, constraints);
		
		constraints.gridx = 1;
		constraints.gridy = 1;
		add(downArrowKey, constraints);
		
		constraints.gridx = 0;
		add(leftArrowKey, constraints);
		
		constraints.gridx = 2;
		add(rightArrowKey, constraints);

		// initialize and add slider
		speedSlider = new JSlider(JSlider.HORIZONTAL, SPEED_MIN, SPEED_MAX, SPEED_INIT);
		speedSlider.addChangeListener(this);

		speedSlider.setMajorTickSpacing(20);
		speedSlider.setMinorTickSpacing(5);
		speedSlider.setPaintTicks(true);
		speedSlider.setPaintLabels(true);
		speedSlider.setFocusable(false);

		constraints.gridx = 1;
		constraints.gridy = 2;
		add(speedSlider, constraints);
	}


	public void updateKeys(boolean[] keysCurrentlyPressed) {
		upArrowKey.setBackground(null);
		downArrowKey.setBackground(null);
		leftArrowKey.setBackground(null);
		rightArrowKey.setBackground(null);
		
		if(keysCurrentlyPressed[KeyEvent.VK_UP]) {
			upArrowKey.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_DOWN]) {
			downArrowKey.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_LEFT]) {
			leftArrowKey.setBackground(pressedBackgroundColor);
		}
		if(keysCurrentlyPressed[KeyEvent.VK_RIGHT]) {
			rightArrowKey.setBackground(pressedBackgroundColor);
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
	
	

}
