package control;

import view.Animator;

public class Handler {

	private Animator animator;
	
	public Handler() {
		animator = new Animator();
	}
	
	public void showFrame() {
		animator.showFrame();
	}
}
