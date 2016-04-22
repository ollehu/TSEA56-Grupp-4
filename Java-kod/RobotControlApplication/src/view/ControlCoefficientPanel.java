package view;

import java.awt.Desktop.Action;
import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import jssc.SerialPortException;
import serialCOM.ControlSettingID;
import serialCOM.DataID;

/**
 * Element containing one control coefficient
 * @author isak
 *
 */
public class ControlCoefficientPanel extends JPanel{

	private Animator animator;
	
	private JLabel nameLabel;
	private JTextField valueField;
	
	private String name;
	private double value = 1;
	
	public ControlCoefficientPanel(String name, Animator animator) {
		this.name = name;
		
		nameLabel = new JLabel(name + ": ");
		add(nameLabel);
		
		valueField = new JTextField("" + value);
		add(valueField);
		
		javax.swing.Action action = new AbstractAction() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				value = Double.parseDouble(valueField.getText());
				
				// send new value to robot and write to log
				try {
					animator.getHandler().getSerialPortCOM().sendToRobot(DataID.CONTROL_SETTING, getControlSettingID(), getControlValue());
					animator.getHandler().getLogWriter().appendToLog(name + ": " + value);
				} catch (SerialPortException e1) {
					e1.printStackTrace();
				}
			}
		};
		
		valueField.addActionListener(action);
	}
	
	public void updatePanel(){
		nameLabel.setText(name + ": ");
		valueField.setText("" + value);
	}
	
	void setAutonomousMode(boolean isAutonomousModeOn) {
		valueField.setEditable(isAutonomousModeOn);
	}
	
	private int getControlSettingID() {
		if(name.equals("P")) {
			return ControlSettingID.PROPORTIONAL;
		} else if(name.equals("D")) {
			return ControlSettingID.DERIVATIVE;
		} else if(name.equals("K")) {
			return ControlSettingID.KONSTANT;
		} else if(name.equals("Speed")){
			return ControlSettingID.SPEED;
		} else {
			return -1;

		}
	}
	
	public int getControlValue() {
		if(name == "P") {
			return (int) (value * 100);
		} else if(name == "D") {
			return (int) (value * 100);
		} else if(name == "K") {
			return (int) (value * 10);
		} else if(name == "Speed"){
			return (int) value;
		} else {
			return -1;
		}
	}
	
	
}
