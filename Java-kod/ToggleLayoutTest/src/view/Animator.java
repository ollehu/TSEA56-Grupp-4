package view;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

public class Animator {
	
	private JFrame frame;
	
	private JTabbedPane tabbedPane;
	
	private Panel2D panel2d;
	private Panel3D panel3d;
	
	public Animator() {
		frame = new JFrame("Toggle Layout Test");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLayout(new GridBagLayout());
	
		
		// create constraints
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		// create tabbedPane
		tabbedPane = new JTabbedPane();
		
		// create and add 2D-panel
		panel2d = new Panel2D();
		tabbedPane.add("2D-panel", panel2d);
		
		// create and add 3D-panel
		panel3d = new Panel3D();
		tabbedPane.add("3D-panel", panel3d);
	
		frame.add(tabbedPane);
		
		// pack and show
		frame.pack();
		frame.setVisible(true);
	}
}
