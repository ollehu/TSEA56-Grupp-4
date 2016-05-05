package view;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
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
	}
		
	public Map getMap() {
		return map;
	}

}
