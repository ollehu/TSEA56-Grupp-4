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
	 * 
	 * @author Isak Stroemberg
	 */
	public void update(int[] sensorValues) {
		int accumulatedRotation = 0;
		if(values != null) {
			accumulatedRotation = values[values.length - 1];
		} 
		
		values = sensorValues;
		
		// take lidar average
		for(int index = 0; index < oldLidarValues.length - 1; index++) {
			oldLidarValues[index + 1] = oldLidarValues[index];
		}
		oldLidarValues[0] = values[4];
		
		int lidarAverage = 0;
		for(int value : oldLidarValues) {
			lidarAverage += value;
		}
		lidarAverage /= oldLidarValues.length;
		
		values[4] = lidarAverage;
		
		values[values.length - 1] += accumulatedRotation;
		
		setChanged();
		notifyObservers(values);
	}
}
