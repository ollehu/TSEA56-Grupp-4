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
		
		for(int index = 0; index < OtherConstants.ROBOT_STATUS_NAMES.length; index++) {
			robotStatus.add(new RobotStatus(index));
		}
	}
	
	public void initialize() {
		update(ControlSettingID.SPEED, 50);
		update(ControlSettingID.DEBUG_MODE, 0);
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
	
	public void toggle(int identifier) {
		RobotStatus statusToUpdate = robotStatus.get(convertIdentifierToIndex(identifier));
		
		int value;
		if(statusToUpdate.getValue() == 1) {
			value = 0;
		} else if((statusToUpdate.getValue() == 0) || (statusToUpdate.getValue() == -1)) {
			value = 1;
		} else {
			return;
		}
		
		statusToUpdate.setValue(value);
		setChanged();
		notifyObservers(statusToUpdate);
	}
	
	//================================================================================
    // Accessors
    //================================================================================
	public int getDebugMode() {
		return robotStatus.get(1).getValue();
	}
	
	public int getSpeed() {
		return robotStatus.get(3).getValue();
	}
	
	public int getLastControlCommand() {
		return robotStatus.get(4).getValue();
	}
	
	public int getControlSetting(int controlSettingID) {
		return robotStatus.get(convertIdentifierToIndex(controlSettingID)).getValue();
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	private int convertIdentifierToIndex(int identifier) {
		if(identifier == ControlSettingID.AUTONOMOUS_MODE) {
			return 0;
		
		} else if (identifier == ControlSettingID.DEBUG_MODE) {
			return 1;		
		
		} else if (identifier == ControlSettingID.CLAW) {
			return 2;		
		
		} else if (identifier == ControlSettingID.SPEED) {
			return 3;		
		
		} else if(identifier == ControlSettingID.LAST_CONTROL_COMMAND) {
			return 4;
		} else {
			return -1;
		}
	}
}
