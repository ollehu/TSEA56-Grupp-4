package control;

import java.awt.event.ActionEvent;

import javax.swing.AbstractAction;
import javax.swing.JTextArea;
import javax.swing.UIManager;

import resources.*;
import view.*;

public class ActionHandler {

	private Animator animator;
	
	/**
	 * Public actions for use
	 */
	public DisplayKeybindingsAction displayKeybindingsAction = new DisplayKeybindingsAction();
	
	public SaveLogAction saveLogAction;
	public CommentLogAction commentLogAction = new CommentLogAction();
	
	public SelectCOMPortAction selectCOMPortAction = new SelectCOMPortAction();
	
	public DebugModeAction debugModeAction = new DebugModeAction();
	public ClearMapAction clearMapAction = new ClearMapAction();
	
	public SendControlCommandAction rotateLeftAction = new SendControlCommandAction(ControlID.ROTATE_LEFT);
	public SendControlCommandAction rotateRightAction = new SendControlCommandAction(ControlID.ROTATE_RIGHT);
	public SendControlCommandAction forwardsAction = new SendControlCommandAction(ControlID.FORWARDS);
	public SendControlCommandAction backwardsAction = new SendControlCommandAction(ControlID.BACKWARDS);
	
//	public SendControlCommandAction forwardsRightAction = new SendControlCommandAction(ControlID.FORWARDS_RIGHT);
//	public SendControlCommandAction forwardsLeftAction = new SendControlCommandAction(ControlID.FORWARDS_LEFT);
//	public SendControlCommandAction backwardsLeftAction = new SendControlCommandAction(ControlID.BACKWARDS_LEFT);
//	public SendControlCommandAction backwardsRightAction = new SendControlCommandAction(ControlID.BACKWARDS_RIGHT);
	
	public SendControlCommandAction stopAction = new SendControlCommandAction(ControlID.STOP);
	public SendControlCommandAction clawAction = new SendControlCommandAction(ControlID.ROTATE_LEFT);
	
	/**
	 * Constructor
	 * @param handler
	 */
	public ActionHandler(Animator animator) {
		this.animator = animator;
		
		saveLogAction = new SaveLogAction();
		
	}
	
	
	//================================================================================
    // Menu actions
    //================================================================================
	/**
	 * action	
	 * @author isak
	 *
	 */
	private class DisplayKeybindingsAction extends AbstractAction {

		private JTextArea textArea;
		
		public DisplayKeybindingsAction() {
			super("Display keybindings");

			putValue(SHORT_DESCRIPTION, "Displays all hotkeys used by the program");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			//TODO display keybindings popup
		}
		
	}
	
	private class SaveLogAction extends AbstractAction {

		public SaveLogAction() {
			super("Save log", UIManager.getIcon("FileView.floppyDriveIcon"));
			
			putValue(SHORT_DESCRIPTION, "Closes and saves the log after asking for a final comment");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO save log
			animator.simulateReceivedData();
		}
		
	}
	
	private class CommentLogAction extends AbstractAction {

		public CommentLogAction() {
			super("Comment log");
			
			putValue(SHORT_DESCRIPTION, "Appends a comment to the log");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO comment log
		}
		
	}
	
	private class SelectCOMPortAction extends AbstractAction {

		public SelectCOMPortAction() {
			super("Select communications port");
			
			putValue(SHORT_DESCRIPTION, "Select a port for robot communication from the available ports");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			
		}
		
	}
	
	private class DebugModeAction extends AbstractAction {

		public DebugModeAction() {
			super("Debug mode");
			
			putValue(SHORT_DESCRIPTION, "Toggle robot debug mode");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
		}
		
	}
	
	private class ClearMapAction extends AbstractAction {

		public ClearMapAction() {
			super("Clear map");
			
			putValue(SHORT_DESCRIPTION, "Clears the map. Does NOT affect the robot's internal map");
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
		}
		
	}
	
	//================================================================================
    // Robot control actions
    //================================================================================
	private class SendControlCommandAction extends AbstractAction {

		private int controlCommand;
		
		public SendControlCommandAction(int controlCommand) {
			super("Send control command");
			
			this.controlCommand = controlCommand;
			
	
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO send control command to robot
		}
		
	}
	
	private class SendControlSettingAction extends AbstractAction {

		private int controlSetting;
		
		public SendControlSettingAction(int controlSetting) {
			super("Send control setting");
			
			this.controlSetting = controlSetting;
		}
		
		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
		}
		
	}
}
