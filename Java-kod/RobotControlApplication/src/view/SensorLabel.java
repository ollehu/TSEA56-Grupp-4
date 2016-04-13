package view;

import javax.swing.JLabel;

public class SensorLabel extends JLabel{

	private String name;
	private String unit;
	
	private double value = -1;
	
	public SensorLabel(String name, String unit) {
		super();
		
		this.name = name;
		this.unit = unit;
		
		updateText();
	}
	
	public void updateText() {
		setText(name + ": " + value + " " + unit);
	}

	public void setValue(double value) {
		this.value = value;
		updateText();
	}
	
}
