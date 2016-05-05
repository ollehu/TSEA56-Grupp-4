package control;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import view.Animator;

/**
 * Handles key presses. Uses a boolean array to keep track of pressed keys, this enable multiple keypresses and held down keys
 * @author isak
 *
 */
public class MyKeyListener implements KeyListener{

	/**
	 * Stores each key's current state (i.e. is key #i currently pressed)
	 */
	private boolean[] keysCurrentlyPressed;
	
	private Animator animator;
	
	private Handler handler;
	
	private boolean isAutonomousModeOn;
	
	public MyKeyListener(Animator animator, Handler handler) {
		this.animator = animator;
		this.handler = handler;
		keysCurrentlyPressed = new boolean[255];
		
		KeyboardFocusManager manager = KeyboardFocusManager.getCurrentKeyboardFocusManager();
		manager.addKeyEventDispatcher(new MyDispatcher());
	}
	
	@Override
	public void keyTyped(KeyEvent e) {
		// TODO Auto-generated method stub
		if(e.getKeyChar() == 'c') {
			handler.toggleClaw();
		}
	}

	/**
	 * Stores keypress if a key is pressed
	 */
	@Override
	public void keyPressed(KeyEvent e) {
		if(!isAutonomousModeOn){
			if(e.getKeyCode() == KeyEvent.VK_UP || e.getKeyCode() ==  KeyEvent.VK_DOWN || 
					e.getKeyCode() == KeyEvent.VK_RIGHT || e.getKeyCode() == KeyEvent.VK_LEFT) {
				keysCurrentlyPressed[e.getKeyCode()] = true;
				animator.updateKeys(keysCurrentlyPressed);
				handler.respondsToKeyEvent(keysCurrentlyPressed);
			}
		}
	}

	/**
	 * Deletes key press if a key is released
	 */
	@Override
	public void keyReleased(KeyEvent e) {
		if(!isAutonomousModeOn) {
			if(e.getKeyCode() == KeyEvent.VK_UP || e.getKeyCode() ==  KeyEvent.VK_DOWN || 
					e.getKeyCode() == KeyEvent.VK_RIGHT || e.getKeyCode() == KeyEvent.VK_LEFT) {
				keysCurrentlyPressed[e.getKeyCode()] = false;
				animator.updateKeys(keysCurrentlyPressed);
				handler.respondsToKeyEvent(keysCurrentlyPressed);
			}
		}
	}

	
	void setAutonomousMode(boolean isAutonomousModeOn) {
		this.isAutonomousModeOn = isAutonomousModeOn;
	}
	
	/**
	 * Dispatches key events. Used to allow user to control robot even if focus is changed
	 * @author isak
	 *
	 */
	private class MyDispatcher implements KeyEventDispatcher {
        @Override
        public boolean dispatchKeyEvent(KeyEvent e) {
            if (e.getID() == KeyEvent.KEY_PRESSED) {
                keyPressed(e);
            } else if (e.getID() == KeyEvent.KEY_RELEASED) {
                keyReleased(e);
            } else if (e.getID() == KeyEvent.KEY_TYPED) {
                keyTyped(e);
            }
            return false;
        }
    }


}
