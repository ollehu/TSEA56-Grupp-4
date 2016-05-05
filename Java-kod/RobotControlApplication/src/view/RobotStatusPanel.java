package view;

import java.awt.BorderLayout;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import model.RobotStatus;
import resources.OtherConstants;

public class RobotStatusPanel extends JPanel implements Observer{

	/**
	 * Robot status labels
	 */
	private ArrayList<RobotStatusLabel> robotStatusLabels;
	
	/**
	 * Construcor
	 */
	public RobotStatusPanel() {
		robotStatusLabels = new ArrayList<>();
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		int index = 0;
		for(String labelName : OtherConstants.ROBOT_STATUS_NAMES) {
			String[] states = OtherConstants.ROBOT_STATUS_STATES[index];
			RobotStatusLabel nextLabel = new RobotStatusLabel(labelName, states);
			
			robotStatusLabels.add(nextLabel);
			add(nextLabel);
			
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

	
}
