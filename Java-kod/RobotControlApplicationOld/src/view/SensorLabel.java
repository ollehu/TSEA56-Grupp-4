package view;

import java.awt.Dimension;

import javax.swing.JLabel;

/**
 * Element containing one sensor data 
 * @author isak
 *
 */
public class SensorLabel extends JLabel{

	private String name;
	private String unit;
	
	private int value = -1;
	
	public SensorLabel(String name, String unit) {
		super();
		
		this.name = name;
		this.unit = unit;
		
		updateText();
	}
	
	public void updateText() {
		setText(name + ": " + value + " " + unit);
	}

	public void setValue(int value) {
		this.value = value;
		updateText();
	}

	public String getName() {
		return name;
	}

	public int getValue() {
		return value;
	}
	
	
	
}
