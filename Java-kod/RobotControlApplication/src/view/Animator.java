package view;

import java.awt.Frame;
import java.awt.GridLayout;

import javax.swing.JFrame;

import control.*;

public class Animator {
	
	private Handler handler;
	
	private JFrame frame;
	
	private RobotControlPanel robotControlPanel;
	private MapPanel mapPanel;
	private GraphPanel graphPanel;
	private TablePanel tablePanel;
	
	public void updateKeys(boolean[] keysCurrentlyPressed) {
		robotControlPanel.updateKeys(keysCurrentlyPressed);
	}
	
	public Animator(Handler handler) {
		this.handler = handler;
		
		frame = new JFrame("Robot remote control");
		frame.setLayout(new GridLayout(2, 2));
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// create and add panels
		robotControlPanel = new RobotControlPanel(handler);
		mapPanel = new MapPanel();
		graphPanel = new GraphPanel();
		tablePanel = new TablePanel();
		
		frame.add(robotControlPanel);
		frame.add(mapPanel);
		frame.add(graphPanel);
		frame.add(tablePanel);
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}

	public RobotControlPanel getRobotControlPanel() {
		return robotControlPanel;
	}
	
	
}
