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
		values = sensorValues;
		
		setChanged();
		notifyObservers(values);
	}
}
