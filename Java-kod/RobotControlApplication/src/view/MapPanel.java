package view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;

import model.MapElement;

public class MapPanel extends JPanel implements Observer{

	/**
	 * Explored visual map elements
	 */
	private ArrayList<MapVisualElement> exploredMapElements;
	
	private GridBagConstraints constraints;
	
	/**
	 * Constants for initiation 
	 */
	public static final int X_MAX = 29;
	public static final int Y_MAX = 29;
	
	public static final int WIDTH = 400;
	public static final int HEIGHT = 400;
	
	/**
	 * To paint current square (not yet used)
	 */
	private int currentX = 15;
	private int currentY = 15;
	
	/**
	 * Variables for boundaries
	 */
	private int northMax = 15;
	private int southMax = 15;
	private int westMax = 15;
	private int eastMax = 15;
	
	
	public MapPanel() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		
		constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		exploredMapElements = new ArrayList<>();
	}
	
	public void clear() {
		this.removeAll();
		
		// reset boundaries
		currentX = 15;
		currentY = 15;
		
		northMax = 15;
		southMax = 15;
		westMax = 15;
		eastMax = 15;
	}
	
	/**
	 * Paint this component
	 */
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
	}
	
	/**
	 * Map data observer
	 */
	@Override
	public void update(Observable o, Object arg) {
		if(o instanceof model.MapData) {
			if(arg instanceof MapElement) {
				// create a visual map element with the correct value
				int value = ((MapElement) arg).getValue();
				MapVisualElement nextVisualElement = new MapVisualElement(value);
				
				// add the element at the correct x- and y-coordinate
				int xCoordinate = ((MapElement) arg).getxCoordinate();
				int yCoordinate = ((MapElement) arg).getyCoordinate();
				
				constraints.gridx = xCoordinate;
				constraints.gridy = yCoordinate;
				
				add(nextVisualElement);
				exploredMapElements.add(nextVisualElement);
				
				// expand boundaries if needed
				if(currentX < westMax) {
					westMax = currentX;
				} else if(currentX > eastMax) {
					eastMax = currentX;
				}
				if(currentY < southMax) {
					southMax = currentY;
				} else if(currentY > northMax) {
					northMax = currentY;
				}
				
				// adjust width and height of all added elements
				int nextWidth = WIDTH / (eastMax - westMax + 1);
				int nextHeight = HEIGHT / (northMax - southMax + 1);
				
				int nextDimension = Integer.min(nextWidth, nextHeight);
				
				for(MapVisualElement element : exploredMapElements) {
					element.setDimension(nextDimension, nextDimension);
				}
			}
		}
	}
}
