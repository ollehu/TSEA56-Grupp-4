package view;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JComponent;
import javax.swing.UIManager;

public class Tile extends JComponent{
	
	private int width = 15;
	private int height = 15;
	
	private Color unexploredBackgroundColor = new Color(200, 0, 0);
	private Color exploredBackgroundColor = UIManager.getColor("Panel.background");
	
	private Color unexploredWallColor = new Color(200, 200, 200);
	private Color exploredWallColor = new Color(0, 0, 0);

	/**
	 * 0 = north, 1 = east, 2 = south, 3 = west
	 */
	private boolean[] hasWalls;
	
	private boolean hasBeenExplored = false;
	
	public Tile() {
		setPreferredSize(new Dimension(width + 2, height + 2));
	}
	
	public void explore(boolean[] hasWalls) {
		hasBeenExplored = true;
		this.hasWalls = hasWalls;
	}
	
	@Override
	public void paintComponent(Graphics graphics) {
		Graphics2D graphics2d = (Graphics2D) graphics;
		
		if (hasBeenExplored) {
			graphics2d.setStroke(new BasicStroke(4));
			
			this.setBackground(exploredBackgroundColor);
			graphics2d.setColor(exploredWallColor);
			
			super.paintComponent(graphics);
			
			if(hasWalls[0]){
				graphics2d.drawLine(0, 0, width, 0);
			}
			if(hasWalls[1]){
				graphics2d.drawLine(width, 0, width, height);
			}
			if(hasWalls[2]){
				graphics2d.drawLine(0, height, width, height);
			}
			if(hasWalls[3]){
				graphics2d.drawLine(0, 0, 0, height);
			}
		} else {
			graphics2d.setStroke(new BasicStroke(1));
			this.setBackground(unexploredBackgroundColor);
			graphics2d.setColor(unexploredWallColor);
			
			super.paintComponent(graphics);
			
			graphics2d.fillRect(0, 0, width, height);
		}
	}
	
}
