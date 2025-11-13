package maths;

public class mat4 {
	public float data[];
	
	public mat4() {
		data = new float[16];
	}
	
	public mat4(float value) {
		data = new float[16];
		
		data[0]  = value;
		data[5]  = value;
	    data[10] = value;
	    data[15] = value;
	}
	
	public void 
	ortho(float w, float h, float zn, float zf) {
		float frange = 1.0f / (zf - zn);
	    
		data[0] = 2.0f / w;
		data[1] = 0.0f; 
		data[2] = 0.0f; 
		data[3] = 0.0f; 
	  
		data[4] = 0.0f; 
		data[5] = 2.0f / h;
	    data[6] = 0.0f; 
	    data[7] = 0.0f; 
	  
	    data[8] = 0.0f; 
	    data[9] = 0.0f;
	    data[10] = frange;
	    data[11] = 0.0f; 
	  
	    data[12] = 0.0f;
	    data[13] = 0.0f; 
	    data[14] = -frange * zn; 
	    data[15] = 1.0f;
	}

	public void
	multiply(mat4 b) {
	    mat4 c = new mat4();
	    
	    for(int row = 0; row < 4; row++) {
	        for(int col = 0; col < 4; col++) {
	            float sum = 0.0f;
	            for(int e = 0; e < 4; e++) {
	                sum += this.data[e + row * 4] * b.data[col + e * 4];
	            }
	            c.data[col + row * 4] = sum;
	        }
	    }
	    
	    for(int i = 0; i < 16; i++) {
	    	this.data[i] = c.data[i];
	    }
	}
	
	public static mat4
	translate_matrix(vec3 position) {
	    mat4 matrix = new mat4(1.0f);

	    matrix.data[12] = position.x;
	    matrix.data[13] = position.y;
	    matrix.data[14] = position.z;
	    
	    return(matrix);
	}

}
