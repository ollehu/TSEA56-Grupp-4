package view;

import java.awt.Dimension;

import javax.swing.JPanel;
import javax.swing.JTextArea;

/**
 * Panel containing map
 * @author isak
 *
 */
public class MapPanel extends JPanel{

	private Map map;
	
	public MapPanel() {
		map = new Map();
		
		add(map);
		
		boolean[] hasWalls = {true,true,false,false};
		map.explore(1, 1, hasWalls);
		map.explore(1, 3, hasWalls);
		map.explore(4, 1, hasWalls);
		
//		mapPlaceHolder = new JTextArea(20,40);
//		mapPlaceHolder.setEditable(false);
//		mapPlaceHolder.setText("This is a placeholder for the map");
//		
//		add(mapPlaceHolder);
	}
	
}
