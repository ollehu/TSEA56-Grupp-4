package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JComponent;
import javax.swing.UIManager;

public class MapVisualElement extends JComponent{
	
	public static final int UNEXPLORED = 245;
	public static final int WALL = 244;
	public static final int TARGET = 243;
	public static final int START = 242;
	
	private Color unexploredBackgroundColor = UIManager.getColor("Panel.background");
	private Color wallBackgroundColor = new Color(10,10,10);
	private Color exploredBackgroundColor = new Color(0, 0, 155);
	private Color startBackgroundColor = new Color(0, 155, 0);
	private Color targetBackgroundColor = new Color(155, 0, 0);
	
	/**
	 * 245 (0xF5) = unexplored, 244 (0xF4) =  wall, 
	 * 243 (0xF3) = target, 242 = (0xF2) = start, 0-225 = range from start
	 */
	private int value = 245;
	
	private int width = 15;
	private int height = 15;
	
	public MapVisualElement() {
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		setVisible(false);
	}
	
	public void explore(int value) {
		this.value = value;
		setVisible(true);
	}
	
	public void clear() {
		this.value = UNEXPLORED;
		setVisible(false);
		repaint();
	}
	
	public boolean isExplored() {
		return value != 245;
	}
	
	public void setDimension(int width, int height) {
		this.width = width;
		this.height = height;
		repaint();
	}
	
	@Override
	protected void paintComponent(Graphics graphics) {
		super.paintComponent(graphics);
		
		if(value <= 225){
			graphics.setColor(exploredBackgroundColor);
		} else if(value == 244){
			graphics.setColor(wallBackgroundColor);
		} else if(value == 243){
			graphics.setColor(targetBackgroundColor);
		} else if(value == 242){
			graphics.setColor(startBackgroundColor);
		} else {
			graphics.setColor(unexploredBackgroundColor);
		}
		
		graphics.fillRect(0, 0, width, height);
	}

}
