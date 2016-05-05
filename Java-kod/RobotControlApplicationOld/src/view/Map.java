package view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.swing.JPanel;

public class Map extends JPanel{

	private MapVisualElement[][] mapTiles;
	private ArrayList<MapVisualElement> exploredTiles;
	private GridBagConstraints constraints;
	
	public static final int X_MAX = 29;
	public static final int Y_MAX = 29;
	
	public static final int WIDTH = 400;
	public static final int HEIGHT = 400;
	
	private int exploreCounter = 0;
	private int currentX = 15;
	private int currentY = 15;
	
	private int northMax = 15;
	private int southMax = 15;
	private int westMax = 15;
	private int eastMax = 15;
	
	
	public Map() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		
		constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		mapTiles = new MapVisualElement[X_MAX][Y_MAX];
		exploredTiles = new ArrayList<>();
		for(int x = 0; x < X_MAX; x++) {
			for(int y = 0; y < Y_MAX; y++) {
				constraints.gridx = x;
				constraints.gridy = y;
				
				mapTiles[x][y] = new MapVisualElement();
				add(mapTiles[x][y], constraints);
			}
		}
	}
	
	public void clear() {
		for(MapVisualElement tile: exploredTiles) {
			tile.clear();
		}
		
		exploredTiles = new ArrayList<>();
	}
	
	public void update(int nextX, int nextY, int value){
		// explore element
		currentX = nextX;
		currentY = nextY;
		mapTiles[nextX][nextY].explore(value);
		exploredTiles.add(mapTiles[nextX][nextY]);
		
		// change boundaries if needed
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
		
		// adjust dimensions of all explored tiles
		int nextWidth = WIDTH / (eastMax - westMax + 1);
		int nextHeight = HEIGHT / (northMax - southMax + 1);
		
		int nextDimension = Integer.min(nextWidth, nextHeight);
		
		for(MapVisualElement element : exploredTiles) {
			element.setDimension(nextDimension, nextDimension);
		}
	}
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
	}
}
