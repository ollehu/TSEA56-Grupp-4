package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JComponent;

public class AdaptiveElement extends JComponent{
	
	private Color unexploredColor = new Color(200, 200, 200);
	private Color exploredColor = new Color(100, 100, 100);
	
	private int value = 245;
	
	private int width = 15;
	private int height = 15;
	
	public AdaptiveElement() {
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		setVisible(false);
	}
	
	public void explore(int value) {
		this.value = value;
		setVisible(true);
	}
	
	public boolean isExplored() {
		return value != 245;
	}
	
	public void setDimension(int width, int height) {
		this.width = width;
		this.height = height;
	}
	
	@Override
	protected void paintComponent(Graphics graphics) {
		super.paintComponent(graphics);
		
		if(value == 245){
			graphics.setColor(unexploredColor);
		} else {
			graphics.setColor(exploredColor);
		}
		
		graphics.fillRect(0, 0, width, height);
	}

}
