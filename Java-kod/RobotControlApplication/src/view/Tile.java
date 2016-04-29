package view;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JComponent;
import javax.swing.UIManager;

/**
 * Element containing one map section
 * @author isak
 *
 */
public class Tile extends JComponent{
	
	private int width = 10;
	private int height = 10;
	
	public static final int UNEXPLORED = 245;
	public static final int WALL = 244;
	public static final int TARGET = 243;
	public static final int START = 242;

	
	private Color unexploredBackgroundColor = new Color(110,110,110);
	private Color wallBackgroundColor = new Color(10,10,10);
	private Color exploredBackgroundColor = UIManager.getColor("Panel.background");
	private Color startBackgroundColor = new Color(0, 155, 0);
	private Color targetBackgroundColor = new Color(155, 0, 0);

	/**
	 * 245 (0xF5) = unexplored, 244 (0xF4) =  wall, 
	 * 243 (0xF3) = target, 242 = (0xF2) = start, 0-225 = range from start
	 */
	private int value = UNEXPLORED;
	
	public Tile() {
		setPreferredSize(new Dimension(width + 2, height + 2));
	}
	
	public void setValue(int value) {
		this.value = value;
		repaint();
	}
	
	@Override
	public void paintComponent(Graphics graphics) {
		if(value == UNEXPLORED) {
			graphics.setColor(unexploredBackgroundColor);
		} else if(value == WALL) {
			graphics.setColor(wallBackgroundColor);
		} else if(value == TARGET) {
			graphics.setColor(targetBackgroundColor);
		} else if(value == START) {
			graphics.setColor(startBackgroundColor);
		} else {
			graphics.setColor(exploredBackgroundColor);
		}
		
		super.paintComponent(graphics);
			
		graphics.fillRect(0, 0, width, height);
		
		if(value <= 225) {
			graphics.drawString("" + value, 0, 0);
		}
	}
	
	public void clear() {
		value = UNEXPLORED;
		repaint();
	}
}
