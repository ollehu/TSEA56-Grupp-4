package view;

import java.awt.Graphics;

import javax.swing.JComponent;

import resources.CustomColors;

public class MapVisualElement extends JComponent{

	/**
	 * 245 (0xF5) = unexplored, 244 (0xF4) =  wall, 
	 * 243 (0xF3) = target, 242 (0xF2) = start, 241 (0xF1) = current 
	 * 0-225 = range from start
	 */
	private int value;
	
	public static final int ELEMENT_WIDTH = 15;
	public static final int ELEMENT_HEIGHT = 15;

	private int width = ELEMENT_WIDTH;
	private int height = ELEMENT_HEIGHT;
	
	private boolean isCurrent = false;
	
	/**
	 * Constructor
	 */
	public MapVisualElement() {
		setVisible(false);
	}
	
	public void explore(int value) {
		setVisible(true);
		this.value = value;
	}
	
	public void setCurrent(boolean isCurrent) {
		this.isCurrent = isCurrent;
		repaint();
	}
	
	public void clear() {
		setVisible(false);
	}
	
	public void setDimension(int width, int height) {
		this.width = width;
		this.height = height;
		repaint();
	}
	
	@Override
	protected void paintComponent(Graphics graphics) {
		super.paintComponent(graphics);
		
		if(isCurrent) {
			graphics.setColor(CustomColors.CURRENT_POSITION_COLOR);

		} else {
			if(value <= 225){
				graphics.setColor(CustomColors.EXPLORED_BACKGROUND_COLOR);
			} else if(value == 245){
				graphics.setColor(CustomColors.UNEXPLORED_BACKGROUND_COLOR);
			} else if(value == 244){
				graphics.setColor(CustomColors.WALL_BACKGROUND_COLOR);
			} else if(value == 243){
				graphics.setColor(CustomColors.TARGET_BACKGROUND_COLOR);
			} else if(value == 242){
				graphics.setColor(CustomColors.START_BACKGROUND_COLOR);
			}
		}
		
		graphics.fillRect(0, 0, width, height);
	}
}
