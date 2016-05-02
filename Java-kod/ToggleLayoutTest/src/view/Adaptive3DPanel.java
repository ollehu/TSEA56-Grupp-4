package view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GraphicsConfiguration;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.util.ArrayList;
import java.util.concurrent.ThreadLocalRandom;

import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.Canvas3D;
import javax.media.j3d.DirectionalLight;
import javax.media.j3d.Transform3D;
import javax.media.j3d.TransformGroup;
import javax.media.j3d.View;
import javax.security.auth.login.Configuration;
import javax.swing.JPanel;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Vector3d;
import javax.vecmath.Vector3f;

import com.sun.j3d.utils.geometry.ColorCube;
import com.sun.j3d.utils.geometry.Sphere;
import com.sun.j3d.utils.universe.SimpleUniverse;
import com.sun.j3d.utils.universe.Viewer;
import com.sun.j3d.utils.universe.ViewingPlatform;

public class Adaptive3DPanel extends JPanel{

	private Canvas3D canvas3d;
	private SimpleUniverse simpleUniverse;
	private BranchGroup branchGroup;
	private ViewingPlatform viewingPlatform;
	
	private TransformGroup transformGroup;
	private Transform3D transform;
	private Vector3f vector;
	
	private ColorCube cube;
	
	private boolean[][] isElementExplored;
//	private Adaptive3DElement[][] adaptiveElements;
	private GridBagConstraints constraints;
	
	public static final int X_MAX = 29;
	public static final int Y_MAX = 29;
	
	public static final int WIDTH = 600;
	public static final int HEIGHT = 600;
	
	private int exploreCounter = 0;
	private int currentX = 0;
	private int currentY = 0;
	
	private int northMax = 0;
	private int southMax = 0;
	private int westMax = 0;
	private int eastMax = 0;
	
	
	public Adaptive3DPanel() {
		setLayout(new GridBagLayout());
		setPreferredSize(new Dimension(WIDTH, HEIGHT));
		
		// add constraints
		constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		
		// create is element explored-array
		isElementExplored = new boolean[X_MAX][Y_MAX];
		for(int x = 0; x < X_MAX; x++) {
			for(int y = 0; y < Y_MAX; y++) {
				isElementExplored[x][y] = false;
			}
		}
		
		// create and add 3d components
		GraphicsConfiguration configuration = SimpleUniverse.getPreferredConfiguration();
		canvas3d = new Canvas3D(configuration);
		add(canvas3d, constraints);

		// create viewing platform
		viewingPlatform = new ViewingPlatform();
		viewingPlatform.getViewPlatform().setActivationRadius(30f);
		
		TransformGroup viewTransform = viewingPlatform.getViewPlatformTransform();
		Transform3D transform3d = new Transform3D();
		
		transform3d.lookAt(new Point3d(5, 5, 15), new Point3d(0,0,0), new Vector3d(0,1,0));
		transform3d.invert();
		viewTransform.setTransform(transform3d);
		
		Viewer viewer = new Viewer(canvas3d);
		View view = viewer.getView();
		view.setBackClipDistance(30);
		
		// create universe
		simpleUniverse = new SimpleUniverse(viewingPlatform, viewer);

		// create a structure to contain objects
		branchGroup = new BranchGroup();
		
		branchGroup.setCapability(BranchGroup.ALLOW_DETACH);
		branchGroup.setCapability(BranchGroup.ALLOW_CHILDREN_EXTEND);
		branchGroup.setCapability(BranchGroup.ALLOW_CHILDREN_READ);
		branchGroup.setCapability(BranchGroup.ALLOW_CHILDREN_WRITE);

		// create a red light that shines 100m from the origin
		Color3f lightColor = new Color3f(1.8f, 1.8f, 1.8f);

		BoundingSphere bounds = new BoundingSphere(new Point3d(0.0, 0.0, 0.0), 100.0);

		Vector3f lightDirection = new Vector3f(4.0f, -7.0f, -12.0f);

		DirectionalLight light1= new DirectionalLight(lightColor, lightDirection);
		light1.setInfluencingBounds(bounds);
		branchGroup.addChild(light1);

		// add the group of objects to the Universe
		simpleUniverse.addBranchGraph(branchGroup);
		
		cube = new ColorCube(0.5f);
		transformGroup = new TransformGroup();
		transform = new Transform3D();
		BranchGroup temp = new BranchGroup();
		
		vector = new Vector3f(.0f, .0f, .0f);
		transform.setTranslation(vector);
		
		transformGroup.setTransform(transform);
		
		transformGroup.addChild(cube);
		temp.addChild(transformGroup);
		branchGroup.detach();
		branchGroup.addChild(temp);
		
		simpleUniverse.addBranchGraph(branchGroup);
		
		cube = new ColorCube(0.1f);
		transformGroup = new TransformGroup();
		transform = new Transform3D();
		temp = new BranchGroup();
		
		vector = new Vector3f(1.0f, .0f, .0f);
		transform.setTranslation(vector);
		
		transformGroup.setTransform(transform);
		
		transformGroup.addChild(cube);
		temp.addChild(transformGroup);
		branchGroup.detach();
		branchGroup.addChild(temp);
		
		simpleUniverse.addBranchGraph(branchGroup);
		
//		adaptiveElements = new Adaptive3DElement[X_MAX][Y_MAX];
//		for(int x = 0; x < X_MAX; x++) {
//			for(int y = 0; y < Y_MAX; y++) {
//				constraints.gridx = x;
//				constraints.gridy = y;
//				
//				adaptiveElements[x][y] = new Adaptive3DElement();
//			}
//		}
	}
	
	
	public void exploreRandom(){
		// get next coordinate
		int nextX = currentX + ThreadLocalRandom.current().nextInt(-1,1);
		int nextY = currentY + ThreadLocalRandom.current().nextInt(-1,1);
		
//		while(adaptiveElements[nextX][nextY].isExplored()){
//			nextX = currentX + ThreadLocalRandom.current().nextInt(-1,1);
//			nextY = currentY + ThreadLocalRandom.current().nextInt(-1,1);
//		}
		
		while(isElementExplored[nextX][nextY]){
			nextX = currentX + ThreadLocalRandom.current().nextInt(-1,1);
			nextY = currentY + ThreadLocalRandom.current().nextInt(-1,1);
		}
		
		// explore element
		currentX = nextX;
		currentY = nextY;
		
		cube = new ColorCube(0.5f);
		transformGroup = new TransformGroup();
		transform = new Transform3D();
		BranchGroup temp = new BranchGroup();
		
		vector = new Vector3f((float) currentX, (float) currentY, .0f);
		transform.setTranslation(vector);
		
		transformGroup.setTransform(transform);
		
		transformGroup.addChild(cube);
		temp.addChild(transformGroup);
		branchGroup.detach();
		branchGroup.addChild(temp);
		
		simpleUniverse.addBranchGraph(branchGroup);
		
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
		
	}
	
	@Override
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
	}
}
