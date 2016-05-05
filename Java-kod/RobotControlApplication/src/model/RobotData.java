package model;

import java.util.ArrayList;
import java.util.Observable;

import resources.ControlID;
import resources.ControlSettingID;
import resources.Converters;
import resources.OtherConstants;

public class RobotData extends Observable{
	
	private ArrayList<RobotStatus> robotStatus;
	
	/**
	 * Constructor
	 */
	public RobotData() {
		robotStatus = new ArrayList<>();
		
		int index = 0;
		for(String statusName : OtherConstants.ROBOT_STATUS_NAMES) {
			robotStatus.add(new RobotStatus(index));
			index++;
		}
	}
	
	/**
	 * Update a robot status and notify
	 * @param identifier
	 * @param value
	 */
	public void update(int identifier, int value) {
		RobotStatus statusToUpdate = robotStatus.get(convertIdentifierToIndex(identifier));
		
		statusToUpdate.setValue(value);
		setChanged();
		notifyObservers(statusToUpdate);
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	private int convertIdentifierToIndex(int identifier) {
		if(identifier == ControlSettingID.AUTONOMOUS_MODE) {
			return 0;
		
		} else if (identifier == ControlSettingID.DEBUG_MODE) {
			return 1;		
		
		} else if (identifier == ControlID.CLAW) {
			return 2;		
		
		} else if (identifier == ControlSettingID.SPEED) {
			return 3;		
		
		} else {
			return -1;
		}
	}
}
