package view;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;

import control.ActionHandler;
import control.Handler;

public class Animator {

	private Handler handler;

	/**
	 * Main frame
	 */
	private JFrame frame;

	/**
	 * Main frame menu bar
	 */
	private MenuBar menuBar;

	/**
	 * Panel containing robot status
	 */
	private RobotStatusPanel robotStatusPanel;

	/**
	 * Panel containing map
	 */
	private MapPanel mapPanel;

	/**
	 * Panel containing sensor value graphs
	 */
	private GraphPanel graphPanel;

	/**
	 * Panel containing sensor values
	 */
	private TablePanel tablePanel;

	//================================================================================
    // Initialization
    //================================================================================
	/**
	 * Constructor
	 * @param handler
	 */
	public Animator(Handler handler) {
		this.handler = handler;
		
		// create frame
		frame = new JFrame("Robot remote control");
		frame.setLayout(new GridBagLayout());
		
		frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		frame.addWindowListener(new WindowAdapter()
		{
			public void windowClosing(WindowEvent e)
			{
				handler.exit();
				
				System.exit(0);
			}
		});
		
		// create constraints
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.insets = new Insets(2, 2, 2, 2);
		constraints.weightx = 1.0;
		constraints.weighty = 1.0;
		constraints.gridx = 0;
		constraints.gridy = 0;
		constraints.fill = GridBagConstraints.BOTH;
		constraints.anchor = GridBagConstraints.FIRST_LINE_START;
		
		// add map panel
		mapPanel = new MapPanel();
		frame.add(mapPanel, constraints);
		
		// add table panel
		constraints.gridx = 1;
		constraints.fill = GridBagConstraints.VERTICAL;
		constraints.anchor = GridBagConstraints.FIRST_LINE_END;
		tablePanel = new TablePanel(handler.getActionHandler());
		frame.add(tablePanel, constraints);
		
		// add graph panel
		constraints.gridx = 0;
		constraints.gridy = 1;
		constraints.fill = GridBagConstraints.HORIZONTAL;
		constraints.anchor = GridBagConstraints.LAST_LINE_START;
		graphPanel = new GraphPanel();
		frame.add(graphPanel, constraints);
		
		// add robot status panel
		constraints.gridx = 1;
		constraints.fill = GridBagConstraints.BOTH;
		constraints.anchor = GridBagConstraints.LAST_LINE_END;
		robotStatusPanel = new RobotStatusPanel(handler.getActionHandler());
		frame.add(robotStatusPanel, constraints);
	}
	
	/**
	 * Initialized menu bar. Used to call initialization separate from the constructor
	 */
	public void initializeMenuBar() {
		// create menu
		menuBar = new MenuBar(handler.getActionHandler());
		frame.setJMenuBar(menuBar);
	}
	
	/**
	 * Shows the main window
	 */
	public void showFrame() {
		// show frame
		frame.pack();
		frame.setVisible(true);
	}

	//================================================================================
    // Accessors
    //================================================================================
	public RobotStatusPanel getRobotStatusPanel() {
		return robotStatusPanel;
	}
	
	public GraphPanel getGraphPanel() {
		return graphPanel;
	}
	
	public TablePanel getTablePanel() {
		return tablePanel;
	}
	
	public MapPanel getMapPanel() {
		return mapPanel;
	}
	
	public JFrame getFrame() {
		return frame;
	}
	
	public MenuBar getMenuBar() {
		return menuBar;
	}
	
	//================================================================================
    // Testing
    //================================================================================
	public void simulateReceivedData() {
		handler.simulateReceivedData();
	}
	
	public int testReturnValue() {
		return 1;
	}
}
