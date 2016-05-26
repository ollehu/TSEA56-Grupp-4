package view;

import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

import control.ActionHandler;
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
	public TablePanel(ActionHandler actionHandler) {
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		sensorTable = new JTable(OtherConstants.SENSOR_DATA, OtherConstants.SENSOR_COLUMNS);
		sensorTable.setCellSelectionEnabled(false);
		add(sensorTable);
		
		controlTable = new JTable(OtherConstants.CONTROL_DATA, OtherConstants.CONTROL_COLUMNS) {
			
			private static final long serialVersionUID = 1L;

			public boolean isCellEditable(int row, int column) {
		            return column == 1;
			 }
			
		};
		controlTable.getModel().addTableModelListener(actionHandler.controlTableListener);
		controlTable.addFocusListener(new FocusAdapter() {
			@Override
		    public void focusLost(FocusEvent arg0) {
		       controlTable.clearSelection();
		    }
		});
		add(controlTable);
	}

	/**
	 * Sensor data observer
	 * 
	 * @author Isak Stroemberg
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
