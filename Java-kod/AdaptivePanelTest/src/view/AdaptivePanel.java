package view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.swing.JPanel;

public class AdaptivePanel extends JPanel{

	private AdaptiveElement[][] adaptiveElements;
	private ArrayList<AdaptiveElement> exploredElements;
	private GridBagConstraints constraints;
	
	public static final int X_MAX = 29;
	public static final int Y_MAX = 29;
	
	public static final int WIDTH = 600;
	public static final int HEIGHT = 600;
	
	private int exploreCounter = 0;
	private int currentX = 15;
	private int currentY = 15;
	
	private int northMax = 15;
	private int southMax = 15;
	private int westMax = 15;
	private int eastMax = 15;
	
	
	public AdaptivePanel() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		
		constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		adaptiveElements = new AdaptiveElement[X_MAX][Y_MAX];
		exploredElements = new ArrayList<>();
		for(int x = 0; x < X_MAX; x++) {
			for(int y = 0; y < Y_MAX; y++) {
				constraints.gridx = x;
				constraints.gridy = y;
				
				adaptiveElements[x][y] = new AdaptiveElement();
				add(adaptiveElements[x][y], constraints);
			}
		}
	}
	
	
	public void exploreRandom(){
		// get next coordinate
		int nextX = currentX + ThreadLocalRandom.current().nextInt(-1,1);
		int nextY = currentY + ThreadLocalRandom.current().nextInt(-1,1);
		
		while(adaptiveElements[nextX][nextY].isExplored()){
			nextX = currentX + ThreadLocalRandom.current().nextInt(-1,1);
			nextY = currentY + ThreadLocalRandom.current().nextInt(-1,1);
		}
		
		// explore element
		currentX = nextX;
		currentY = nextY;
		adaptiveElements[nextX][nextY].explore(exploreCounter++);
		exploredElements.add(adaptiveElements[nextX][nextY]);
		
		// change boundaries if needed
		if(currentX < westMax) {
			westMax = currentX;
		} else if(currentX > eastMax) {
			eastMax = currentX;
		}
		if(currentY < southMax) {
			southMax = currentY;
		} else if(currentY > northMax) {
			northMax = currentY;
		}
		
		// adjust dimensions of all explored tiles
		int nextWidth = WIDTH / (eastMax - westMax + 1);
		int nextHeight = HEIGHT / (northMax - southMax + 1);
		
		for(AdaptiveElement element : exploredElements) {
			element.setDimension(nextWidth, nextHeight);
		}
	}
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
	}
}
