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
	 * Button used to start run or trigger next decision
	 */
	private JButton startRunNextMoveButton;
	
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
		
		// add start run/next decision button
		startRunNextMoveButton = new JButton(actionHandler.startRunAction);
		add(startRunNextMoveButton, constraints);
		
		// add current heat panel
		constraints.gridy++;
		currentHeatPanel = new JPanel();
		currentHeatLabel = new JLabel("Current heat: ");
		currentHeatPanel.add(currentHeatLabel);
		currentHeatComboBox = new JComboBox<>(availableHeats);
		currentHeatComboBox.addActionListener(actionHandler.comboBoxListener);
		currentHeatPanel.add(currentHeatComboBox);
		add(currentHeatPanel, constraints);
		
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
		startRunNextMoveButton.setVisible(state);
		currentHeatPanel.setVisible(state);
	}
	
	/**
	 * Sets debug mode
	 */
	public void setDebugMode(boolean state) {
		if(state) {
			startRunNextMoveButton.setAction(actionHandler.nextDecisionAction);
		} else {
			startRunNextMoveButton.setAction(actionHandler.startRunAction);
		}
	}
}
