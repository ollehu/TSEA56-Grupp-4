package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;


public class Panel2D extends JPanel{
	
	private AdaptivePanel adaptivePanel;
	
	private JButton discoverElementButton;
	
	public Panel2D() {
		setLayout(new GridBagLayout());
		
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		// add adaptive panel
		adaptivePanel = new AdaptivePanel();
		add(adaptivePanel, constraints);
		
		// add button
		constraints.gridy++;
		constraints.fill = GridBagConstraints.HORIZONTAL;
		discoverElementButton = new JButton("Discover new element");
		discoverElementButton.addActionListener(new DiscoverElementListener());
		add(discoverElementButton, constraints);
		
	}
	
	private class DiscoverElementListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			adaptivePanel.exploreRandom();
			repaint();
		}
		
	}

}
