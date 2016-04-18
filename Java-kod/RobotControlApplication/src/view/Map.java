package view;

import java.awt.GridLayout;

import javax.swing.JPanel;

/**
 * Component containing the map
 * @author isak
 *
 */
public class Map extends JPanel{

	Tile[][] mapTiles;
	
	public Map() {
		mapTiles = new Tile[15][15];
		setLayout(new GridLayout(15, 15));
		
		for(int i = 0; i < 15; i++) {
			for(int j = 0; j < 15; j++) {
				mapTiles[j][i] = new Tile();
				add(mapTiles[j][i]);
			}
		}
	}
	
	public void explore(int x, int y, boolean[] hasWalls) {
		mapTiles[x][y].explore(hasWalls);
	}
	
}
