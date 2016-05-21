package model;

import java.util.Observable;

import view.AdaptiveMapPanel;

public class MapData extends Observable{
	
	private MapElement[][] mapElements;
	
	/**
	 * Constructor
	 */
	public MapData() {
		mapElements = new MapElement[AdaptiveMapPanel.X_MAX][AdaptiveMapPanel.Y_MAX];
	}
	
	/**
	 * Update a map coordinate and notify
	 * @param xCoordinate
	 * @param yCoordinate
	 * @param value
	 */
	public void update(int xCoordinate, int yCoordinate, int value) {
		MapElement mapToUpdate = new MapElement(xCoordinate, yCoordinate, value);
		mapElements[xCoordinate][yCoordinate] = mapToUpdate;
		
		setChanged();
		notifyObservers(mapToUpdate);
	}
	
}
