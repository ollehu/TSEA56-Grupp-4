package view;

import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.InputMap;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JTable;
import javax.swing.KeyStroke;

public class Animator {

	private JFrame frame;
	
	private JTable table;
	
	private JButton incrementValuesButton;
	
	private String[] columnNames = {"Sensor", "Value", "Unit"};
	
	Object[][] data = {
			{"Lidar", new Integer(-1), "cm"},
			{"IR", new Integer(-1), "cm"},
			{"Angular Velocity", new Integer(-1), "deg/s"}
	};
	
	public Animator() {
		frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLayout(new FlowLayout());

		// add table
		table = new JTable(data, columnNames){
			
			private static final long serialVersionUID = 1L;

			public boolean isCellEditable(int row, int column){
				return false;
			}
		};
		table.setCellSelectionEnabled(false);
		frame.add(table);
		
		// add button
		incrementValuesButton = new JButton("Increment values");
		incrementValuesButton.addActionListener(new IncrementListener());
		frame.add(incrementValuesButton);
		
		// set key bindings
		setKeyBindings();
		
		// show frame
		frame.pack();
		frame.setVisible(true);
	}
	
	private class IncrementListener implements ActionListener{

		@Override
		public void actionPerformed(ActionEvent e) {
			int newValue = (int) table.getValueAt(1, 1) + 1;
			
			table.setValueAt(newValue, 1, 1);
		}
		
	}
	
	private void setKeyBindings() {
		String ACTION_KEY = "theAction";
		KeyStroke space = KeyStroke.getKeyStroke(' ');
		
		InputMap inputMap = incrementValuesButton.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
		inputMap.put(space, ACTION_KEY);
	}
}
