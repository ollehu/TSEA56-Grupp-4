import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.security.Key;
import java.util.ArrayList;

public class MyKeyListener implements KeyListener{

	private boolean[] keysCurrentlyPressed;
	
	private Animator animator;
	
	public MyKeyListener(Animator animator) {
		this.animator = animator;
		keysCurrentlyPressed = new boolean[255];
	}
	
	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void keyPressed(KeyEvent e) {
		keysCurrentlyPressed[e.getKeyCode()] = true;
		animator.print(keysCurrentlyPressed);
	}

	@Override
	public void keyReleased(KeyEvent e) {
		keysCurrentlyPressed[e.getKeyCode()] = false;
		animator.print(keysCurrentlyPressed);
	}

}
