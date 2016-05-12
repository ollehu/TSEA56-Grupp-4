package model;

import java.util.Observable;

public class SensorData extends Observable{

	/**
	 * Last sensor values
	 */
	private int[] values;
	
	/**
	 * Update sensor values and notify
	 * @param sensorValues
	 */
	public void update(int[] sensorValues) {
		int accumulatedRotation = values[values.length - 1];
		
		values = sensorValues;
		
		values[values.length - 1] += accumulatedRotation;
		
		setChanged();
		notifyObservers(values);
	}
}
