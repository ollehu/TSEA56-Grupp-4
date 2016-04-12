package view;

import java.awt.Frame;

import javax.swing.JFrame;

import control.*;

public class Animator {
	
	private Handler handler;
	
	private JFrame frame;
	
	private RobotControlPanel robotControlPanel;
	
	public void updateKeys(boolean[] keysCurrentlyPressed) {
		robotControlPanel.updateKeys(keysCurrentlyPressed);
	}
	
	public Animator(Handler handler) {
		this.handler = handler;
		
		frame = new JFrame("Robot remote control");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// create and add controlpanel
		robotControlPanel = new RobotControlPanel();
		frame.add(robotControlPanel);
		
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}

	public RobotControlPanel getRobotControlPanel() {
		return robotControlPanel;
	}
	
	
}
