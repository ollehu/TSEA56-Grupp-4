package control;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import view.Animator;

public class MyKeyListener implements KeyListener{
	
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

	private boolean[] keysCurrentlyPressed;
	
	private Animator animator;
	
	private Handler handler;
	
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
		
	}

	@Override
	public void keyPressed(KeyEvent e) {
		keysCurrentlyPressed[e.getKeyCode()] = true;
		animator.updateKeys(keysCurrentlyPressed);
		handler.respondsToKeyEvent(keysCurrentlyPressed);
	}

	@Override
	public void keyReleased(KeyEvent e) {
		keysCurrentlyPressed[e.getKeyCode()] = false;
		animator.updateKeys(keysCurrentlyPressed);
		handler.respondsToKeyEvent(keysCurrentlyPressed);
	}

}
