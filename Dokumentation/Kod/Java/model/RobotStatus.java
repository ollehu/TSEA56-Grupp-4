package model;

public class RobotStatus {

	private int index;
	
	private int value = -1;
	
	/**
	 * Constructor
	 * @param name
	 */
	public RobotStatus(int index) {
		this.index = index;
	}
	
	//================================================================================
    // Accessors
    //================================================================================
	public void setValue(int value) {
		this.value = value;
	}
	
	public int getIndex() {
		return index;
	}
	
	public int getValue() {
		return value;
	}
}
