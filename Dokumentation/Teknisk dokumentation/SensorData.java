package model;

import java.util.Observable;

public class SensorData extends Observable{

	/**
	 * Last sensor values
	 */
	private int[] values;
	private int[] oldLidarValues = new int[5];
	
	/**
	 * Update sensor values and notify
	 * @param sensorValues
	 */
	public void update(int[] sensorValues) {
		int accumulatedRotation = 0;
		if(values != null) {
			accumulatedRotation = values[values.length - 1];
		} 
		
		values = sensorValues;
		
		values[values.length - 1] += accumulatedRotation;
		
		setChanged();
		notifyObservers(values);
	}
}
