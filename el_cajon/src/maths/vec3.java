package maths;

public class vec3 {
	public float x, y, z;

	public vec3(float x, float y, float z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}
	
	public static vec3 add(vec3 a, vec3 b) {
		return(new vec3(a.x + b.x, a.y + b.y, a.z + b.z));
	}
	
}
