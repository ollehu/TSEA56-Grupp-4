package view;

import javax.swing.JPanel;
import javax.swing.JTextArea;

public class MapPanel extends JPanel{

	private JTextArea mapPlaceHolder;
	
	public MapPanel() {
		mapPlaceHolder = new JTextArea(20,40);
		mapPlaceHolder.setEditable(false);
		mapPlaceHolder.setText("This is a placeholder for the map");
		
		add(mapPlaceHolder);
	}
	
}
