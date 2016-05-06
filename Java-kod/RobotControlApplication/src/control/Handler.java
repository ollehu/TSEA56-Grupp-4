package control;

import java.util.Observable;
import java.util.Observer;
import java.util.ResourceBundle.Control;

import demo.orsoncharts.swing.ExitOnClose;
import model.*;
import resources.ControlSettingID;
import view.*;

public class Handler implements Observer{

	/**
	 * View items
	 */
	private Animator animator;
	
	/**
	 * Control items
	 */
	private ActionHandler actionHandler;
	private KeyHandler keyHandler;
	private SerialCommunicationHandler serialCommunicationHandler;
	
	/**
	 * Model items
	 */
	private Log log;
	private RobotData robotData;
	private SensorData sensorData;
	private MapData mapData;
	
	/**
	 * Constructor
	 */
	public Handler() {
		// initialize model
		log = new Log();
		robotData = new RobotData();
		sensorData = new SensorData();
		mapData = new MapData();
		
		//initialize serial com
		serialCommunicationHandler = new SerialCommunicationHandler(log, robotData, sensorData, mapData);
		
		/* initialize animator and action handler
		this is split into multiple calls due to
		the way they interact */
		actionHandler = new ActionHandler();
		
		animator = new Animator(this);
		animator.initializeMenuBar();
		animator.showFrame();
		
		actionHandler.initializeActionHandler(animator, log, robotData, serialCommunicationHandler);
		
		// initialize key handler and serial com
		keyHandler = new KeyHandler(animator, actionHandler);
		
		// assign all observers
		assignObservers();
		dataInitialization();
	}
	
	/**
	 * Called on program exit
	 */
	public void exit() {
		log.closeLog(false);
		serialCommunicationHandler.closeSerialPort();
	}
	
	//================================================================================
    // Robot mode handling
    //================================================================================
	/**
	 * Robot status observer
	 * @param o
	 * @param arg
	 */
	@Override
	public void update(Observable o, Object arg) {
		if(o instanceof model.RobotData) {
			if(arg instanceof RobotStatus) {
				int index = ((RobotStatus) arg).getIndex();
				boolean state = ((RobotStatus) arg).getValue() == 1;
				
				if(index == 0) {
					setAutonomousMode(state);
				} else if(index == 1){
					setDebugMode(state);
				}
			}
		}		
	}
	
	/**
	 * Sets autonomous mode
	 * @param state
	 */
	private void setAutonomousMode(boolean state) {
		//TODO add mode handling
		
		// internal handling
		if(state) {
			keyHandler.unbindTemporaryKeys();
		} else {
			keyHandler.bindTemporaryKeys();
			robotData.update(ControlSettingID.DEBUG_MODE, 0);
		}
		
		// external handling
		animator.getRobotStatusPanel().setAutonomousMode(state);
		animator.getMenuBar().setAutonomousMode(state);
	}
	
	/**
	 * Sets debug mode
	 * @param state
	 */
	private void setDebugMode(boolean state) {
		//TODO add mode handling
		
		//internal handling
		if(state) {
			log.createNewLog();
		} else {
			log.closeLog(false);
		}
		
		// external handling
		animator.getRobotStatusPanel().setDebugMode(state);
		animator.getMenuBar().setDebugMode(state);
		animator.getTablePanel().setDebugMode(state);
	}
	
	//================================================================================
    // Accessors
    //================================================================================

	public Animator getAnimator() {
		return animator;
	}
	
	public ActionHandler getActionHandler() {
		return actionHandler;
	}
	
	//================================================================================
    // Internal methods
    //================================================================================
	/**
	 * Assigns observers to the observables
	 */
	private void assignObservers() {
		// add sensor data observers
		sensorData.addObserver(animator.getTablePanel());
		sensorData.addObserver(animator.getGraphPanel());
		
		// add robot status observers
		robotData.addObserver(animator.getRobotStatusPanel());
		robotData.addObserver(this);
		
		// add map data observer
		mapData.addObserver(animator.getMapPanel());
	}
	
	private void dataInitialization() {
		robotData.initialize();
	}
	
	//================================================================================
    // Testing
    //================================================================================
	public void simulateReceivedData() {
		serialCommunicationHandler.simulateReceivedData();
	}
}
