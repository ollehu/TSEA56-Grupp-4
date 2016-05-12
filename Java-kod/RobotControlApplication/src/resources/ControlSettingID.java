package resources;

/**
 * List of all control setting IDs
 * @author isak
 *
 */
public class ControlSettingID {

	/**
	 * Autonomous mode on = 1, off = 0
	 */
	public static final int AUTONOMOUS_MODE = 1;
	
	public static final int PROPORTIONAL = 2;
	
	public static final int DERIVATIVE = 3;
	
	public static final int KONSTANT = 4;
	
	public static final int SPEED = 5;
	
	public static final int ROTATION_SPEED = 14;

	/**
	 * Trigger next control decision (only available in debug mode)
	 */
	public static final int NEXT_DECISION = 6;
	
	/**
	 * Debug mode on = 1, off = 0
	 */
	public static final int DEBUG_MODE = 7;
	
	/**
	 * Ping to tell robot that the computer has connected
	 */
	public static final int PING = 8;
	
	public static final int CONSTANT_90 = 9;
	
	public static final int CONSTANT_180 = 10;
	
	public static final int CLAW = 11;
	
	public static final int LAST_CONTROL_COMMAND = 12;
	
	public static final int CURRENT_HEAT = 13;
	
}

