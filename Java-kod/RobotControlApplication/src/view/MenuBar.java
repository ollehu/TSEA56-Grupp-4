package view;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JSeparator;

import control.ActionHandler;

public class MenuBar extends JMenuBar {

	/**
	 * Contains all actions
	 */
	ActionHandler actionHandler;
	
	/**
	 * File menu with its items
	 */
	private JMenu fileMenu;
	private JMenuItem saveLogMenuItem;
	private JMenuItem commentLogMenuItem;
	
	private JSeparator fileFirstSeparator;
	
	private JMenuItem selectCOMPortMenuItem;

	/**
	 * Edit menu with its items
	 */
	private JMenu editMenu;
	private JCheckBoxMenuItem debugModeMenuItem;
	
	private JSeparator editFirstSeparator;
	
	private JMenuItem clearMapMenuItem;

	/**
	 * Help menu with its items
	 */
	private JMenu helpMenu;
	private JMenuItem displayKeybindingsMenuItem;

	public MenuBar(ActionHandler actionHandler) {
		super();
		this.actionHandler = actionHandler;

		// create file menu and items
		fileMenu = new JMenu("File");
		add(fileMenu);

		saveLogMenuItem = new JMenuItem(actionHandler.saveLogAction);
		fileMenu.add(saveLogMenuItem);

		commentLogMenuItem = new JMenuItem(actionHandler.commentLogAction);
		fileMenu.add(commentLogMenuItem);

		fileFirstSeparator = new JSeparator();
		fileMenu.add(fileFirstSeparator);
		
		selectCOMPortMenuItem = new JMenuItem(actionHandler.selectCOMPortAction);
		fileMenu.add(selectCOMPortMenuItem);


		// create edit menu and items
		editMenu = new JMenu("Edit");
		add(editMenu);

		debugModeMenuItem = new JCheckBoxMenuItem(actionHandler.debugModeAction);
		editMenu.add(debugModeMenuItem);

		editFirstSeparator = new JSeparator();
		editMenu.add(editFirstSeparator);
		
		clearMapMenuItem = new JMenuItem(actionHandler.clearMapAction);
		editMenu.add(clearMapMenuItem);
		
		
		// create help menu and items
		helpMenu = new JMenu("Help");
		add(helpMenu);
		
		displayKeybindingsMenuItem = new JMenuItem(actionHandler.displayKeybindingsAction);
		helpMenu.add(displayKeybindingsMenuItem);
	}

	/**
	 * Sets autonomous mode
	 */
	public void setAutonomousMode(boolean state) {
		debugModeMenuItem.setVisible(state);
		editFirstSeparator.setVisible(state);
	}
	
	/**
	 * Sets debug mode
	 */
	public void setDebugMode(boolean state) {
		saveLogMenuItem.setVisible(state);
		commentLogMenuItem.setVisible(state);
		fileFirstSeparator.setVisible(state);
	}
	
}
