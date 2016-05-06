package view;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.JButton;
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
		
		startRunNextMoveButton = new JButton(actionHandler.startRunAction);
		add(startRunNextMoveButton, constraints);
		
		int index = 0;
		constraints.gridy++;
		for(String labelName : OtherConstants.ROBOT_STATUS_NAMES) {
			String[] states = OtherConstants.ROBOT_STATUS_STATES[index++];
			RobotStatusLabel nextLabel = new RobotStatusLabel(labelName, states);
			
			robotStatusLabels.add(nextLabel);
			add(nextLabel,constraints);
			constraints.gridy++;
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
	 * 
	 */
	public void setAutonomousMode(boolean state) {
		//TODO add mode handling
		startRunNextMoveButton.setVisible(state);
	}
	
	public void setDebugMode(boolean state) {
		//TODO add mode handling
		if(state) {
			startRunNextMoveButton.setAction(actionHandler.nextDecisionAction);
		} else {
			startRunNextMoveButton.setAction(actionHandler.startRunAction);
		}
	}
}
