package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JComponent;

import com.sun.j3d.utils.geometry.ColorCube;

public class Adaptive3DElement extends ColorCube{
	
	private Color unexploredColor = new Color(200, 200, 200);
	private Color exploredColor = new Color(100, 100, 100);
	
	private int value = 245;
	
	private int width = 15;
	private int height = 15;
	
	public boolean isExplored() {
		return value != 245;
	}
	
	public void explore(int value){
		this.value = value;
	}

}
