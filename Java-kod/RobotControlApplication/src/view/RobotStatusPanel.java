package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import control.ActionHandler;
import model.RobotStatus;
import resources.OtherConstants;

public class RobotStatusPanel extends JPanel implements Observer{

	private ActionHandler actionHandler;
	
	/**
	 * Button used to start run 
	 */
	private JButton startRunButton;
	
	/**
	 * Button used to trigger next decision
	 */
	private JButton nextDecisionButton;
	
	/**
	 * Items used to select heat
	 */
	private JPanel currentHeatPanel;
	private JLabel currentHeatLabel;
	private Integer[] availableHeats = {1, 2};
	private JComboBox<Integer> currentHeatComboBox;
	
	/**
	 * Robot status labels
	 */
	private ArrayList<RobotStatusLabel> robotStatusLabels;
	
	/**
	 * Construcor
	 */
	public RobotStatusPanel(ActionHandler actionHandler) {
		this.actionHandler = actionHandler;
		
		robotStatusLabels = new ArrayList<>();
		setLayout(new GridBagLayout());
		
		// constraints
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		constraints.anchor = GridBagConstraints.FIRST_LINE_START;
		
		// add start run button
		startRunButton = new JButton(actionHandler.startRunAction);
		add(startRunButton, constraints);
		
		// add next decision button
		constraints.gridy++;
		nextDecisionButton = new JButton(actionHandler.nextDecisionAction);
		add(nextDecisionButton, constraints);
		
		// add current heat panel
		constraints.gridy++;
		currentHeatPanel = new JPanel();
		currentHeatLabel = new JLabel("Current heat: ");
		currentHeatPanel.add(currentHeatLabel);
		currentHeatComboBox = new JComboBox<>(availableHeats);
		currentHeatComboBox.addActionListener(actionHandler.comboBoxListener);
		currentHeatPanel.add(currentHeatComboBox);
//		add(currentHeatPanel, constraints);
		
		// add robot statuses
		int index = 0;
		constraints.gridy++;
		for(String labelName : OtherConstants.ROBOT_STATUS_NAMES) {
			String[] states = OtherConstants.ROBOT_STATUS_STATES[index];
			RobotStatusLabel nextLabel = new RobotStatusLabel(labelName, states);
			
			robotStatusLabels.add(nextLabel);
			add(nextLabel,constraints);
			nextLabel.setVisible(OtherConstants.ROBOT_STATUS_VISIBILITY[index]);
			
			constraints.gridy++;
			index++;
		}
	}

	/**
	 * Robot status observer
	 */
	@Override
	public void update(Observable o, Object arg) {
		// check if update is in robotData
		if(o instanceof model.RobotData) {
			if(arg instanceof RobotStatus) {
				int index = ((RobotStatus) arg).getIndex();
				int value = ((RobotStatus) arg).getValue();
				
				robotStatusLabels.get(index).setValue(value);
			}
		}		
	}

	/**
	 * Sets autonomous mode
	 */
	public void setAutonomousMode(boolean state) {
		startRunButton.setVisible(state);
		currentHeatPanel.setVisible(state);
	}
	
	/**
	 * Sets debug mode
	 */
	public void setDebugMode(boolean state) {
		nextDecisionButton.setVisible(state);
	}
}
