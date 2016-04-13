package view;

import javax.swing.JPanel;
import javax.swing.JTextArea;

public class TablePanel extends JPanel{

	private JTextArea tablePlaceHolder;

	public TablePanel() {
		tablePlaceHolder = new JTextArea(20,20);
		tablePlaceHolder.setEditable(false);
		tablePlaceHolder.setText("This is a placeholder for the map");

		add(tablePlaceHolder);
	}
}
