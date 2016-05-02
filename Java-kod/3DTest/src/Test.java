import javax.media.j3d.BoundingSphere;
import javax.media.j3d.BranchGroup;
import javax.media.j3d.DirectionalLight;
import javax.vecmath.Color3f;
import javax.vecmath.Point3d;
import javax.vecmath.Vector3f;

import com.sun.j3d.utils.geometry.ColorCube;
import com.sun.j3d.utils.geometry.Sphere;
import com.sun.j3d.utils.universe.SimpleUniverse;

public class Test {

	
	public Test(){
		// create universe
		SimpleUniverse universe = new SimpleUniverse();

		// create a structure to contain objects
		BranchGroup group = new BranchGroup();

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
	
	public static void main(String[] args) {
		new Test();
	}
}
