package engine;

public class CollisionBox {
	public float x0, y0, x1, y1;

	public CollisionBox(float x0, float y0, float x1, float y1) {
		this.x0 = x0;
		this.y0 = y0;
		this.x1 = x1;
		this.y1 = y1;
	}
	
	public boolean
	do_overlap(CollisionBox other) {
	    return(	this.x0 < other.x1  &&
	    		this.x1 > other.x0  &&
	    		this.y0 < other.y1  &&
	    		this.y1 > other.y0);
	}
}
