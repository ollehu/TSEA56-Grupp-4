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

public class AdaptiveMapPanel extends JPanel implements Observer{

	private MapVisualElement[][] elementHolder;
	
	/**
	 * Explored visual map elements
	 */
	private ArrayList<MapVisualElement> exploredMapElements;
	
	private GridBagConstraints constraints;
	
	/**
	 * Constants for initiation 
	 */
	public static final int X_MAX = 31;
	public static final int Y_MAX = 18;
	
	public static final int WIDTH_MAX = MapVisualElement.ELEMENT_WIDTH*X_MAX;
	public static final int HEIGHT_MAX = MapVisualElement.ELEMENT_HEIGHT*Y_MAX;
	
	/**
	 * To paint current square (not yet used)
	 */
	private int lastX = 16;
	private int lastY = 1;
	
	/**
	 * Variables for boundaries
	 */
	private int northMax = 1;
	private int southMax = 1;
	private int westMax = 16;
	private int eastMax = 16;
	
	
	public AdaptiveMapPanel() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(WIDTH_MAX, HEIGHT_MAX));
		
		constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.anchor = GridBagConstraints.CENTER;
		constraints.fill = GridBagConstraints.BOTH;
		
		exploredMapElements = new ArrayList<>();
		elementHolder = new MapVisualElement[X_MAX][Y_MAX];
		for(int x = 0; x < X_MAX; x++) {
			for(int y = 0; y < Y_MAX; y++) {
				constraints.gridx = x;
				constraints.gridy = Y_MAX - y + 1;
				
				elementHolder[x][y] = new MapVisualElement();
				add(elementHolder[x][y], constraints);
			}
		}
	}
	
	/**
	 * Sets all MapVisualElements as unexplored
	 */
	public void clear() {
		for(MapVisualElement mapVisualElement: exploredMapElements) {
			mapVisualElement.clear();
		}
		
		// reset boundaries
		lastX = 16;
		lastY = 1;
		
		northMax = 1;
		southMax = 1;
		westMax = 16;
		eastMax = 16;
		
		exploredMapElements.clear();
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
				
				// add the element at the correct x- and y-coordinate
				int xCoordinate = ((MapElement) arg).getxCoordinate();
				int yCoordinate = ((MapElement) arg).getyCoordinate();
				
				constraints.gridx = xCoordinate;
				constraints.gridy = yCoordinate;
				
				if(value == 241) {
					
					elementHolder[lastX][lastY].setCurrent(false);
					elementHolder[xCoordinate][yCoordinate].setCurrent(true);

					lastX = xCoordinate;
					lastY = yCoordinate;

				} else {
					elementHolder[xCoordinate][yCoordinate].explore(value);

				}
				exploredMapElements.add(elementHolder[xCoordinate][yCoordinate]);
				
				// expand boundaries if needed
				if(xCoordinate < westMax) {
					westMax = xCoordinate;
				} else if(xCoordinate > eastMax) {
					eastMax = xCoordinate;
				}
				if(yCoordinate < southMax) {
					southMax = yCoordinate;
				} else if(yCoordinate > northMax) {
					northMax = yCoordinate;
				}
				
				// adjust width and height of all added elements
				int numberOfElementsWidth = eastMax - westMax + 1;
				int numberOfElementsHeight = northMax - southMax + 1;
				
				int nextWidth = WIDTH_MAX / numberOfElementsWidth;
				int nextHeight = HEIGHT_MAX / numberOfElementsHeight;
				
				int nextDimension = Integer.min(nextWidth, nextHeight);
				
				for(MapVisualElement element : exploredMapElements) {
					element.setDimension(nextDimension, nextDimension);
				}
				
				// adjust panel size
				if(numberOfElementsHeight < numberOfElementsWidth) {
					int height = nextDimension * numberOfElementsHeight;
					
					setPreferredSize(new Dimension(WIDTH_MAX, height));

				} else if (numberOfElementsHeight > numberOfElementsWidth){
					int width = nextDimension * numberOfElementsWidth;
					
					setPreferredSize(new Dimension(width, HEIGHT_MAX));

				} else {
					setPreferredSize(new Dimension(WIDTH_MAX, HEIGHT_MAX));
				}
				
				repaint();
			}
		}
	}
}
