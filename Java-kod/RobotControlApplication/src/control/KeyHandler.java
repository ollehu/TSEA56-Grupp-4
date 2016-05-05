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
	
	//================================================================================
    // Internal methods
    //================================================================================
	private void bindPermanentKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
	
		// get input map and add keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
	}
	
	private void bindTemporaryKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
		
		// get input map and add keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
		inputMap.put(KeyStroke.getKeyStroke('w'), "forwards");
		inputMap.put(KeyStroke.getKeyStroke('a'), "rotate left");
		inputMap.put(KeyStroke.getKeyStroke('s'), "backwards");
		inputMap.put(KeyStroke.getKeyStroke('d'), "rotate right");

	}
	
	private void unbindTemporaryKeys() {
		JPanel bindPanel = animator.getRobotStatusPanel();
		
		// get input map and remove keys		
		InputMap inputMap = bindPanel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
		inputMap.put(KeyStroke.getKeyStroke('w'), "none");
		inputMap.put(KeyStroke.getKeyStroke('a'), "none");
		inputMap.put(KeyStroke.getKeyStroke('s'), "none");
		inputMap.put(KeyStroke.getKeyStroke('d'), "none");
	}
	
}
