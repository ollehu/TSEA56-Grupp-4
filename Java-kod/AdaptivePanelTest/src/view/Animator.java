package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.security.KeyStore.PrivateKeyEntry;

import javax.swing.Action;
import javax.swing.JButton;
import javax.swing.JFrame;

public class Animator {

	private JFrame frame;
	
	private AdaptivePanel adaptivePanel;
	
	private JButton discoverElementButton;
	
	public Animator() {
		frame = new JFrame("Adaptive panel test");
		frame.setLayout(new GridBagLayout());
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		// add adaptive panel
		adaptivePanel = new AdaptivePanel();
		frame.add(adaptivePanel, constraints);
		
		// add button
		constraints.gridy++;
		constraints.fill = GridBagConstraints.HORIZONTAL;
		discoverElementButton = new JButton("Discover new element");
		discoverElementButton.addActionListener(new DiscoverElementListener());
		frame.add(discoverElementButton, constraints);
		
		frame.pack();
		frame.setVisible(true);
	}
	
	private class DiscoverElementListener implements ActionListener {

		@Override
		public void actionPerformed(ActionEvent e) {
			adaptivePanel.exploreRandom();
			frame.repaint();
		}
		
	}
}
