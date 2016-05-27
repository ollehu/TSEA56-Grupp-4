package view;

import java.awt.Color;

import javax.swing.JLabel;
import resources.*;


public class RobotStatusLabel extends JLabel{

	private String[] states;
	
	private String name;
	
	private int value = -1;
	
	/**
	 * Constructor
	 * @param name
	 * @param states
	 */
	public RobotStatusLabel(String name, String[] states) {
		super();
		
		this.name = name;
		this.states = states;
		
		setForeground(CustomColors.UNKNOWN_COLOR);
		setText(name + ": unknown");
	}
	
	/**
	 * Converts and sets label value
	 * @param value
	 */
	public void setValue(int value) {
		this.value = value;
		
		if(states != null) {
			if(states.length == 2 && value == 1) {
				setForeground(CustomColors.ON_COLOR);
			} else if (states.length == 2 && value == 0){
				setForeground(CustomColors.OFF_COLOR);
			} else {
				setForeground(Color.BLACK);
			}
			
			try {
				setText(name + ": " + states[value]);
			} catch (Exception e) {
				e.printStackTrace();
			}
		} else {
			setForeground(Color.BLACK);
			
			setText(name + ": " + value);
		}
	}
	
}
