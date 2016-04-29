package view;

import java.awt.GridLayout;

import javax.swing.JPanel;

/**
 * Component containing the map
 * @author isak
 *
 */
public class Map extends JPanel{

	private Tile[][] mapTiles;
	
	private int width = 28;
	private int height = 28;
	
	public Map() {
		mapTiles = new Tile[width][height];
		setLayout(new GridLayout(width,height));
		
		for(int i = width - 1; i >= 0; i--) {
			for(int j = height - 1; j >= 0; j--) {
				mapTiles[j][i] = new Tile();
				add(mapTiles[j][i]);
			}
		}
	}
	
	public void updateMap(int x, int y, int value) {
		mapTiles[x][y].setValue(value);;
	}
	
	public void clearMap() {
		for(Tile tileRow[] : mapTiles) {
			for(Tile tile : tileRow) {
				tile.clear();
			}
		}
	}
}
