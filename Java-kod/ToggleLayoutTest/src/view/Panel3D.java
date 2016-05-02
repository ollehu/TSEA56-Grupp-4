package view;

import java.awt.GraphicsConfiguration;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Canvas3D;
import javax.media.j3d.DirectionalLight;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Vector3f;

import com.sun.j3d.utils.geometry.Sphere;
import com.sun.j3d.utils.universe.SimpleUniverse;

public class Panel3D extends JPanel{

//	private Adaptive3DPanel adaptive3dPanel;
		
	private JButton discoverElementButton;

	public Panel3D() {
		setLayout(new GridBagLayout());

		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;

		// add 3D-map
//		adaptive3dPanel = new Adaptive3DPanel();
//		add(adaptive3dPanel, constraints);

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
			repaint();
		}
		
	}
}
