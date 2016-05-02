package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.zip.ZipEntry;

import javax.swing.JPanel;
import javax.swing.JTextArea;

/**
 * Panel containing all SensorLabels and ControlCoefficientPanels
 * @author isak
 *
 */
public class TablePanel extends JPanel{

	private Animator animator;

	private ArrayList<SensorLabel> sensorList;

	private ArrayList<ControlCoefficientPanel> controlList;
	
	private String[] sensorNames = {"IR F/R", "IR F/L", "IR B/R",
			"IR B/L", "Lidar Lite", "Angular velocity"};

	private String[] sensorUnits = {"mm", "mm", "mm",
			"mm", "cm", "deg/s"};

	private String[] controlCoefficients = {"P", "D", "K", "Speed"};

	public TablePanel(Animator animator) {
		this.animator = animator;

		sensorList = new ArrayList<>();
		controlList = new ArrayList<>();

		setLayout(new GridBagLayout());
		GridBagConstraints constraints = new GridBagConstraints();
		//constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.anchor = GridBagConstraints.LINE_START;

		// add sensors to sensor list
		for(int index = 0; index < sensorNames.length; index++) {
			sensorList.add(new SensorLabel(sensorNames[index], sensorUnits[index]));
		}

		// add control coefficients to list
		for(String coeff: controlCoefficients) {
			controlList.add(new ControlCoefficientPanel(coeff, animator));
		}
		
		// add elements in lists to panel
		int index = 0;
		for(SensorLabel sensorLabel : sensorList) {
			constraints.gridy = index;
			this.add(sensorLabel, constraints);

			index++;
		}
		for(ControlCoefficientPanel controlPanel : controlList) {
			constraints.gridy = index;
			this.add(controlPanel, constraints);

			index++;
		}
	}

	public void setAutonomousMode(boolean isAutonomousModeOn) {
		for(ControlCoefficientPanel controlCoefficientPanel : controlList) {
			controlCoefficientPanel.setAutonomousMode(isAutonomousModeOn);
		}
	}

	public void updateSensorValues(int[] sensorValues) {
		int index = 0;
		for(SensorLabel sensor : sensorList) {
			sensor.setValue(sensorValues[index]);
			index++;
		}
	}
	
	public int[] getControllerCoefficients() {
		int[] controllerCoefficients = new int[3];
		
		int index = 0;
		for(ControlCoefficientPanel controlCoefficientPanel : controlList) {
			controllerCoefficients[index] = controlCoefficientPanel.getControlValue();
			index++;
		}
		
		return controllerCoefficients;
	}
	
	public String getLatestSensorValues() {
		String lastSensorValues = "";
		
		for(SensorLabel sensorLabel : sensorList) {
			lastSensorValues += sensorLabel.getName() + ": " + sensorLabel.getValue() + " ";
		}
		
		return lastSensorValues;
	}
	
	public void setDebugMode(boolean state) {
		for(ControlCoefficientPanel coefficientPanel : controlList) {
			coefficientPanel.setVisible(state);
		}
	}
}
