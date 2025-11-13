package maths;

public class rect {
	public float x, y, w, h;

	public rect(float x, float y, float w, float h) {
		this.x = x;
		this.y = y;
		this.w = w;
		this.h = h;
	}

	public boolean contains_point(float x, float y) {
	    return((x >= this.x) && (x <= this.x + this.w) && (y >= this.y) && (y <= this.y + this.h));
	}

}
