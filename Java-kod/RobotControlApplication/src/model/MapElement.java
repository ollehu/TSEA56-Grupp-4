package model;

public class MapElement {

	/**
	 * Coordinates
	 */
	private int xCoordinate;
	private int yCoordinate;
	
	/**
	 * 245 (0xF5) = unexplored, 244 (0xF4) =  wall, 
	 * 243 (0xF3) = target, 242 = (0xF2) = start, 0-225 = range from start
	 */
	private int value;
	
	/**
	 * Constructor
	 */
	public MapElement(int xCoordinate, int yCoordinate, int value) {
		this.xCoordinate = xCoordinate;
		this.yCoordinate = yCoordinate;
		
		this.value = value;
	}
	
	//================================================================================
    // Accessors
    //================================================================================
	public int getxCoordinate() {
		return xCoordinate;
	}
	
	public int getyCoordinate() {
		return yCoordinate;
	}
	
	public int getValue() {
		return value;
	}
}
