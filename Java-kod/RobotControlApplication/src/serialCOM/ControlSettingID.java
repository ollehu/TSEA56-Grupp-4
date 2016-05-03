package serialCOM;

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
	
	public static final int SPEED = 6;
	
	/**
	 * Trigger next control decision (only avaialable in debug mode)
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
}

