package view;

import javax.swing.JPanel;
import javax.swing.JTextArea;

public class GraphPanel extends JPanel{

	private JTextArea graphPlaceHolder;

	public GraphPanel() {
		graphPlaceHolder = new JTextArea(7,40);
		graphPlaceHolder.setEditable(false);
		graphPlaceHolder.setText("This is a placeholder for the map");

		add(graphPlaceHolder);
	}

}
