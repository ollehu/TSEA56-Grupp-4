package view;

import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JPanel;

import control.*;

/**
 * Handles the GUI
 * @author isak
 *
 */
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
		frame.setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// create and add layout panels
		topPanel = new JPanel();
		bottomPanel = new JPanel();
		
		// create and add content panels
		robotControlPanel = new RobotControlPanel(handler, this);
		mapPanel = new MapPanel();
		graphPanel = new GraphPanel();
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
		
		// close serial port on window close
		frame.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				handler.closeSerialPort();
			}
		});
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
	
	public void setAutonomousMode(boolean isAutonomousModeOn) {
		tablePanel.setAutonomousMode(isAutonomousModeOn);
	}
}
