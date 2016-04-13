package view;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;

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
//		frame.setLayout(new BorderLayout());
		frame.setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// create and add layout panels
		topPanel = new JPanel();
		bottomPanel = new JPanel();
		
//		frame.add(topPanel, BorderLayout.NORTH);
//		frame.add(bottomPanel, BorderLayout.SOUTH);
		
		// create and add content panels
		robotControlPanel = new RobotControlPanel(handler, this);
		mapPanel = new MapPanel();
		graphPanel = new GraphPanel();
		tablePanel = new TablePanel();
		
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		frame.add(mapPanel, constraints);
		
		constraints.gridx = 1;
		frame.add(tablePanel, constraints);
		
		constraints.gridx = 0;
		constraints.gridy = 1;
		frame.add(graphPanel, constraints);
		
		constraints.gridx = 1;
		frame.add(robotControlPanel, constraints);
		
//		topPanel.add(mapPanel);
//		topPanel.add(tablePanel);
//		bottomPanel.add(graphPanel);
//		bottomPanel.add(robotControlPanel);
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
