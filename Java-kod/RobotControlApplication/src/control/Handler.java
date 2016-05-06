package control;

import java.util.Observable;
import java.util.Observer;

import model.*;
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
	
	private void setAutonomousMode(boolean state) {
		//TODO add mode handling
		
		// internal handling
		if(state) {
			keyHandler.unbindTemporaryKeys();
			System.out.println("Auto on");
		} else {
			keyHandler.unbindTemporaryKeys();
			System.out.println("Auto off");
		}
		
		// external handling
		animator.getRobotStatusPanel().setAutonomousMode(state);
	}
	
	private void setDebugMode(boolean state) {
		//TODO add mode handling
		
		//internal handling
		if(state) {
			System.out.println("Debug on");
		} else {
			System.out.println("Debug off");
		}
		
		// external handling
		animator.getRobotStatusPanel().setDebugMode(state);
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
