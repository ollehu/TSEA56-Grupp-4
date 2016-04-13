package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.JTextArea;

public class TablePanel extends JPanel{

	private JTextArea tablePlaceHolder;
	
	private ArrayList<SensorLabel> sensorList;
	
	private String[] sensorNames = {"IR F/R", "IR F/L", "IR B/R",
									"IR B/L", "Lidar", "Angular vel"};

	private String[] sensorUnits = {"cm", "cm", "cm",
									"cm", "cm", "deg/s"};
	
	public TablePanel() {
		sensorList = new ArrayList<>();
		
		setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.anchor = GridBagConstraints.FIRST_LINE_START;
		
		for(int index = 0; index < sensorNames.length; index++) {
			sensorList.add(new SensorLabel(sensorNames[index], sensorUnits[index]));
		}
		
		int index = 0;
		for(SensorLabel sensorLabel : sensorList) {
			constraints.gridy = index;
			this.add(sensorLabel, constraints);
			
			index++;
		}
		
	}
}
