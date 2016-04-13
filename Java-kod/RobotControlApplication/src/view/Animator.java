package view;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.GridBagLayout;
import java.awt.GridLayout;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JPanel;

import control.*;

public class Animator {
	
	private Handler handler;
	
	private JFrame frame;
	
	private JPanel topPanel;
	private JPanel bottomPanel;
	
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
		frame.setLayout(new BorderLayout());
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// create and add layout panels
		topPanel = new JPanel();
		bottomPanel = new JPanel();
		
		frame.add(topPanel, BorderLayout.NORTH);
		frame.add(bottomPanel, BorderLayout.SOUTH);
		
		// create and add content panels
		robotControlPanel = new RobotControlPanel(handler, this);
		mapPanel = new MapPanel();
		graphPanel = new GraphPanel();
		tablePanel = new TablePanel();
		
		topPanel.add(mapPanel);
		topPanel.add(tablePanel);
		bottomPanel.add(graphPanel);
		bottomPanel.add(robotControlPanel);
	}
	
	public void showFrame() {
		frame.pack();
		frame.setVisible(true);
	}

	public RobotControlPanel getRobotControlPanel() {
		return robotControlPanel;
	}

	public JFrame getFrame() {
		return frame;
	}
	
	
	
	
}
