package view;

import java.awt.Dimension;
import java.awt.GraphicsConfiguration;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Canvas3D;
import javax.media.j3d.DirectionalLight;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Vector3f;

import com.sun.j3d.utils.geometry.Sphere;
import com.sun.j3d.utils.universe.SimpleUniverse;

public class Test3DMap extends JPanel{

	private Canvas3D canvas3d;

	private SimpleUniverse universe;

	private BranchGroup group;

	public Test3DMap() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(AdaptivePanel.WIDTH, AdaptivePanel.HEIGHT));

		GridBagConstraints constraints = new GridBagConstraints();
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;

		// add 3D-canvas
		GraphicsConfiguration configuration = SimpleUniverse.getPreferredConfiguration();
		canvas3d = new Canvas3D(configuration);
		add(canvas3d, constraints);

		// create universe
		universe = new SimpleUniverse(canvas3d);

		// create a structure to contain objects
		group = new BranchGroup();

		// create a ball and add
		Sphere sphere = new Sphere(0.5f);
		group.addChild(sphere);

		// create a red light that shines 100m from the origin
		Color3f light1Color = new Color3f(1.8f, 0.1f, 0.1f);

		BoundingSphere bounds = new BoundingSphere(new Point3d(0.0, 0.0, 0.0), 100.0);

		Vector3f light1Direction = new Vector3f(4.0f, -7.0f, -12.0f);

		DirectionalLight light1 = new DirectionalLight(light1Color, light1Direction);
		light1.setInfluencingBounds(bounds);
		group.addChild(light1);

		// look towards the ball
		universe.getViewingPlatform().setNominalViewingTransform();

		// add the group of objects to the Universe
		universe.addBranchGraph(group);
	}
	
}
