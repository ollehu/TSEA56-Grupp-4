package control;

import java.awt.Desktop.Action;

import javax.swing.ActionMap;
import javax.swing.InputMap;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JComboBox;
import javax.swing.JPanel;
import javax.swing.KeyStroke;

import view.Animator;


public class KeyHandler {

	private Animator animator;
	
	private ActionHandler actionHandler;
	
	/**
	 * Constructor
	 * @param animator
	 * @param actionHandler
	 */
	public KeyHandler(Animator animator, ActionHandler actionHandler) {
		this.animator = animator;
		this.actionHandler = actionHandler;
		
		// get action map and add actions
		ActionMap actionMap = animator.getRobotStatusPanel().getActionMap();
		
		actionMap.put("forwards", actionHandler.forwardsAction);
		actionMap.put("rotate left", actionHandler.rotateLeftAction);
		actionMap.put("backwards", actionHandler.backwardsAction);
		actionMap.put("rotate right", actionHandler.rotateRightAction);
		actionMap.put("stop", actionHandler.stopAction);
		
		actionMap.put("increase speed", actionHandler.increaseSpeedAction);
		actionMap.put("decrease speed", actionHandler.decreaseSpeedAction);
		actionMap.put("claw", actionHandler.clawAction);
		
		bindTemporaryKeys();
	}
	
	/**
	 * Sets keybinds 
	 * @param state true = keybinds on, false = keybinds off
	 */
	public void setTemporaryKeyBinds(boolean state) {
		if(state) {
			bindTemporaryKeys();
		} else {
			unbindTemporaryKeys();
		}
	}
	
	/**
	 * Binds all temporary keys i.e. those that can be unbound
	 */
	public void bindTemporaryKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
		
		// get input map and add keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
		inputMap.put(KeyStroke.getKeyStroke('w'), "forwards");
		inputMap.put(KeyStroke.getKeyStroke('a'), "rotate left");
		inputMap.put(KeyStroke.getKeyStroke('s'), "backwards");
		inputMap.put(KeyStroke.getKeyStroke('d'), "rotate right");
		
		inputMap.put(KeyStroke.getKeyStroke("released W"), "stop");
		inputMap.put(KeyStroke.getKeyStroke("released A"), "stop");
		inputMap.put(KeyStroke.getKeyStroke("released S"), "stop");
		inputMap.put(KeyStroke.getKeyStroke("released D"), "stop");
		
		inputMap.put(KeyStroke.getKeyStroke('x'), "increase speed");
		inputMap.put(KeyStroke.getKeyStroke('z'), "decrease speed");
		inputMap.put(KeyStroke.getKeyStroke('c'), "claw");
	}
	
	/**
	 * Unbinds all temporary keys
	 */
	public void unbindTemporaryKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
		
		// get input map and remove keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
		inputMap.put(KeyStroke.getKeyStroke('w'), "none");
		inputMap.put(KeyStroke.getKeyStroke('a'), "none");
		inputMap.put(KeyStroke.getKeyStroke('s'), "none");
		inputMap.put(KeyStroke.getKeyStroke('d'), "none");
		
		inputMap.put(KeyStroke.getKeyStroke("released W"), "none");
		inputMap.put(KeyStroke.getKeyStroke("released A"), "none");
		inputMap.put(KeyStroke.getKeyStroke("released S"), "none");
		inputMap.put(KeyStroke.getKeyStroke("released D"), "none");
		
		inputMap.put(KeyStroke.getKeyStroke('x'), "none");
		inputMap.put(KeyStroke.getKeyStroke('z'), "none");
		inputMap.put(KeyStroke.getKeyStroke('c'), "none");
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	private void bindPermanentKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
	
		// get input map and add keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
	}
}
