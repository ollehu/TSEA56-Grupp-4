package view;

import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

import resources.OtherConstants;

public class TablePanel extends JPanel implements Observer{

	/**
	 * Tables
	 */
	private JTable sensorTable;
	private JTable controlTable;
	
	/**
	 * Constructor
	 */
	public TablePanel() {
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		sensorTable = new JTable(OtherConstants.SENSOR_DATA, OtherConstants.SENSOR_COLUMNS);
		sensorTable.setCellSelectionEnabled(false);
		add(sensorTable);
		
		controlTable = new JTable(OtherConstants.CONTROL_DATA, OtherConstants.CONTROL_COLUMNS);
		controlTable.getModel().addTableModelListener(new TableModelListener() {
			
			@Override
			public void tableChanged(TableModelEvent e) {
				int row = e.getFirstRow();
				int column = e.getColumn();
				TableModel model = (TableModel) e.getSource();
				
				double data = (double) model.getValueAt(row, column);
			}
		});
		add(controlTable);
	}

	/**
	 * Sensor data observer
	 */
	@Override
	public void update(Observable o, Object arg) {
		// check if update is in sensorData
		if(o instanceof model.SensorData) {
			if(arg instanceof int[]) {
				int[] sensorData = (int[]) arg;
				
				int index = 0;
				for(int value : sensorData) {
					sensorTable.setValueAt(value, index, 1);
					index++;
				}
			}
		}
	}
	
	/**
	 * Sets debug mode
	 */
	public void setDebugMode(boolean state) {
		controlTable.setVisible(state);
	}
}
